/*****************************************************************************
**  g2dResourceCounterOptiX.cpp
**
**      See g2dResourceCounterOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DRESOURCECOUNTEROPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dResourceCounterOptiX.hpp"
#endif

namespace gox
{
	float g2dResourceCounterOptiX::GetTotalVideoMemory()
	{
		return 0.0f;
	}

	float g2dResourceCounterOptiX::GetTotalSceneTextureMemory()
	{
		return 0.0f;
	}

	int g2dResourceCounterOptiX::GetNumSceneTextures()
	{
		return 0;
	}

	float g2dResourceCounterOptiX::GetTotalFramebufferMemory()
	{
		return 0.0f;
	}

	int g2dResourceCounterOptiX::GetNumFramebufferTextures()
	{
		return 0;
	}

	float g2dResourceCounterOptiX::GetTotalShadowMapMemory()
	{
		return 0.0f;
	}

	int g2dResourceCounterOptiX::GetNumShadowMapTextures()
	{
		return 0;
	}

	float g2dResourceCounterOptiX::GetTotalVertexBufferMemory()
	{
		return 0.0f;
	}

	int g2dResourceCounterOptiX::GetNumVertexBuffers()
	{
		return 0;
	}

	float g2dResourceCounterOptiX::GetTotalIndexBufferMemory()
	{
		return 0.0f;
	}

	int g2dResourceCounterOptiX::GetNumIndexBuffers()
	{
		return 0;
	}
}
