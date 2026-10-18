/*****************************************************************************
**  vkdBindings.cpp
**
**      See vkdBindings.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <algorithm>
#include <cstring>

namespace gvk
{
	namespace
	{
		//	Binding table layout (see vkdBindings.hpp)
		const uint32_t c_ConstantsOffset = 0;
		const uint32_t c_TexturesOffset = c_ConstantsOffset + vkdBindings::c_ConstantSlots * 8;
		const uint32_t c_SamplersOffset = c_TexturesOffset + vkdBindings::c_TextureSlots * 4;
		const uint32_t c_UavsOffset = c_SamplersOffset + vkdBindings::c_SamplerSlots * 4;
		const uint32_t c_TableSize = c_UavsOffset + vkdBindings::c_UavSlots * 4;

		VkDeviceSize align_up(VkDeviceSize i_Value, VkDeviceSize i_Alignment)
		{
			return (i_Value + i_Alignment - 1) / i_Alignment * i_Alignment;
		}
	}

	vkdBindings::vkdBindings(vkdDevice& i_Device)
		: m_Device(i_Device)
	{
		VkPhysicalDeviceDescriptorHeapPropertiesEXT heap = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_PROPERTIES_EXT};
		VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
		properties.pNext = &heap;
		vkGetPhysicalDeviceProperties2(i_Device.Physical(), &properties);

		const VkDeviceSize image_size = std::max(
			vkGetPhysicalDeviceDescriptorSizeEXT(i_Device.Physical(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE),
			vkGetPhysicalDeviceDescriptorSizeEXT(i_Device.Physical(), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE));
		const VkDeviceSize image_stride = align_up(image_size, heap.imageDescriptorAlignment);
		const VkDeviceSize sampler_stride = align_up(
			vkGetPhysicalDeviceDescriptorSizeEXT(i_Device.Physical(), VK_DESCRIPTOR_TYPE_SAMPLER), heap.samplerDescriptorAlignment);

		create_heap(m_Resources, c_MaxImages, image_stride, heap.minResourceHeapReservedRange,
					heap.resourceHeapAlignment, "vkdBindings resource heap");
		create_heap(m_Samplers, c_MaxSamplers, sampler_stride, heap.minSamplerHeapReservedRange,
					heap.samplerHeapAlignment, "vkdBindings sampler heap");

		m_FreeImages.reserve(c_MaxImages);
		for (uint32_t i = c_MaxImages; i > 0; --i)
		{
			m_FreeImages.push_back(i - 1);		// index 0 first
		}
		build_mappings();
	}

	vkdBindings::~vkdBindings()
	{
		VmaAllocator allocator = m_Device.Allocator();
		for (Heap* heap : {&m_Resources, &m_Samplers})
		{
			VkBuffer buffer = heap->m_Buffer;
			VmaAllocation allocation = heap->m_Allocation;
			m_Device.DeferDestroy([allocator, buffer, allocation]() { vmaDestroyBuffer(allocator, buffer, allocation); });
		}
	}

	void vkdBindings::create_heap(Heap& o_Heap, uint32_t i_Count, VkDeviceSize i_Stride, VkDeviceSize i_Reserved,
								  VkDeviceSize i_Alignment, const char* i_Name)
	{
		o_Heap.m_Stride = i_Stride;
		o_Heap.m_ReservedOffset = align_up(i_Count * i_Stride, std::max<VkDeviceSize>(i_Alignment, 256));
		o_Heap.m_ReservedSize = i_Reserved;
		o_Heap.m_Size = o_Heap.m_ReservedOffset + i_Reserved;

		VkBufferCreateInfo info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		info.size = o_Heap.m_Size;
		info.usage = VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		VmaAllocationCreateInfo allocation_info = {};
		allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocation_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocation_info.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		VmaAllocationInfo mapped = {};
		VK_CHECK(vmaCreateBufferWithAlignment(m_Device.Allocator(), &info, &allocation_info,
											  std::max<VkDeviceSize>(i_Alignment, 65536),
											  &o_Heap.m_Buffer, &o_Heap.m_Allocation, &mapped));
		o_Heap.m_pCpu = static_cast<uint8_t*>(mapped.pMappedData);
		VkBufferDeviceAddressInfo address = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
		address.buffer = o_Heap.m_Buffer;
		o_Heap.m_Gpu = vkGetBufferDeviceAddress(m_Device.Device(), &address);
		m_Device.SetName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<uint64_t>(o_Heap.m_Buffer), i_Name);
	}

	void vkdBindings::build_mappings()
	{
		m_Mappings.clear();
		auto add = [this](uint32_t i_Binding, VkSpirvResourceTypeFlagsEXT i_Mask, VkDescriptorMappingSourceEXT i_Source)
		{
			VkDescriptorSetAndBindingMappingEXT mapping = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_AND_BINDING_MAPPING_EXT};
			mapping.descriptorSet = 0;
			mapping.firstBinding = i_Binding;
			mapping.bindingCount = 1;			// one index (or address) per binding
			mapping.resourceMask = i_Mask;
			mapping.source = i_Source;
			m_Mappings.push_back(mapping);
			return &m_Mappings.back();
		};
		m_Mappings.reserve(c_ConstantSlots + c_TextureSlots + c_SamplerSlots + c_UavSlots);

		for (uint32_t slot = 0; slot < c_ConstantSlots; ++slot)
		{
			VkDescriptorSetAndBindingMappingEXT* m = add(slot, VK_SPIRV_RESOURCE_TYPE_UNIFORM_BUFFER_BIT_EXT,
														 VK_DESCRIPTOR_MAPPING_SOURCE_INDIRECT_ADDRESS_EXT);
			m->sourceData.indirectAddress.pushOffset = 0;
			m->sourceData.indirectAddress.addressOffset = c_ConstantsOffset + slot * 8;
		}
		auto indexed = [&](uint32_t i_Binding, VkSpirvResourceTypeFlagsEXT i_Mask, uint32_t i_TableOffset, VkDeviceSize i_Stride)
		{
			VkDescriptorSetAndBindingMappingEXT* m = add(i_Binding, i_Mask, VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_INDIRECT_INDEX_EXT);
			m->sourceData.indirectIndex.heapOffset = 0;
			m->sourceData.indirectIndex.pushOffset = 0;
			m->sourceData.indirectIndex.addressOffset = i_TableOffset;
			m->sourceData.indirectIndex.heapIndexStride = static_cast<uint32_t>(i_Stride);
			m->sourceData.indirectIndex.heapArrayStride = static_cast<uint32_t>(i_Stride);
		};
		for (uint32_t slot = 0; slot < c_TextureSlots; ++slot)
		{
			indexed(c_TextureBinding + slot,
					VK_SPIRV_RESOURCE_TYPE_SAMPLED_IMAGE_BIT_EXT | VK_SPIRV_RESOURCE_TYPE_READ_ONLY_IMAGE_BIT_EXT,
					c_TexturesOffset + slot * 4, m_Resources.m_Stride);
		}
		for (uint32_t slot = 0; slot < c_SamplerSlots; ++slot)
		{
			indexed(c_SamplerBinding + slot, VK_SPIRV_RESOURCE_TYPE_SAMPLER_BIT_EXT,
					c_SamplersOffset + slot * 4, m_Samplers.m_Stride);
		}
		for (uint32_t slot = 0; slot < c_UavSlots; ++slot)
		{
			indexed(c_UavBinding + slot, VK_SPIRV_RESOURCE_TYPE_READ_WRITE_IMAGE_BIT_EXT,
					c_UavsOffset + slot * 4, m_Resources.m_Stride);
		}
		m_MappingInfo.mappingCount = static_cast<uint32_t>(m_Mappings.size());
		m_MappingInfo.pMappings = m_Mappings.data();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	uint32_t vkdBindings::AddImage(const VkImageViewCreateInfo& i_View, VkDescriptorType i_Type)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		DBG_ASSERT(!m_FreeImages.empty(), "vkdBindings: resource heap full (" << c_MaxImages << " images)");
		const uint32_t index = m_FreeImages.back();
		m_FreeImages.pop_back();

		VkImageDescriptorInfoEXT image = {VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT};
		image.pView = &i_View;
		image.layout = VK_IMAGE_LAYOUT_GENERAL;
		VkResourceDescriptorInfoEXT info = {VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT};
		info.type = i_Type;
		info.data.pImage = &image;
		VkHostAddressRangeEXT destination = {m_Resources.m_pCpu + index * m_Resources.m_Stride,
											 static_cast<size_t>(m_Resources.m_Stride)};
		VK_CHECK(vkWriteResourceDescriptorsEXT(m_Device.Device(), 1, &info, &destination));
		return index;
	}

	void vkdBindings::RemoveImage(uint32_t i_Index)
	{
		m_Device.DeferDestroy([this, i_Index]()
		{
			std::lock_guard<std::mutex> lock(m_Lock);
			m_FreeImages.push_back(i_Index);
		});
	}

	uint32_t vkdBindings::AddSampler(const VkSamplerCreateInfo& i_Info)
	{
		const SamplerKey key(i_Info.magFilter, i_Info.minFilter, i_Info.mipmapMode, i_Info.addressModeU,
							 i_Info.addressModeV, i_Info.addressModeW, i_Info.anisotropyEnable, i_Info.maxAnisotropy,
							 i_Info.compareEnable, i_Info.mipLodBias, i_Info.compareOp, i_Info.borderColor,
							 i_Info.minLod, i_Info.maxLod, i_Info.unnormalizedCoordinates, i_Info.flags);
		std::lock_guard<std::mutex> lock(m_Lock);
		auto found = m_SamplerIndex.find(key);
		if (found != m_SamplerIndex.end())
		{
			return found->second;
		}
		DBG_ASSERT(m_SamplerCount < c_MaxSamplers, "vkdBindings: sampler heap full (" << c_MaxSamplers << " samplers)");
		const uint32_t index = m_SamplerCount++;
		VkHostAddressRangeEXT destination = {m_Samplers.m_pCpu + index * m_Samplers.m_Stride,
											 static_cast<size_t>(m_Samplers.m_Stride)};
		VK_CHECK(vkWriteSamplerDescriptorsEXT(m_Device.Device(), 1, &i_Info, &destination));
		m_SamplerIndex[key] = index;
		return index;
	}

	void vkdBindings::SetConstantBuffer(uint32_t i_Slot, VkDeviceAddress i_Address, VkDeviceSize /*i_Size*/)
	{
		DBG_ASSERT(i_Slot < c_ConstantSlots, "vkdBindings: constant buffer slot b" << i_Slot << " out of range");
		m_Constants[i_Slot] = i_Address;
	}

	void vkdBindings::SetTexture(uint32_t i_Slot, uint32_t i_HeapIndex)
	{
		DBG_ASSERT(i_Slot < c_TextureSlots, "vkdBindings: texture slot t" << i_Slot << " out of range");
		m_Textures[i_Slot] = i_HeapIndex;
	}

	void vkdBindings::SetSampler(uint32_t i_Slot, uint32_t i_HeapIndex)
	{
		DBG_ASSERT(i_Slot < c_SamplerSlots, "vkdBindings: sampler slot s" << i_Slot << " out of range");
		m_SamplerSlots[i_Slot] = i_HeapIndex;
	}

	void vkdBindings::SetUav(uint32_t i_Slot, uint32_t i_HeapIndex)
	{
		DBG_ASSERT(i_Slot < c_UavSlots, "vkdBindings: UAV slot u" << i_Slot << " out of range");
		m_Uavs[i_Slot] = i_HeapIndex;
	}

	void vkdBindings::BindHeaps(VkCommandBuffer i_Commands)
	{
		for (const Heap* heap : {&m_Resources, &m_Samplers})
		{
			VkBindHeapInfoEXT bind = {VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT};
			bind.heapRange.address = heap->m_Gpu;
			bind.heapRange.size = heap->m_Size;
			bind.reservedRangeOffset = heap->m_ReservedOffset;
			bind.reservedRangeSize = heap->m_ReservedSize;
			if (heap == &m_Resources)
			{
				vkCmdBindResourceHeapEXT(i_Commands, &bind);
			}
			else
			{
				vkCmdBindSamplerHeapEXT(i_Commands, &bind);
			}
		}
	}

	void vkdBindings::Apply(VkCommandBuffer i_Commands, vkdRingBuffer& i_Ring)
	{
		vkdRingBuffer::Allocation table = i_Ring.Allocate(c_TableSize, 16);
		uint8_t* p = static_cast<uint8_t*>(table.m_pCpu);
		memcpy(p + c_ConstantsOffset, m_Constants, sizeof(m_Constants));
		memcpy(p + c_TexturesOffset, m_Textures, sizeof(m_Textures));
		memcpy(p + c_SamplersOffset, m_SamplerSlots, sizeof(m_SamplerSlots));
		memcpy(p + c_UavsOffset, m_Uavs, sizeof(m_Uavs));

		VkPushDataInfoEXT push = {VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT};
		push.offset = 0;
		push.data.address = &table.m_Gpu;
		push.data.size = sizeof(table.m_Gpu);
		vkCmdPushDataEXT(i_Commands, &push);
	}
}
