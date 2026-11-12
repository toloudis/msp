/*****************************************************************************
**  cuLoader.hpp
**
**      cuLoader loads the CUDA driver API from the display driver
**	(nvcuda.dll) and fills a table of the functions GraphicsOptiX uses,
**	through which every CUDA call goes: cu().cuMemAlloc(...). Nothing
**	links CUDA, so MachStudio starts on machines without an NVIDIA driver.
**
**	The functions are fetched with cuGetProcAddress for the CUDA version
**	of the toolkit headers (CUDA_VERSION), so each entry has the signature
**	cuda.h declares for that name: cuda.h maps many names to versioned ones
**	(cuMemAlloc to cuMemAlloc_v2, ...), and the table follows the same
**	mapping, member names included.
**
**	To use another function, add it to GOX_CU_FUNCTIONS.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CULOADER_HPP
#error cuLoader.hpp multiply included
#endif
#define GOX_CULOADER_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <string>

//	X(name) for each CUDA driver function in the table
#define GOX_CU_FUNCTIONS(X)				\
	X(cuInit)							\
	X(cuDriverGetVersion)				\
	X(cuGetErrorName)					\
	X(cuGetErrorString)					\
	X(cuDeviceGetCount)					\
	X(cuDeviceGet)						\
	X(cuDeviceGetName)					\
	X(cuDeviceGetAttribute)			\
	X(cuDeviceGetUuid)				\
	X(cuDeviceTotalMem)					\
	X(cuDevicePrimaryCtxRetain)			\
	X(cuDevicePrimaryCtxRelease)		\
	X(cuCtxSetCurrent)					\
	X(cuCtxGetCurrent)					\
	X(cuCtxSynchronize)					\
	X(cuStreamCreate)					\
	X(cuStreamDestroy)					\
	X(cuStreamSynchronize)				\
	X(cuMemAlloc)						\
	X(cuMemFree)						\
	X(cuMemcpyHtoD)						\
	X(cuMemcpyDtoH)						\
	X(cuMemcpyHtoDAsync)				\
	X(cuMemcpyDtoHAsync)				\
	X(cuMemcpyDtoDAsync)				\
	X(cuMemsetD8Async)					\
	X(cuImportExternalMemory)			\
	X(cuExternalMemoryGetMappedBuffer)	\
	X(cuDestroyExternalMemory)			\
	X(cuImportExternalSemaphore)		\
	X(cuDestroyExternalSemaphore)		\
	X(cuWaitExternalSemaphoresAsync)	\
	X(cuSignalExternalSemaphoresAsync)	\
	X(cuEventCreate)					\
	X(cuEventDestroy)				\
	X(cuEventRecord)					\
	X(cuEventSynchronize)			\
	X(cuEventElapsedTime)

namespace gox
{
	struct cuFunctions
	{
#define GOX_CU_DECLARE(name) decltype(&::name) name = nullptr;
		GOX_CU_FUNCTIONS(GOX_CU_DECLARE)
#undef GOX_CU_DECLARE
	};

	class cuLoader
	{
		public:
			//--------------------------------------------------------------------
			//	Load - loads the CUDA driver API and calls cuInit; returns false
			//	(o_Error says why) if there is no NVIDIA driver, or one older
			//	than the toolkit's headers. Loading again once loaded does
			//	nothing.
			//--------------------------------------------------------------------
			static bool Load(std::string& o_Error);

			//--------------------------------------------------------------------
			//	Unload - releases the library. No CUDA object may be alive.
			//--------------------------------------------------------------------
			static void Unload();

			//--------------------------------------------------------------------
			//	IsLoaded
			//--------------------------------------------------------------------
			static bool IsLoaded();

			//--------------------------------------------------------------------
			//	Get - the function table (all null until Load succeeds)
			//--------------------------------------------------------------------
			static const cuFunctions& Get();
	};

	inline const cuFunctions& cu()
	{
		return cuLoader::Get();
	}
}
