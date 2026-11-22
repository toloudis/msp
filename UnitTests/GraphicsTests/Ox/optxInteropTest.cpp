/*****************************************************************************
**  optxInteropTest.cpp
**
**      GraphicsOptiX on GraphicsVulkan: both APIs on one GPU (the Vulkan
**	device has the CUDA device's UUID; a UUID no adapter has fails device
**	creation, naming who wanted it), a buffer from Vulkan's export pool
**	written by Vulkan and read by CUDA (after waiting on Vulkan's
**	timeline) and the other way round, and a block Vulkan frees forgotten
**	by CUDA.
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#ifndef GOX_CUEXTERNALMEMORYCACHE_HPP
#include "GraphicsOptiX/cu/cuExternalMemoryCache.hpp"
#endif
#ifndef GOX_CUEXTERNALSEMAPHORE_HPP
#include "GraphicsOptiX/cu/cuExternalSemaphore.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDEXPORTPOOL_HPP
#include "GraphicsVulkan/vkd/vkdExportPool.hpp"
#endif

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	class optxInteropTest : public gtOptiX
	{
		protected:
			//	An exportable buffer of i_Bytes Vulkan can copy to and from
			std::unique_ptr<gvk::vkdBuffer> exportable(VkDeviceSize i_Bytes)
			{
				return gvk::vkdBuffer::Create(Device(), i_Bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
											  "optxInteropTest", true);
			}

			//	A buffer's first i_Count texels, read back by Vulkan: copied into
			//	an R8G8B8A8 image i_Count x 1, as OptiX frames reach windows
			std::vector<uint32_t> read_with_vulkan(const gvk::vkdBuffer& i_Buffer, uint32_t i_Count)
			{
				gvk::vkdImage* image = Target(i_Count, 1);
				{
					std::lock_guard<std::recursive_mutex> lock(Device().StreamLock());
					VkBufferImageCopy region = {};
					region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
					region.imageExtent = { i_Count, 1, 1 };
					vkCmdCopyBufferToImage(Context().Commands(), i_Buffer.Buffer(), image->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
				}
				const std::vector<uint8_t> bytes = Read(image);
				std::vector<uint32_t> texels(i_Count);
				std::memcpy(texels.data(), bytes.data(), i_Count * sizeof(uint32_t));
				return texels;
			}
	};

	std::vector<uint32_t> pattern(size_t i_Count, uint32_t i_Seed)
	{
		std::vector<uint32_t> data(i_Count);
		for (size_t i = 0; i < i_Count; ++i)
		{
			data[i] = static_cast<uint32_t>((i + i_Seed) * 2654435761u);
		}
		return data;
	}
}

TEST_F(optxInteropTest, OneGpu)
{
	EXPECT_EQ(optxContext::GetDeviceUUID(), Device().Adapter().m_DeviceUUID);
	EXPECT_TRUE(Device().HasExternalMemory());
	EXPECT_NE(nullptr, Device().ExportPool());
}

TEST_F(optxInteropTest, UnknownUuidFailsWithItsOwnerNamed)
{
	gvk::vkdDeviceOptions options;
	options.m_RequiredDeviceUUID = std::array<uint8_t, 16>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	options.m_RequiredDeviceName = "the test's device";
	std::string error;
	std::unique_ptr<gvk::vkdDevice> device(gvk::vkdDevice::Create(options, error));
	EXPECT_EQ(nullptr, device);
	EXPECT_NE(std::string::npos, error.find("No Vulkan device is the test's device")) << error;
}

TEST_F(optxInteropTest, VulkanWritesCudaReads)
{
	const std::vector<uint32_t> data = pattern(4096, 7);
	std::unique_ptr<gvk::vkdBuffer> buffer = exportable(data.size() * sizeof(uint32_t));
	ASSERT_TRUE(buffer->IsExportable());
	uint64_t value = 0;
	{
		std::lock_guard<std::recursive_mutex> lock(Device().StreamLock());
		buffer->Upload(Context(), 0, data.data(), data.size() * sizeof(uint32_t));
		value = Context().Flush();
	}

	const CUdeviceptr ptr = Cache().GetMemoryCache().Map(*buffer);
	ASSERT_NE(0u, ptr);
	cuExternalSemaphore& timeline = Cache().GetVulkanTimeline();
	ASSERT_TRUE(timeline.IsImported());
	ASSERT_TRUE(timeline.Wait(value));
	std::vector<uint32_t> back(data.size());
	ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoHAsync(back.data(), ptr, back.size() * sizeof(uint32_t), optxContext::GetStream()));
	ASSERT_TRUE(optxContext::Synchronize());
	EXPECT_EQ(data, back);

	//	A second buffer in the same block: no second import
	std::unique_ptr<gvk::vkdBuffer> other = exportable(256);
	EXPECT_EQ(buffer->Memory(), other->Memory());
	EXPECT_EQ(ptr - buffer->MemoryOffset() + other->MemoryOffset(), Cache().GetMemoryCache().Map(*other));
	EXPECT_EQ(1, Cache().GetMemoryCache().GetImportCount());
}

TEST_F(optxInteropTest, CudaWritesVulkanReads)
{
	const std::vector<uint32_t> data = pattern(64, 3);
	std::unique_ptr<gvk::vkdBuffer> buffer = exportable(data.size() * sizeof(uint32_t));
	const CUdeviceptr ptr = Cache().GetMemoryCache().Map(*buffer);
	ASSERT_NE(0u, ptr);
	ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyHtoD(ptr, data.data(), data.size() * sizeof(uint32_t)));
	ASSERT_TRUE(optxContext::Synchronize());
	EXPECT_EQ(data, read_with_vulkan(*buffer, uint32_t(data.size())));
}

TEST_F(optxInteropTest, FreedBlockIsForgotten)
{
	//	Big enough for memory of its own, freed with the buffer
	std::unique_ptr<gvk::vkdBuffer> buffer = exportable(gvk::vkdExportPool::c_DedicatedSize + 4096);
	EXPECT_TRUE(buffer->IsDedicatedMemory());
	ASSERT_NE(0u, Cache().GetMemoryCache().Map(*buffer));
	EXPECT_EQ(1u, Cache().GetMemoryCache().GetBlockCount());

	buffer.reset();
	Context().Flush();
	Device().WaitIdle();	// runs the deferred destruction
	EXPECT_EQ(0u, Cache().GetMemoryCache().GetBlockCount());
}
