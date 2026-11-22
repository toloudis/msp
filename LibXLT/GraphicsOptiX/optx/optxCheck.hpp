/*****************************************************************************
**  optxCheck.hpp
**
**      Result checking for GraphicsOptiX, as GraphicsVulkan's VK_CHECK.
**
**	CU_CHECK(expr) evaluates a CUDA driver call (through cu()); OPTIX_CHECK
**	evaluates an OptiX call. If it fails, either DBG_ASSERTs with the
**	expression and the result's name, and evaluates to true; false if the
**	call succeeded. So: if (CU_CHECK(...)) { recover }.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXCHECK_HPP
#error optxCheck.hpp multiply included
#endif
#define GOX_OPTXCHECK_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

namespace gox
{
	//	cuResultName - "CUDA_ERROR_OUT_OF_MEMORY" etc.
	const char* cuResultName(CUresult i_Result);

	//	optxResultName - "OPTIX_ERROR_INVALID_VALUE" etc.
	const char* optxResultName(OptixResult i_Result);

	//	Both return true if i_Result is a failure (and assert)
	bool cuFailed(CUresult i_Result, const char* i_Expression, const char* i_File, int i_Line);
	bool optxFailed(OptixResult i_Result, const char* i_Expression, const char* i_File, int i_Line);
}

#define CU_CHECK(expr) ::gox::cuFailed((expr), #expr, __FILE__, __LINE__)
#define OPTIX_CHECK(expr) ::gox::optxFailed((expr), #expr, __FILE__, __LINE__)
