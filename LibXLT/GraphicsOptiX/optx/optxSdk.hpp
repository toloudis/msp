/*****************************************************************************
**  optxSdk.hpp
**
**      Includes the CUDA driver API header (cuda.h, from the CUDA toolkit;
**	cuda.props) and the OptiX host API (types and function declarations).
**	Every GraphicsOptiX file that names OptiX or CUDA includes this header
**	rather than the SDKs'.
**
**	Only cuda.h's types and constants are used directly: nothing links the
**	CUDA driver library, so calling a cu* function declared there fails
**	to link. CUDA calls go through cuLoader's table (cu/cuLoader.hpp), and
**	OptiX calls through OptiX's own function table (optxLoader).
**
**	A build without either SDK compiles none of the files that include
**	this (GraphicsOptiXLayerNoSdk.cpp).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSDK_HPP
#error optxSdk.hpp multiply included
#endif
#define GOX_OPTXSDK_HPP

#include <cuda.h>
#include <optix_host.h>
