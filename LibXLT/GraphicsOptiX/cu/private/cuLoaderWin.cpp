/*****************************************************************************
**  cuLoaderWin.cpp
**
**      See cuLoader.hpp. Windows: the driver API is nvcuda.dll, in the
**	system folder wherever an NVIDIA display driver is installed.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif

#include <windows.h>

#include <string>

namespace gox
{
	namespace
	{
		HMODULE		l_hLibrary = NULL;
		cuFunctions	l_Functions;

		typedef CUresult (CUDAAPI* GetProcAddressFn)(const char*, void**, int, cuuint64_t, CUdriverProcAddressQueryResult*);
	}

	bool cuLoader::Load(std::string& o_Error)
	{
		o_Error.clear();
		if (l_hLibrary)
		{
			return true;
		}

		//	Only from the system folder: never a copy next to the exe
		HMODULE library = ::LoadLibraryExW(L"nvcuda.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (!library)
		{
			o_Error = "no CUDA in the display driver (nvcuda.dll not found; an NVIDIA GPU and driver are needed)";
			return false;
		}

		//	cuda.h maps cuGetProcAddress to cuGetProcAddress_v2, the name nvcuda.dll exports
		GetProcAddressFn get_proc = reinterpret_cast<GetProcAddressFn>(::GetProcAddress(library, "cuGetProcAddress_v2"));
		if (!get_proc)
		{
			::FreeLibrary(library);
			o_Error = "the display driver's CUDA is too old (no cuGetProcAddress_v2; update the NVIDIA driver)";
			return false;
		}

		//	The unversioned name (#name isn't macro-expanded) at the headers'
		//	CUDA version gives the version cuda.h declares
		cuFunctions functions;
		std::string missing;
#define GOX_CU_FETCH(name)																	\
		{																					\
			void* pfn = NULL;																\
			CUdriverProcAddressQueryResult status = CU_GET_PROC_ADDRESS_SYMBOL_NOT_FOUND;	\
			if (get_proc(#name, &pfn, CUDA_VERSION, CU_GET_PROC_ADDRESS_DEFAULT, &status) == CUDA_SUCCESS	\
				&& status == CU_GET_PROC_ADDRESS_SUCCESS && pfn)							\
			{																				\
				functions.name = reinterpret_cast<decltype(functions.name)>(pfn);			\
			}																				\
			else																			\
			{																				\
				missing += missing.empty() ? #name : ", " #name;							\
			}																				\
		}
		GOX_CU_FUNCTIONS(GOX_CU_FETCH)
#undef GOX_CU_FETCH

		if (!missing.empty())
		{
			::FreeLibrary(library);
			o_Error = "the display driver's CUDA is older than CUDA " + std::to_string(CUDA_VERSION / 1000) + "." +
					  std::to_string(CUDA_VERSION % 1000 / 10) + " (update the NVIDIA driver; missing: " + missing + ")";
			return false;
		}

		const CUresult result = functions.cuInit(0);
		if (result != CUDA_SUCCESS)
		{
			const char* name = NULL;
			functions.cuGetErrorName(result, &name);
			::FreeLibrary(library);
			o_Error = std::string("CUDA could not be initialized (") + (name ? name : std::to_string(int(result)).c_str()) + ")";
			return false;
		}

		l_hLibrary = library;
		l_Functions = functions;
		return true;
	}

	void cuLoader::Unload()
	{
		if (l_hLibrary)
		{
			::FreeLibrary(l_hLibrary);
			l_hLibrary = NULL;
			l_Functions = cuFunctions();
		}
	}

	bool cuLoader::IsLoaded()
	{
		return l_hLibrary != NULL;
	}

	const cuFunctions& cuLoader::Get()
	{
		return l_Functions;
	}
}
