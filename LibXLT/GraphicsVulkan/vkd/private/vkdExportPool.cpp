/*****************************************************************************
**  vkdExportPool.cpp
**
**      See vkdExportPool.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDEXPORTPOOL_HPP
#include "GraphicsVulkan/vkd/vkdExportPool.hpp"
#endif
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

namespace gvk
{
	std::unique_ptr<vkdExportPool> vkdExportPool::Create(VmaAllocator i_Allocator, std::string& o_Error)
	{
		std::unique_ptr<vkdExportPool> pool(new vkdExportPool);
		pool->m_Allocator = i_Allocator;
		pool->m_ExportInfo.handleTypes = vkdPlatformMemoryHandleType();
		pool->m_BufferInfo.handleTypes = vkdPlatformMemoryHandleType();
		pool->m_ImageInfo.handleTypes = vkdPlatformMemoryHandleType();

		//	The memory type for what the pool holds: mesh buffers (vertices,
		//	indices) and the frame buffers OptiX writes (storage, copies)
		VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		buffer_info.pNext = &pool->m_BufferInfo;
		buffer_info.size = 65536;
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
							VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
							VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo allocation = {};
		allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocation.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		uint32_t memory_type = 0;
		VkResult result = vmaFindMemoryTypeIndexForBufferInfo(i_Allocator, &buffer_info, &allocation, &memory_type);
		if (result != VK_SUCCESS)
		{
			o_Error = std::string("no device-local memory type for exportable buffers: ") + vkdResultName(result);
			return NULL;
		}

		VmaPoolCreateInfo pool_info = {};
		pool_info.memoryTypeIndex = memory_type;
		pool_info.blockSize = 0;	// VMA's default, which allows dedicated allocations
		pool_info.pMemoryAllocateNext = &pool->m_ExportInfo;
		result = vmaCreatePool(i_Allocator, &pool_info, &pool->m_Pool);
		if (result != VK_SUCCESS)
		{
			o_Error = std::string("vmaCreatePool failed for exportable memory: ") + vkdResultName(result);
			return NULL;
		}
		vmaSetPoolName(i_Allocator, pool->m_Pool, "vkdExportPool");

		//	The memory type for exportable textures (sampled, uploaded, mips
		//	blitted, rendered into); without one, images aren't exported
		VkImageCreateInfo image_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
		image_info.pNext = &pool->m_ImageInfo;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_info.extent = { 256, 256, 1 };
		image_info.mipLevels = 9;
		image_info.arrayLayers = 1;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
						   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		uint32_t image_type = 0;
		if (vmaFindMemoryTypeIndexForImageInfo(i_Allocator, &image_info, &allocation, &image_type) == VK_SUCCESS)
		{
			VmaPoolCreateInfo image_pool = {};
			image_pool.memoryTypeIndex = image_type;
			image_pool.pMemoryAllocateNext = &pool->m_ExportInfo;
			if (vmaCreatePool(i_Allocator, &image_pool, &pool->m_ImagePool) == VK_SUCCESS)
			{
				vmaSetPoolName(i_Allocator, pool->m_ImagePool, "vkdExportPool images");
			}
			else
			{
				pool->m_ImagePool = nullptr;
			}
		}
		return pool;
	}

	vkdExportPool::~vkdExportPool()
	{
		if (m_ImagePool)
		{
			vmaDestroyPool(m_Allocator, m_ImagePool);
		}
		if (m_Pool)
		{
			vmaDestroyPool(m_Allocator, m_Pool);
		}
	}
}
