/*****************************************************************************
**  optxFrameShare.cpp
**
**      See optxFrameShare.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXFRAMESHARE_HPP
#include "GraphicsOptiX/optx/optxFrameShare.hpp"
#endif
#ifndef GOX_CUEXTERNALMEMORYCACHE_HPP
#include "GraphicsOptiX/cu/cuExternalMemoryCache.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <cstring>
#include <mutex>

namespace gox
{
	bool optxFrameShare::GetColorOrder(VkFormat i_Format, optxColorOrder& o_Order)
	{
		switch (i_Format)
		{
			case VK_FORMAT_R8G8B8A8_UNORM:
				o_Order = e_optxColorRGBA;
				return true;
			case VK_FORMAT_B8G8R8A8_UNORM:
				o_Order = e_optxColorBGRA;
				return true;
			default:
				return false;
		}
	}

	bool optxFrameShare::GetDepthLayout(VkFormat i_Format, optxDepthLayout& o_Layout)
	{
		switch (i_Format)
		{
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_X8_D24_UNORM_PACK32:
				o_Layout = e_optxDepthD24;
				return true;
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				o_Layout = e_optxDepthFloat;
				return true;
			default:
				return false;
		}
	}

	optxFrameShare::optxFrameShare(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context, cuExternalMemoryCache& i_Memory,
								   cuExternalSemaphore& i_VulkanTimeline)
		: m_Device(i_Device)
		, m_Context(i_Context)
		, m_Memory(i_Memory)
		, m_VulkanTimeline(i_VulkanTimeline)
		, m_Width(0)
		, m_Height(0)
		, m_Layout(e_Color8)
		, m_Current(0)
		, m_Frames(VK_NULL_HANDLE)
		, m_FrameValue(0)
		, m_bWaitPending(false)
	{
		DBG_ASSERT(m_Device.HasExternalMemory(), "optxFrameShare needs a device with external memory");
	}

	optxFrameShare::~optxFrameShare()
	{
		Release();
	}

	bool optxFrameShare::make_semaphore()
	{
		if (m_Frames)
		{
			return true;
		}
		VkExportSemaphoreCreateInfo export_info = { VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO };
		export_info.handleTypes = gvk::vkdPlatformSemaphoreHandleType();
		VkSemaphoreTypeCreateInfo type_info = { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
		type_info.pNext = &export_info;
		type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		type_info.initialValue = 0;
		VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		info.pNext = &type_info;
		if (vkCreateSemaphore(m_Device.Device(), &info, NULL, &m_Frames) != VK_SUCCESS)
		{
			DBG_LOG("GraphicsOptiX: the frame semaphore couldn't be made");
			m_Frames = VK_NULL_HANDLE;
			return false;
		}
		m_Device.SetName(VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)m_Frames, "optxFrameShare frames");
		if (!m_CudaFrames.Import(m_Device.Device(), m_Frames))
		{
			vkDestroySemaphore(m_Device.Device(), m_Frames, NULL);
			m_Frames = VK_NULL_HANDLE;
			return false;
		}
		m_FrameValue = 0;
		m_bWaitPending = false;
		return true;
	}

	bool optxFrameShare::Resize(unsigned i_Width, unsigned i_Height, ColorLayout i_Layout)
	{
		if (!make_semaphore())
		{
			return false;
		}
		if (i_Width == m_Width && i_Height == m_Height && i_Layout == m_Layout && m_Outputs[0].m_pColor)
		{
			return true;
		}
		//	CUDA may still write the old buffers; Vulkan's copies from them are
		//	waited for by the deferred destruction of the vkdBuffers
		optxContext::Synchronize();
		free_buffers();
		if (i_Width == 0 || i_Height == 0)
		{
			return false;
		}

		const bool half = i_Layout == e_ColorHalf;
		const VkDeviceSize bytes = VkDeviceSize(i_Width) * i_Height * 4;
		const VkDeviceSize color_bytes = half ? 2 * bytes : bytes;
		const VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		for (FrameOutputs& outputs : m_Outputs)
		{
			outputs.m_pColor = gvk::vkdBuffer::Create(m_Device, color_bytes, usage, "optxFrameShare colour", true);
			outputs.m_pDepth = gvk::vkdBuffer::Create(m_Device, bytes, usage, "optxFrameShare depth", true);
			if (!outputs.m_pColor || !outputs.m_pDepth)
			{
				DBG_LOG("GraphicsOptiX: shared frame buffers of " << i_Width << "x" << i_Height << " couldn't be made");
				free_buffers();
				return false;
			}
			outputs.m_ColorPtr = m_Memory.Map(*outputs.m_pColor);
			outputs.m_DepthPtr = m_Memory.Map(*outputs.m_pDepth);
			if (!outputs.m_ColorPtr || !outputs.m_DepthPtr)
			{
				free_buffers();
				return false;
			}
		}
		m_Width = i_Width;
		m_Height = i_Height;
		m_Layout = i_Layout;
		return true;
	}

	void optxFrameShare::free_buffers()
	{
		for (FrameOutputs& outputs : m_Outputs)
		{
			outputs = FrameOutputs();		// the buffers are destroyed once Vulkan is done with them
		}
		m_Current = 0;
		m_Width = m_Height = 0;
	}

	void optxFrameShare::Release()
	{
		if (!m_Frames && !m_Outputs[0].m_pColor)
		{
			return;
		}
		optxContext::Synchronize();		// CUDA's signals and writes are done
		free_buffers();
		m_CudaFrames.Release();
		if (m_Frames)
		{
			//	A submission (or one still to come: a wait added to the next)
			//	may wait on it
			VkDevice device = m_Device.Device();
			VkSemaphore semaphore = m_Frames;
			m_Device.DeferDestroy([device, semaphore]() { vkDestroySemaphore(device, semaphore, NULL); });
			m_Frames = VK_NULL_HANDLE;
		}
		m_FrameValue = 0;
		m_bWaitPending = false;
	}

	bool optxFrameShare::BeginWrite()
	{
		if (!m_Outputs[0].m_pColor)
		{
			return false;
		}
		m_Current = (m_Current + 1) % c_OutputCount;
		const uint64_t value = m_Outputs[m_Current].m_LastReadValue;
		if (value == 0 || m_Device.CompletedValue() >= value)
		{
			return true;
		}
		//	A copy still in the open command buffer: submit it, or CUDA would
		//	wait for a value Vulkan's next submission waits on CUDA to reach
		if (value >= m_Device.PendingValue())
		{
			m_Context.Flush();
		}
		return m_VulkanTimeline.Wait(value);
	}

	bool optxFrameShare::EndWrite()
	{
		if (!m_CudaFrames.IsImported() || !m_CudaFrames.Signal(m_FrameValue + 1))
		{
			return false;
		}
		++m_FrameValue;
		m_bWaitPending = true;
		return true;
	}

	//	The first copy after a write waits for it (stream lock held)
	void optxFrameShare::add_wait()
	{
		if (m_bWaitPending)
		{
			VkSemaphoreSubmitInfo wait = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
			wait.semaphore = m_Frames;
			wait.value = m_FrameValue;
			wait.stageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
			m_Device.AddSubmitWait(wait);
			m_bWaitPending = false;
		}
	}

	void optxFrameShare::copy(const gvk::vkdBuffer& i_Buffer, gvk::vkdImage* i_pImage, VkImageAspectFlags i_Aspect)
	{
		DBG_ASSERT(i_pImage->Width() == m_Width && i_pImage->Height() == m_Height, "optxFrameShare: image size differs");
		VkBufferImageCopy region = {};
		region.imageSubresource = { i_Aspect, 0, 0, 1 };
		region.imageExtent = { m_Width, m_Height, 1 };
		vkCmdCopyBufferToImage(m_Context.Commands(), i_Buffer.Buffer(), i_pImage->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		m_Outputs[m_Current].m_LastReadValue = m_Device.PendingValue();
	}

	void optxFrameShare::CopyTo(gvk::vkdImage* i_pColor, gvk::vkdImage* i_pDepth)
	{
		FrameOutputs& outputs = m_Outputs[m_Current];
		if (!outputs.m_pColor || !i_pColor)
		{
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		add_wait();
		copy(*outputs.m_pColor, i_pColor, VK_IMAGE_ASPECT_COLOR_BIT);
		if (i_pDepth)
		{
			copy(*outputs.m_pDepth, i_pDepth, VK_IMAGE_ASPECT_DEPTH_BIT);
		}
	}

	namespace
	{
		float half_to_float(uint16_t h)
		{
			const uint32_t sign = uint32_t(h & 0x8000u) << 16;
			uint32_t exponent = (h >> 10) & 0x1Fu;
			uint32_t mantissa = h & 0x3FFu;
			uint32_t bits;
			if (exponent == 0)
			{
				if (mantissa == 0)
				{
					bits = sign;
				}
				else
				{
					exponent = 1;
					while (!(mantissa & 0x400u))
					{
						mantissa <<= 1;
						--exponent;
					}
					mantissa &= 0x3FFu;
					bits = sign | ((exponent + 112u) << 23) | (mantissa << 13);
				}
			}
			else if (exponent == 31)
			{
				bits = sign | 0x7F800000u | (mantissa << 13);
			}
			else
			{
				bits = sign | ((exponent + 112u) << 23) | (mantissa << 13);
			}
			float f;
			std::memcpy(&f, &bits, sizeof(f));
			return f;
		}
	}

	std::vector<float> optxFrameShare::read_half(CUdeviceptr i_Ptr) const
	{
		if (!i_Ptr || m_Layout != e_ColorHalf)
		{
			return std::vector<float>();
		}
		std::vector<uint16_t> halves(size_t(m_Width) * m_Height * 4);
		CUstream stream = optxContext::GetStream();
		if (CU_CHECK(cu().cuMemcpyDtoHAsync(halves.data(), i_Ptr, halves.size() * sizeof(uint16_t), stream)) ||
			CU_CHECK(cu().cuStreamSynchronize(stream)))
		{
			return std::vector<float>();
		}
		std::vector<float> values(halves.size());
		for (size_t i = 0; i < halves.size(); ++i)
		{
			values[i] = half_to_float(halves[i]);
		}
		return values;
	}
}
