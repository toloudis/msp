/*****************************************************************************
**  GraphicsOptiXLayerNoSdk.cpp
**
**      GraphicsOptiXLayer for a build made without the OptiX SDK
**	(optix.props: GOX_NO_OPTIX_SDK) or the CUDA toolkit (cuda.props:
**	GOX_NO_CUDA_TOOLKIT). GraphicsOptiX.vcxproj then compiles this file
**	instead of every other source, so the rest of the library can use
**	OptiX and CUDA freely. Init fails, saying what was missing; nothing
**	else is ever called.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_GRAPHICSOPTIXLAYER_HPP
#include "GraphicsOptiX/GraphicsOptiXLayer.hpp"
#endif

#include <cstddef>
#include <string>

namespace
{
	std::string l_InitError;
}

bool GraphicsOptiXLayer::Init(void* /*i_hWnd*/, const g3dGfxOptions& /*i_Options*/)
{
#if defined(GOX_NO_OPTIX_SDK) && defined(GOX_NO_CUDA_TOOLKIT)
	l_InitError = "this build was made without the NVIDIA OptiX SDK and the CUDA toolkit";
#elif defined(GOX_NO_OPTIX_SDK)
	l_InitError = "this build was made without the NVIDIA OptiX SDK";
#else
	l_InitError = "this build was made without the CUDA toolkit";
#endif
	return false;
}

void GraphicsOptiXLayer::CleanUp() throw()
{
}

void GraphicsOptiXLayer::InitGraphics()
{
}

void GraphicsOptiXLayer::CleanUpGraphics()
{
}

g2dSystem* GraphicsOptiXLayer::GetSystem2D()
{
	return NULL;
}

g3dSystem* GraphicsOptiXLayer::GetSystem3D()
{
	return NULL;
}

const char* GraphicsOptiXLayer::GetName()
{
	return "OptiX";
}

const char* GraphicsOptiXLayer::GetCodeName()
{
	return "optix";
}

const std::string& GraphicsOptiXLayer::GetInitError()
{
	return l_InitError;
}
