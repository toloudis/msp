/*****************************************************************************
**  cuExternal.hpp
**
**      The platform part of importing Vulkan memory and semaphores into
**	CUDA (PAC style): the handle type and handle in CUDA's import
**	descriptors, for handles from GraphicsVulkan's vkdExportMemoryHandle
**	and vkdExportSemaphoreHandle. Implemented per platform
**	(cuExternalWin.cpp on Windows: opaque Win32 handles, which CUDA
**	doesn't take ownership of, so the caller closes them after importing).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CUEXTERNAL_HPP
#error cuExternal.hpp multiply included
#endif
#define GOX_CUEXTERNAL_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

namespace gox
{
	//	Sets io_Desc's type and handle for an exported memory block
	void cuSetPlatformMemoryHandle(CUDA_EXTERNAL_MEMORY_HANDLE_DESC& io_Desc, void* i_Handle);

	//	Sets io_Desc's type and handle for an exported timeline semaphore
	void cuSetPlatformTimelineHandle(CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC& io_Desc, void* i_Handle);
}
