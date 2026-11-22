/*****************************************************************************
**  vkdExportPool.hpp
**
**      vkdExportPool is device-local memory another API can import
**	(GraphicsOptiX: CUDA builds acceleration structures straight from
**	Vulkan's mesh buffers): a VMA custom pool whose blocks are allocated
**	with VkExportMemoryAllocateInfo (the platform handle type,
**	vkdPlatformMemoryHandleType), in large blocks (VMA's default size for
**	the heap, 256 MB, after a few smaller first ones), so the importer
**	imports each block once rather than each buffer. A buffer bigger than
**	c_DedicatedSize gets an exportable block of its own. vkdDevice makes one when it
**	is created with external memory; vkdBuffer::Create allocates from it
**	when asked for an exportable buffer.
**
**	A buffer bound to such memory must say it may be (VkExternalMemory-
**	BufferCreateInfo): BufferCreateNext gives the struct to chain.
**
**	Images (vkdImageDesc::m_bExportable: textures CUDA samples) have a pool
**	of their own, ImagePool, for the memory type images take, from which
**	each gets a dedicated block (an importer maps an image at the start of
**	its memory); ImageCreateNext is the VkExternalMemoryImageCreateInfo to
**	chain. ImagePool is NULL if no device-local memory type suits
**	exportable images.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDEXPORTPOOL_HPP
#error vkdExportPool.hpp multiply included
#endif
#define GVK_VKDEXPORTPOOL_HPP

#include <volk.h>

#include <memory>
#include <string>

typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VmaPool_T* VmaPool;

namespace gvk
{
	class vkdExportPool
	{
		public:
			//	A buffer bigger than this gets memory of its own, exportable too
			static const VkDeviceSize c_DedicatedSize = 64ull * 1024 * 1024;

			//	NULL (o_Error says why) if the device has no device-local memory
			//	type for exportable buffers
			static std::unique_ptr<vkdExportPool> Create(VmaAllocator i_Allocator, std::string& o_Error);
			~vkdExportPool();

			VmaPool		Pool() const { return m_Pool; }

			//	Chain into VkBufferCreateInfo::pNext for a buffer in this pool
			const VkExternalMemoryBufferCreateInfo* BufferCreateNext() const { return &m_BufferInfo; }

			//	The pool exportable images allocate from (dedicated), or NULL
			VmaPool		ImagePool() const { return m_ImagePool; }
			//	Chain into VkImageCreateInfo::pNext for an image in ImagePool
			const VkExternalMemoryImageCreateInfo* ImageCreateNext() const { return &m_ImageInfo; }

		private:
			vkdExportPool() = default;
			vkdExportPool(const vkdExportPool&) = delete;
			vkdExportPool& operator = (const vkdExportPool&) = delete;

			VmaAllocator						m_Allocator = nullptr;
			VmaPool								m_Pool = nullptr;
			VmaPool								m_ImagePool = nullptr;
			VkExportMemoryAllocateInfo			m_ExportInfo = {VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO};	// the pool keeps a pointer
			VkExternalMemoryBufferCreateInfo	m_BufferInfo = {VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO};
			VkExternalMemoryImageCreateInfo		m_ImageInfo = {VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO};
	};
}
