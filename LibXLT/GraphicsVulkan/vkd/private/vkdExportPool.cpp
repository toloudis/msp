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
		return pool;
	}

	vkdExportPool::~vkdExportPool()
	{
		if (m_Pool)
		{
			vmaDestroyPool(m_Allocator, m_Pool);
		}
	}
}
