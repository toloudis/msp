/*****************************************************************************
**  optxSdk.hpp
**
**      Includes the OptiX host API (types and function declarations), and
**	through it the CUDA driver API header (cuda.h, from the CUDA toolkit;
**	cuda.props). Every GraphicsOptiX file that names OptiX or CUDA includes
**	this header rather than the SDKs'.
**
**	Only cuda.h's types and constants are used directly: nothing links the
**	CUDA driver library, so calling a cu* function declared there fails
**	to link. CUDA calls go through the functions loaded from nvcuda.dll at
**	run time.
**
**	In a build without the CUDA toolkit (GOX_NO_CUDA_TOOLKIT) the two CUDA
**	types OptiX's host API names (CUcontext, CUstream) are declared here
**	exactly as cuda.h declares them, so the library still compiles;
**	GraphicsOptiXLayer::Init then fails.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSDK_HPP
#error optxSdk.hpp multiply included
#endif
#define GOX_OPTXSDK_HPP

#ifdef GOX_NO_CUDA_TOOLKIT
#define OPTIX_DONT_INCLUDE_CUDA
typedef struct CUctx_st* CUcontext;
typedef struct CUstream_st* CUstream;
#endif

#include <optix_host.h>
