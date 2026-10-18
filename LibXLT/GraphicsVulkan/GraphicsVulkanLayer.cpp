/*****************************************************************************
**  GraphicsVulkanLayer.cpp
**
**      See GraphicsVulkanLayer.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif

namespace
{
	std::string l_InitError;
}

bool GraphicsVulkanLayer::Init(void* /*i_hWnd*/, const g3dGfxOptions& /*i_Options*/)
{
	l_InitError = "GraphicsVulkan is not implemented yet";
	return false;
}

void GraphicsVulkanLayer::CleanUp() throw()
{
}

void GraphicsVulkanLayer::InitGraphics()
{
}

void GraphicsVulkanLayer::CleanUpGraphics()
{
}

g2dSystem* GraphicsVulkanLayer::GetSystem2D()
{
	return NULL;
}

g3dSystem* GraphicsVulkanLayer::GetSystem3D()
{
	return NULL;
}

const std::string& GraphicsVulkanLayer::GetInitError()
{
	return l_InitError;
}
