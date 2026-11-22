/*****************************************************************************
**  cuBuffer.hpp
**
**      cuBuffer is one allocation of device memory and the number of bytes
**	in use in it. Copies go on the OptiX context's stream
**	(optxContext::GetStream), in order with the work launched there.
**
**	Reserve only grows the allocation (its contents are lost when it
**	does). Uploads from pageable host memory: CUDA copies the data before
**	the call returns, so the caller may free or change it at once.
**
**	Needs optxContext to exist, and current on the calling thread
**	(optxContext::MakeCurrent). A buffer freed after the context was
**	destroyed is just forgotten: destroying the context released it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CUBUFFER_HPP
#error cuBuffer.hpp multiply included
#endif
#define GOX_CUBUFFER_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <cstddef>

namespace gox
{
	class cuBuffer
	{
		public:
			cuBuffer();
			~cuBuffer();
			cuBuffer(cuBuffer&& io_Other) noexcept;
			cuBuffer& operator=(cuBuffer&& io_Other) noexcept;
			cuBuffer(const cuBuffer&) = delete;
			cuBuffer& operator=(const cuBuffer&) = delete;

			//--------------------------------------------------------------------
			//	Reserve - makes the allocation at least i_Bytes; sets the size
			//	in use to i_Bytes. Returns false if memory ran out (the buffer
			//	is then empty).
			//--------------------------------------------------------------------
			bool Reserve(size_t i_Bytes);

			//--------------------------------------------------------------------
			//	Free - releases the allocation
			//--------------------------------------------------------------------
			void Free();

			//--------------------------------------------------------------------
			//	Upload - Reserve(i_Bytes), then copies i_pData to the start
			//--------------------------------------------------------------------
			bool Upload(const void* i_pData, size_t i_Bytes);

			//--------------------------------------------------------------------
			//	Download - copies the first i_Bytes (at most GetSize()) to
			//	o_pData, after the work queued on the stream so far
			//--------------------------------------------------------------------
			bool Download(void* o_pData, size_t i_Bytes) const;

			//--------------------------------------------------------------------
			//	Clear - sets every byte in use to 0, on the stream
			//--------------------------------------------------------------------
			bool Clear();

			CUdeviceptr Get() const { return m_Ptr; }
			size_t GetSize() const { return m_Size; }
			size_t GetCapacity() const { return m_Capacity; }

		private:
			CUdeviceptr	m_Ptr;
			size_t		m_Size;
			size_t		m_Capacity;
	};
}
