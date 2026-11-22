/*****************************************************************************
**  vkdRingBuffer.cpp
**
**      See vkdRingBuffer.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <algorithm>

namespace gvk
{
	namespace
	{
		const VkDeviceSize c_BlockAlignment = 256;

		VkDeviceSize align_up(VkDeviceSize i_Value, VkDeviceSize i_Alignment)
		{
			return (i_Value + i_Alignment - 1) / i_Alignment * i_Alignment;
		}
	}

	vkdRingBuffer::vkdRingBuffer(vkdDevice& i_Device, VkDeviceSize i_BlockSize)
		: m_Device(i_Device)
		, m_BlockSize(i_BlockSize)
	{
		m_Current = new_block(i_BlockSize);
	}

	vkdRingBuffer::~vkdRingBuffer()
	{
		VmaAllocator allocator = m_Device.Allocator();
		for (const Block& block : m_Blocks)
		{
			VkBuffer buffer = block.m_Buffer;
			VmaAllocation allocation = block.m_Allocation;
			m_Device.DeferDestroy([allocator, buffer, allocation]() { vmaDestroyBuffer(allocator, buffer, allocation); });
		}
	}

	size_t vkdRingBuffer::new_block(VkDeviceSize i_Size)
	{
		Block block;
		block.m_Size = i_Size;
		VkBufferCreateInfo info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		info.size = i_Size;
		info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
					 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
					 VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		VmaAllocationCreateInfo allocation_info = {};
		allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocation_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocation_info.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		VmaAllocationInfo mapped = {};
		VK_CHECK(vmaCreateBufferWithAlignment(m_Device.Allocator(), &info, &allocation_info, c_BlockAlignment,
											  &block.m_Buffer, &block.m_Allocation, &mapped));
		block.m_pCpu = static_cast<uint8_t*>(mapped.pMappedData);
		VkBufferDeviceAddressInfo address = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
		address.buffer = block.m_Buffer;
		block.m_Gpu = vkGetBufferDeviceAddress(m_Device.Device(), &address);
		m_Device.SetName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<uint64_t>(block.m_Buffer), "vkdRingBuffer block");
		m_Blocks.push_back(block);
		return m_Blocks.size() - 1;
	}

	vkdRingBuffer::Allocation vkdRingBuffer::Allocate(VkDeviceSize i_Size, VkDeviceSize i_Alignment)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		const uint64_t pending = m_Device.PendingValue();

		VkDeviceSize offset = align_up(m_Blocks[m_Current].m_Used, i_Alignment);
		if (offset + i_Size > m_Blocks[m_Current].m_Size)
		{
			//	Current block is full: reuse a finished block that's big enough,
			//	or add one.
			const uint64_t completed = m_Device.CompletedValue();
			size_t chosen = m_Blocks.size();
			for (size_t i = 0; i < m_Blocks.size(); ++i)
			{
				if (i != m_Current && m_Blocks[i].m_LastUse <= completed && m_Blocks[i].m_Size >= i_Size)
				{
					chosen = i;
					break;
				}
			}
			if (chosen == m_Blocks.size())
			{
				chosen = new_block(std::max(m_BlockSize, align_up(i_Size, c_BlockAlignment)));
			}
			m_Current = chosen;
			m_Blocks[m_Current].m_Used = 0;
			offset = 0;
		}

		Block& block = m_Blocks[m_Current];
		block.m_Used = offset + i_Size;
		block.m_LastUse = pending;
		Allocation allocation;
		allocation.m_pCpu = block.m_pCpu + offset;
		allocation.m_Gpu = block.m_Gpu + offset;
		allocation.m_Buffer = block.m_Buffer;
		allocation.m_Offset = offset;
		return allocation;
	}

	size_t vkdRingBuffer::BlockCount() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_Blocks.size();
	}
}
