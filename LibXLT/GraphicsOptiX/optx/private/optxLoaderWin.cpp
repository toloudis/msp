/*****************************************************************************
**  optxLoaderWin.cpp
**
**      See optxLoader.hpp. Windows: optix_stubs.h finds nvoptix.dll through
**	the display driver's registry entries (it includes windows.h, which is
**	why this file is a *Win* file). This translation unit also defines the
**	OptiX function table, which must be defined in exactly one.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXLOADER_HPP
#include "GraphicsOptiX/optx/optxLoader.hpp"
#endif

#include <string>

#ifdef GOX_NO_OPTIX_SDK

//	Built without the OptiX SDK (optix.props): OptiX never loads
namespace gox
{
	bool optxLoader::Load(std::string& o_Error)
	{
		o_Error = "this build was made without the NVIDIA OptiX SDK";
		return false;
	}

	void optxLoader::Unload()
	{
	}

	bool optxLoader::IsLoaded()
	{
		return false;
	}
}

#else

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <optix_stubs.h>
#include <optix_function_table_definition.h>

namespace gox
{
	namespace
	{
		void* l_hOptixLibrary = NULL;
	}

	bool optxLoader::Load(std::string& o_Error)
	{
		o_Error.clear();
		if (l_hOptixLibrary)
		{
			return true;
		}
		const OptixResult result = optixInitWithHandle(&l_hOptixLibrary);
		if (result != OPTIX_SUCCESS)
		{
			l_hOptixLibrary = NULL;
			if (result == OPTIX_ERROR_UNSUPPORTED_ABI_VERSION)
			{
				o_Error = "the display driver's OptiX is older than this build's OptiX SDK (update the NVIDIA driver)";
			}
			else if (result == OPTIX_ERROR_LIBRARY_NOT_FOUND)
			{
				o_Error = "no OptiX in the display driver (an NVIDIA GPU and driver are needed)";
			}
			else
			{
				o_Error = std::string("OptiX could not be loaded (") + std::to_string(int(result)) + ")";
			}
			return false;
		}
		return true;
	}

	void optxLoader::Unload()
	{
		if (l_hOptixLibrary)
		{
			optixUninitWithHandle(l_hOptixLibrary);
			l_hOptixLibrary = NULL;
		}
	}

	bool optxLoader::IsLoaded()
	{
		return l_hOptixLibrary != NULL;
	}
}

#endif	// GOX_NO_OPTIX_SDK
