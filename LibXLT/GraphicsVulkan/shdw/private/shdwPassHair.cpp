/****************************************************************************\
**	shdwPassHair.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSHAIR_HPP
#include "GraphicsVulkan/shdw/shdwPassHair.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

shdwPassHair::shdwPassHair()
	: m_sceneInfo(NULL)
{
}

shdwPassHair::shdwPassHair(const camCamera*, matRenderTargetTexture* (&)[4], g2dRenderTarget* i_destination, g2dRenderTarget*,
						   matRenderTargetTexture*, matRenderTargetTexture*, matRenderTargetTexture*, matRenderTargetTexture*,
						   matRenderTargetTexture*, matRenderTargetTexture*)
	: m_sceneInfo(NULL)
{
	m_pRenderTarget = i_destination;
}

shdwPassHair::~shdwPassHair()
{
}

int shdwPassHair::Render(float)
{
	G3D_PROFILE_SCOPE("PassHair");
	GVK_NOT_YET("shdwPassHair");
	return 0;
}

int shdwPassHair::RenderDepthPeeled(float)
{
	GVK_NOT_YET("shdwPassHair");
	return 0;
}

//	(no states until Plan 7 ports the pass)
void shdwPassHair::InitStates()
{
}

void shdwPassHair::CleanupStates()
{
}

}
