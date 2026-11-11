/*****************************************************************************
**  cuBuffer.cpp
**
**      See cuBuffer.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif

#include <algorithm>
#include <utility>

namespace gox
{
	cuBuffer::cuBuffer()
		: m_Ptr(0)
		, m_Size(0)
		, m_Capacity(0)
	{
	}

	cuBuffer::~cuBuffer()
	{
		Free();
	}

	cuBuffer::cuBuffer(cuBuffer&& io_Other) noexcept
		: m_Ptr(io_Other.m_Ptr)
		, m_Size(io_Other.m_Size)
		, m_Capacity(io_Other.m_Capacity)
	{
		io_Other.m_Ptr = 0;
		io_Other.m_Size = 0;
		io_Other.m_Capacity = 0;
	}

	cuBuffer& cuBuffer::operator=(cuBuffer&& io_Other) noexcept
	{
		if (this != &io_Other)
		{
			Free();
			std::swap(m_Ptr, io_Other.m_Ptr);
			std::swap(m_Size, io_Other.m_Size);
			std::swap(m_Capacity, io_Other.m_Capacity);
		}
		return *this;
	}

	bool cuBuffer::Reserve(size_t i_Bytes)
	{
		if (i_Bytes <= m_Capacity)
		{
			m_Size = i_Bytes;
			return true;
		}
		Free();
		CUdeviceptr ptr = 0;
		if (CU_CHECK(cu().cuMemAlloc(&ptr, i_Bytes)))
		{
			return false;
		}
		m_Ptr = ptr;
		m_Size = i_Bytes;
		m_Capacity = i_Bytes;
		return true;
	}

	void cuBuffer::Free()
	{
		//	A destroyed context took its memory with it
		if (m_Ptr && optxContext::Exists())
		{
			CU_CHECK(cu().cuMemFree(m_Ptr));
		}
		m_Ptr = 0;
		m_Size = 0;
		m_Capacity = 0;
	}

	bool cuBuffer::Upload(const void* i_pData, size_t i_Bytes)
	{
		if (!Reserve(i_Bytes))
		{
			return false;
		}
		if (i_Bytes == 0)
		{
			return true;
		}
		return !CU_CHECK(cu().cuMemcpyHtoDAsync(m_Ptr, i_pData, i_Bytes, optxContext::GetStream()));
	}

	bool cuBuffer::Download(void* o_pData, size_t i_Bytes) const
	{
		const size_t bytes = std::min(i_Bytes, m_Size);
		if (bytes == 0)
		{
			return true;
		}
		CUstream stream = optxContext::GetStream();
		if (CU_CHECK(cu().cuMemcpyDtoHAsync(o_pData, m_Ptr, bytes, stream)))
		{
			return false;
		}
		return !CU_CHECK(cu().cuStreamSynchronize(stream));
	}

	bool cuBuffer::Clear()
	{
		if (m_Size == 0)
		{
			return true;
		}
		return !CU_CHECK(cu().cuMemsetD8Async(m_Ptr, 0, m_Size, optxContext::GetStream()));
	}
}
