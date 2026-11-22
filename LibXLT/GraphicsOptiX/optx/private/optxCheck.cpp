/*****************************************************************************
**  optxCheck.cpp
**
**      See optxCheck.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gox
{
	const char* cuResultName(CUresult i_Result)
	{
		const char* name = NULL;
		if (cu().cuGetErrorName && cu().cuGetErrorName(i_Result, &name) == CUDA_SUCCESS && name)
		{
			return name;
		}
		return "unknown CUresult";
	}

	const char* optxResultName(OptixResult i_Result)
	{
		const char* name = optixGetErrorName(i_Result);
		return name ? name : "unknown OptixResult";
	}

	bool cuFailed(CUresult i_Result, const char* i_Expression, const char* i_File, int i_Line)
	{
		if (i_Result == CUDA_SUCCESS)
		{
			return false;
		}
		DBG_ASSERT(false, "CUDA call failed: " << i_Expression << " -> " << cuResultName(i_Result)
						  << " (" << i_File << ":" << i_Line << ")");
		return true;
	}

	bool optxFailed(OptixResult i_Result, const char* i_Expression, const char* i_File, int i_Line)
	{
		if (i_Result == OPTIX_SUCCESS)
		{
			return false;
		}
		DBG_ASSERT(false, "OptiX call failed: " << i_Expression << " -> " << optxResultName(i_Result)
						  << " (" << i_File << ":" << i_Line << ")");
		return true;
	}
}
