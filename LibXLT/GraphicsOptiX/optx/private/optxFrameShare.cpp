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
		, m_ColorPtr(0)
		, m_DepthPtr(0)
		, m_Frames(VK_NULL_HANDLE)
		, m_FrameValue(0)
		, m_bWaitPending(false)
		, m_LastReadValue(0)
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

	bool optxFrameShare::Resize(unsigned i_Width, unsigned i_Height)
	{
		if (!make_semaphore())
		{
			return false;
		}
		if (i_Width == m_Width && i_Height == m_Height && m_pColor)
		{
			return true;
		}
		//	CUDA may still write the old buffers; Vulkan's copies from them are
		//	waited for by the deferred destruction of the vkdBuffers
		optxContext::Synchronize();
		m_pColor.reset();
		m_pDepth.reset();
		m_ColorPtr = m_DepthPtr = 0;
		m_Width = m_Height = 0;
		if (i_Width == 0 || i_Height == 0)
		{
			return false;
		}

		const VkDeviceSize bytes = VkDeviceSize(i_Width) * i_Height * 4;
		const VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		m_pColor = gvk::vkdBuffer::Create(m_Device, bytes, usage, "optxFrameShare colour", true);
		m_pDepth = gvk::vkdBuffer::Create(m_Device, bytes, usage, "optxFrameShare depth", true);
		if (!m_pColor || !m_pDepth)
		{
			DBG_LOG("GraphicsOptiX: shared frame buffers of " << i_Width << "x" << i_Height << " couldn't be made");
			m_pColor.reset();
			m_pDepth.reset();
			return false;
		}
		m_ColorPtr = m_Memory.Map(*m_pColor);
		m_DepthPtr = m_Memory.Map(*m_pDepth);
		if (!m_ColorPtr || !m_DepthPtr)
		{
			m_pColor.reset();
			m_pDepth.reset();
			m_ColorPtr = m_DepthPtr = 0;
			return false;
		}
		m_Width = i_Width;
		m_Height = i_Height;
		m_LastReadValue = 0;
		return true;
	}

	void optxFrameShare::Release()
	{
		if (!m_Frames && !m_pColor)
		{
			return;
		}
		optxContext::Synchronize();		// CUDA's signals and writes are done
		m_pColor.reset();				// destroyed once Vulkan is done with them
		m_pDepth.reset();
		m_ColorPtr = m_DepthPtr = 0;
		m_Width = m_Height = 0;
		m_LastReadValue = 0;
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
		if (!m_pColor)
		{
			return false;
		}
		const uint64_t value = m_LastReadValue;
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

	void optxFrameShare::CopyTo(gvk::vkdImage* i_pColor, gvk::vkdImage* i_pDepth)
	{
		if (!m_pColor || !i_pColor)
		{
			return;
		}
		DBG_ASSERT(i_pColor->Width() == m_Width && i_pColor->Height() == m_Height, "optxFrameShare: target size differs");
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		if (m_bWaitPending)
		{
			VkSemaphoreSubmitInfo wait = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
			wait.semaphore = m_Frames;
			wait.value = m_FrameValue;
			wait.stageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
			m_Device.AddSubmitWait(wait);
			m_bWaitPending = false;
		}
		VkCommandBuffer commands = m_Context.Commands();
		VkBufferImageCopy region = {};
		region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageExtent = { m_Width, m_Height, 1 };
		vkCmdCopyBufferToImage(commands, m_pColor->Buffer(), i_pColor->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		if (i_pDepth)
		{
			DBG_ASSERT(i_pDepth->Width() == m_Width && i_pDepth->Height() == m_Height, "optxFrameShare: depth size differs");
			region.imageSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1 };
			vkCmdCopyBufferToImage(commands, m_pDepth->Buffer(), i_pDepth->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		}
		m_LastReadValue = m_Device.PendingValue();
	}
}
