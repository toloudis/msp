/*****************************************************************************
**  optxSdk.hpp
**
**      Includes the OptiX host API (types and function declarations). Every
**	GraphicsOptiX file that names OptiX includes this header rather than
**	the SDK's.
**
**	OptiX's host API names two CUDA driver types (CUcontext, CUstream).
**	Until GraphicsOptiX uses CUDA itself they are declared here exactly as
**	cuda.h declares them, so the CUDA toolkit isn't needed to build. When
**	the CUDA toolkit is added, include <cuda.h> here instead.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSDK_HPP
#error optxSdk.hpp multiply included
#endif
#define GOX_OPTXSDK_HPP

#define OPTIX_DONT_INCLUDE_CUDA
typedef struct CUctx_st* CUcontext;
typedef struct CUstream_st* CUstream;

#include <optix_host.h>
