/*****************************************************************************
**  vkdBindings.hpp
**
**      vkdBindings gives shaders their resources the DX11 way, by register
**	slot (b = constant buffers, t = textures, s = samplers, u = UAVs),
**	using VK_EXT_descriptor_heap, without any change to the HLSL.
**
**	How it works:
**	- Image, buffer and sampler descriptors live in one resource heap and
**	  one sampler heap (host-visible buffers, written with
**	  vkWrite*DescriptorsEXT). AddImage/AddBuffer/AddSampler return heap
**	  indices. A StructuredBuffer at a t register is a read-only storage
**	  buffer in the resource heap, its index in the same table slot as a
**	  texture's (Plan 7, decision 2), so each resource-heap entry is as big
**	  as the larger of the two descriptors.
**	- Per draw, Apply() writes a binding table into the ring buffer:
**	    offset   0: 16 x 8-byte constant-buffer addresses (b0..b15)
**	    offset 128: 64 x 4-byte texture heap indices      (t0..t63)
**	    offset 384: 16 x 4-byte sampler heap indices      (s0..s15)
**	    offset 448:  8 x 4-byte UAV heap indices           (u0..u7)
**	  and pushes the table's 8-byte address (vkCmdPushDataEXT, offset 0).
**	- Shaders are compiled by DXC with -fvk-b-shift 0, -fvk-t-shift 100,
**	  -fvk-s-shift 200, -fvk-u-shift 300 (all set 0) and
**	  -fvk-auto-shift-bindings (a resource with no register() gets the
**	  register fxc would give it, shifted). ShaderMapping()
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
			static const uint32_t c_MaxImages = 4096;		// resource heap capacity (images and buffers)
			static const uint32_t c_MaxSamplers = 256;		// sampler heap capacity

			static const uint32_t c_TextureBinding = 100;	// DXC -fvk-t-shift
			static const uint32_t c_SamplerBinding = 200;	// DXC -fvk-s-shift
			static const uint32_t c_UavBinding = 300;		// DXC -fvk-u-shift

			explicit vkdBindings(vkdDevice& i_Device);
			~vkdBindings();

			//	i_Type: VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.
			//	The image is in VK_IMAGE_LAYOUT_GENERAL (unified image layouts).
			uint32_t AddImage(const VkImageViewCreateInfo& i_View, VkDescriptorType i_Type);
			//	A read-only storage buffer (a StructuredBuffer or ByteAddressBuffer
			//	at a t register): i_Size bytes from i_Address
			uint32_t AddBuffer(VkDeviceAddress i_Address, VkDeviceSize i_Size);
			//	Frees an index AddImage or AddBuffer gave, once the GPU is done
			//	with work submitted so far.
			void RemoveResource(uint32_t i_Index);
			//	A buffer of c_NullBufferSize zero bytes, to bind where DX11 code
			//	binds a NULL buffer view, which reads as zeros in D3D11 (reads past
			//	its end are undefined here)
			uint32_t NullBufferIndex() const	{ return m_NullBufferIndex; }
			static const VkDeviceSize c_NullBufferSize = 64 * 1024;
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
			VkBuffer								m_NullBuffer = VK_NULL_HANDLE;
			VmaAllocation							m_NullAllocation = nullptr;
			uint32_t								m_NullBufferIndex = 0;
			//	Free resource-heap indices; shared with the deferred returns of
			//	removed resources, which may run after this object is destroyed
			struct FreeIndices
			{
				std::mutex				m_Lock;
				std::vector<uint32_t>	m_Indices;		// back() is handed out next
			};
			uint32_t take_index();

			std::shared_ptr<FreeIndices>			m_pFreeIndices;
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
