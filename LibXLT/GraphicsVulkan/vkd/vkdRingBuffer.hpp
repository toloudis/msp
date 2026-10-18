/*****************************************************************************
**  vkdRingBuffer.hpp
**
**      vkdRingBuffer hands out short-lived GPU-visible memory written by the
**	CPU: constants, dynamic vertices, per-draw binding tables, upload
**	staging. It replaces DX11's Map(WRITE_DISCARD). Memory is
**	host-visible, coherent and has a device address. Blocks are filled
**	front to back; a full block is reused only once the GPU has finished
**	every submission that may read it, and a new block is added when none
**	is free (so data in flight is never overwritten).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDRINGBUFFER_HPP
#error vkdRingBuffer.hpp multiply included
#endif
#define GVK_VKDRINGBUFFER_HPP

#include <volk.h>

#include <mutex>
#include <vector>

typedef struct VmaAllocation_T* VmaAllocation;

namespace gvk
{
	class vkdDevice;

	class vkdRingBuffer
	{
		public:
			//	i_BlockSize: bytes per block (larger allocations get their own block).
			vkdRingBuffer(vkdDevice& i_Device, VkDeviceSize i_BlockSize);
			~vkdRingBuffer();

			struct Allocation
			{
				void*			m_pCpu;		// write here
				VkDeviceAddress	m_Gpu;		// the GPU reads here
				VkBuffer		m_Buffer;	// for copies and bindings
				VkDeviceSize	m_Offset;	// within m_Buffer
			};

			//	Valid until the work submitted after this call has completed.
			Allocation Allocate(VkDeviceSize i_Size, VkDeviceSize i_Alignment = 256);

			size_t BlockCount() const;

		private:
			struct Block
			{
				VkBuffer		m_Buffer = VK_NULL_HANDLE;
				VmaAllocation	m_Allocation = nullptr;
				uint8_t*		m_pCpu = nullptr;
				VkDeviceAddress	m_Gpu = 0;
				VkDeviceSize	m_Size = 0;
				VkDeviceSize	m_Used = 0;
				uint64_t		m_LastUse = 0;		// timeline value that frees it
			};

			size_t new_block(VkDeviceSize i_Size);

			vkdDevice&			m_Device;
			VkDeviceSize		m_BlockSize;
			std::vector<Block>	m_Blocks;
			size_t				m_Current = 0;
			mutable std::mutex	m_Lock;
	};
}
