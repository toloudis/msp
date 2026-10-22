/*****************************************************************************
**  vkdBuffer.hpp
**
**      vkdBuffer is a device-local buffer (static vertices, indices,
**	structured data) written from the CPU through the ring buffer, as
**	DX11's default-usage buffers are with initial data or
**	UpdateSubresource. Buffers DX11 maps with WRITE_DISCARD don't use it:
**	they are written to the ring per use (vkdContext::Upload). It has a
**	device address. Made with STORAGE_BUFFER usage, a shader reads it as a
**	StructuredBuffer through ResourceIndex(). Destruction is deferred until
**	the GPU is done with it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDBUFFER_HPP
#error vkdBuffer.hpp multiply included
#endif
#define GVK_VKDBUFFER_HPP

#include <volk.h>

#include <memory>

typedef struct VmaAllocation_T* VmaAllocation;

namespace gvk
{
	class vkdBindings;
	class vkdContext;
	class vkdDevice;

	class vkdBuffer
	{
		public:
			//	i_Usage gets TRANSFER_DST and SHADER_DEVICE_ADDRESS added
			static std::unique_ptr<vkdBuffer> Create(vkdDevice& i_Device, VkDeviceSize i_Size, VkBufferUsageFlags i_Usage,
													 const char* i_Name);
			~vkdBuffer();

			//	Copies i_Size bytes to i_Offset, through the ring buffer, as the
			//	next command of i_Context
			void Upload(vkdContext& i_Context, VkDeviceSize i_Offset, const void* i_pData, VkDeviceSize i_Size);

			VkBuffer		Buffer() const		{ return m_Buffer; }
			VkDeviceAddress	Address() const		{ return m_Address; }
			VkDeviceSize	Size() const		{ return m_Size; }

			//	Its read-only storage-buffer descriptor in i_Bindings' resource
			//	heap (made on first use; one vkdBindings per buffer)
			uint32_t ResourceIndex(vkdBindings& i_Bindings);

		private:
			vkdBuffer(vkdDevice& i_Device) : m_Device(i_Device) {}
			vkdBuffer(const vkdBuffer&) = delete;
			vkdBuffer& operator = (const vkdBuffer&) = delete;

			vkdDevice&		m_Device;
			VkBuffer		m_Buffer = VK_NULL_HANDLE;
			VmaAllocation	m_Allocation = nullptr;
			VkDeviceAddress	m_Address = 0;
			VkDeviceSize	m_Size = 0;
			vkdBindings*	m_pBindings = nullptr;
			uint32_t		m_ResourceIndex = UINT32_MAX;
	};
}
