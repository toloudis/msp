/*****************************************************************************
**  vkdBindings.hpp
**
**      vkdBindings gives shaders their resources the DX11 way, by register
**	slot (b = constant buffers, t = textures, s = samplers, u = UAVs),
**	using VK_EXT_descriptor_heap, without any change to the HLSL.
**
**	How it works:
**	- Image and sampler descriptors live in one resource heap and one
**	  sampler heap (host-visible buffers, written with
**	  vkWrite*DescriptorsEXT). AddImage/AddSampler return heap indices.
**	- Per draw, Apply() writes a binding table into the ring buffer:
**	    offset   0: 16 x 8-byte constant-buffer addresses (b0..b15)
**	    offset 128: 64 x 4-byte texture heap indices      (t0..t63)
**	    offset 384: 16 x 4-byte sampler heap indices      (s0..s15)
**	    offset 448:  8 x 4-byte UAV heap indices           (u0..u7)
**	  and pushes the table's 8-byte address (vkCmdPushDataEXT, offset 0).
**	- Shaders are compiled by DXC with -fvk-b-shift 0, -fvk-t-shift 100,
**	  -fvk-s-shift 200, -fvk-u-shift 300 (all set 0). ShaderMapping()
**	  maps each of those bindings to the table: constant buffers with
**	  VK_DESCRIPTOR_MAPPING_SOURCE_INDIRECT_ADDRESS_EXT, images and
**	  samplers with VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_INDIRECT_INDEX_EXT.
**	  Pipelines chain it to every stage, set
**	  VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT and have no layout.
**
**	Why a table instead of pushing indices: the push data limit (256 bytes
**	on the target GPU) is smaller than every slot the shaders use. The
**	table also lets Phase 2 keep per-object tables alive across frames.
**
**	Decision (Plan 2, Task 4): this design was verified on a Quadro RTX
**	5000 (driver 596.59) with core, synchronization and GPU-assisted
**	validation clean (vkdBindingsTest). The fallbacks in the spec
**	(descriptor buffers, then push descriptors) were not needed.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDBINDINGS_HPP
#error vkdBindings.hpp multiply included
#endif
#define GVK_VKDBINDINGS_HPP

#include <volk.h>

#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

typedef struct VmaAllocation_T* VmaAllocation;

namespace gvk
{
	class vkdDevice;
	class vkdRingBuffer;

	class vkdBindings
	{
		public:
			static const uint32_t c_ConstantSlots = 16;
			static const uint32_t c_TextureSlots = 64;
			static const uint32_t c_SamplerSlots = 16;
			static const uint32_t c_UavSlots = 8;
			static const uint32_t c_MaxImages = 4096;		// resource heap capacity
			static const uint32_t c_MaxSamplers = 256;		// sampler heap capacity

			static const uint32_t c_TextureBinding = 100;	// DXC -fvk-t-shift
			static const uint32_t c_SamplerBinding = 200;	// DXC -fvk-s-shift
			static const uint32_t c_UavBinding = 300;		// DXC -fvk-u-shift

			explicit vkdBindings(vkdDevice& i_Device);
			~vkdBindings();

			//	i_Type: VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.
			//	The image is in VK_IMAGE_LAYOUT_GENERAL (unified image layouts).
			uint32_t AddImage(const VkImageViewCreateInfo& i_View, VkDescriptorType i_Type);
			//	Frees the index once the GPU is done with work submitted so far.
			void RemoveImage(uint32_t i_Index);
			//	Same state, same index.
			uint32_t AddSampler(const VkSamplerCreateInfo& i_Info);

			void SetConstantBuffer(uint32_t i_Slot, VkDeviceAddress i_Address, VkDeviceSize i_Size);
			void SetTexture(uint32_t i_Slot, uint32_t i_HeapIndex);
			void SetSampler(uint32_t i_Slot, uint32_t i_HeapIndex);
			void SetUav(uint32_t i_Slot, uint32_t i_HeapIndex);

			//	Binds both heaps (once per command buffer, before Apply).
			void BindHeaps(VkCommandBuffer i_Commands);
			//	Writes the binding table for the next draw/dispatch and pushes its address.
			void Apply(VkCommandBuffer i_Commands, vkdRingBuffer& i_Ring);

			//	Chain into VkPipelineShaderStageCreateInfo::pNext of every stage.
			const VkShaderDescriptorSetAndBindingMappingInfoEXT& ShaderMapping() const { return m_MappingInfo; }

		private:
			struct Heap
			{
				VkBuffer		m_Buffer = VK_NULL_HANDLE;
				VmaAllocation	m_Allocation = nullptr;
				uint8_t*		m_pCpu = nullptr;
				VkDeviceAddress	m_Gpu = 0;
				VkDeviceSize	m_Size = 0;
				VkDeviceSize	m_ReservedOffset = 0;
				VkDeviceSize	m_ReservedSize = 0;
				VkDeviceSize	m_Stride = 0;
			};

			void create_heap(Heap& o_Heap, uint32_t i_Count, VkDeviceSize i_Stride, VkDeviceSize i_Reserved,
							 VkDeviceSize i_Alignment, const char* i_Name);
			void build_mappings();

			typedef std::tuple<int, int, int, int, int, int, int, float, int, float, int, int, float, float, int, int> SamplerKey;

			vkdDevice&								m_Device;
			Heap									m_Resources;
			Heap									m_Samplers;
			//	Free resource-heap indices; shared with the deferred returns of
			//	removed images, which may run after this object is destroyed
			struct FreeImages
			{
				std::mutex				m_Lock;
				std::vector<uint32_t>	m_Indices;		// back() is handed out next
			};
			std::shared_ptr<FreeImages>				m_pFreeImages;
			uint32_t								m_SamplerCount = 0;
			std::map<SamplerKey, uint32_t>			m_SamplerIndex;
			std::mutex								m_Lock;

			VkDeviceAddress							m_Constants[c_ConstantSlots] = {};
			uint32_t								m_Textures[c_TextureSlots] = {};
			uint32_t								m_SamplerSlots[c_SamplerSlots] = {};
			uint32_t								m_Uavs[c_UavSlots] = {};

			std::vector<VkDescriptorSetAndBindingMappingEXT>	m_Mappings;
			VkShaderDescriptorSetAndBindingMappingInfoEXT		m_MappingInfo = {VK_STRUCTURE_TYPE_SHADER_DESCRIPTOR_SET_AND_BINDING_MAPPING_INFO_EXT};
	};
}
