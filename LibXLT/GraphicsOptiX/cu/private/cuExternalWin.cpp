/*****************************************************************************
**  cuExternalWin.cpp
**
**      Windows handles for CUDA imports (see cuExternal.hpp): opaque Win32
**	handles, as GraphicsVulkan exports them.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CUEXTERNAL_HPP
#include "GraphicsOptiX/cu/cuExternal.hpp"
#endif

#include <cstddef>

namespace gox
{
	void cuSetPlatformMemoryHandle(CUDA_EXTERNAL_MEMORY_HANDLE_DESC& io_Desc, void* i_Handle)
	{
		io_Desc.type = CU_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32;
		io_Desc.handle.win32.handle = i_Handle;
		io_Desc.handle.win32.name = NULL;
	}

	void cuSetPlatformTimelineHandle(CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC& io_Desc, void* i_Handle)
	{
		io_Desc.type = CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_WIN32;
		io_Desc.handle.win32.handle = i_Handle;
		io_Desc.handle.win32.name = NULL;
	}
}
