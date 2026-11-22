/*****************************************************************************
**  cuExternalMemoryCache.hpp
**
**      cuExternalMemoryCache gives CUDA pointers into Vulkan buffers from
**	the export pool (vkdBuffer::IsExportable), so CUDA reads and writes
**	them in place. Each memory block is imported once
**	(cuImportExternalMemory, then cuExternalMemoryGetMappedBuffer for the
**	whole block) and kept; a buffer's pointer is its block's plus its
**	offset. The cache listens to vkdDevice's memory-free hooks and forgets
**	(destroys the import of) a block when Vulkan frees it, which happens
**	only once no buffer is left in it.
**
**	Map is for any thread that has made the OptiX context current; the
**	free hook runs on whichever thread frees the block.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CUEXTERNALMEMORYCACHE_HPP
#error cuExternalMemoryCache.hpp multiply included
#endif
#define GOX_CUEXTERNALMEMORYCACHE_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <volk.h>

#include <cstddef>
#include <mutex>
#include <unordered_map>

namespace gvk
{
	class vkdBuffer;
	class vkdDevice;
}

namespace gox
{
	class cuExternalMemoryCache
	{
		public:
			explicit cuExternalMemoryCache(gvk::vkdDevice& i_Device);
			~cuExternalMemoryCache();
			cuExternalMemoryCache(const cuExternalMemoryCache&) = delete;
			cuExternalMemoryCache& operator=(const cuExternalMemoryCache&) = delete;

			//--------------------------------------------------------------------
			//	Map - the CUDA address of an exportable buffer's first byte, 0
			//	(logged) if the buffer isn't exportable or the import fails
			//--------------------------------------------------------------------
			CUdeviceptr Map(const gvk::vkdBuffer& i_Buffer);

			//--------------------------------------------------------------------
			//	For tests: blocks imported now, and imports made so far
			//--------------------------------------------------------------------
			size_t GetBlockCount() const;
			int GetImportCount() const;

		private:
			struct Block
			{
				CUexternalMemory	m_Memory;
				CUdeviceptr			m_Ptr;
			};

			void forget(VkDeviceMemory i_Memory);

			gvk::vkdDevice&								m_Device;
			int											m_HookId;
			mutable std::mutex							m_Lock;
			std::unordered_map<VkDeviceMemory, Block>	m_Blocks;
			int											m_nImports;
	};
}
