/*****************************************************************************
**  vkdBuffer.cpp
**
**      See vkdBuffer.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
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

namespace gvk
{
	std::unique_ptr<vkdBuffer> vkdBuffer::Create(vkdDevice& i_Device, VkDeviceSize i_Size, VkBufferUsageFlags i_Usage,
												 const char* i_Name)
	{
		std::unique_ptr<vkdBuffer> buffer(new vkdBuffer(i_Device));
		buffer->m_Size = i_Size;

		VkBufferCreateInfo info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		info.size = i_Size;
		info.usage = i_Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo allocation = {};
		allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		VK_CHECK(vmaCreateBuffer(i_Device.Allocator(), &info, &allocation, &buffer->m_Buffer, &buffer->m_Allocation, NULL));
		i_Device.SetName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<uint64_t>(buffer->m_Buffer), i_Name);

		VkBufferDeviceAddressInfo address = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
		address.buffer = buffer->m_Buffer;
		buffer->m_Address = vkGetBufferDeviceAddress(i_Device.Device(), &address);
		return buffer;
	}

	uint32_t vkdBuffer::ResourceIndex(vkdBindings& i_Bindings)
	{
		DBG_ASSERT(!m_pBindings || m_pBindings == &i_Bindings, "vkdBuffer: registered with another vkdBindings");
		if (m_ResourceIndex == UINT32_MAX)
		{
			m_pBindings = &i_Bindings;
			m_ResourceIndex = i_Bindings.AddBuffer(m_Address, m_Size);
		}
		return m_ResourceIndex;
	}

	vkdBuffer::~vkdBuffer()
	{
		//	Not bound to an effect any more (fxEffectVk)
		m_Device.BufferDestroyed(this);
		if (m_pBindings)
		{
			m_pBindings->RemoveResource(m_ResourceIndex);
		}
		VmaAllocator allocator = m_Device.Allocator();
		VkBuffer buffer = m_Buffer;
		VmaAllocation allocation = m_Allocation;
		m_Device.DeferDestroy([allocator, buffer, allocation]()
		{
			vmaDestroyBuffer(allocator, buffer, allocation);
		});
	}

	void vkdBuffer::Upload(vkdContext& i_Context, VkDeviceSize i_Offset, const void* i_pData, VkDeviceSize i_Size)
	{
		DBG_ASSERT(i_Offset + i_Size <= m_Size, "vkdBuffer::Upload: " << i_Offset << " + " << i_Size << " is past the end (" << m_Size << ")");
		if (i_Size == 0)
		{
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		vkdRingBuffer::Allocation staging = i_Context.Upload(i_pData, i_Size, 16);
		VkBufferCopy region = { staging.m_Offset, i_Offset, i_Size };
		vkCmdCopyBuffer(i_Context.Commands(), staging.m_Buffer, m_Buffer, 1, &region);
	}
}
