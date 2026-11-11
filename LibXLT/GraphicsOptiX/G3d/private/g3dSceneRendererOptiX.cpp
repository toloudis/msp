/*****************************************************************************
**  g3dSceneRendererOptiX.cpp
**
**      See g3dSceneRendererOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DSCENERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneRendererOptiX.hpp"
#endif

namespace gox
{
	int g3dSceneRendererOptiX::Render(g2dRenderTarget* /*i_pWindow*/, const camCamera& /*i_Camera*/,
									  const g3dScene& /*i_Scene*/,
									  const std::vector<g3dLayer*>& /*i_ViewerLayers*/,
									  float /*i_fSimTime*/)
	{
		return 0;
	}

	void g3dSceneRendererOptiX::ReleaseResources()
	{
	}

	int g3dPickRendererOptiX::Render(g2dRenderTarget* /*i_pWindow*/, const camCamera& /*i_Camera*/,
									 const g3dScene& /*i_Scene*/,
									 const std::vector<g3dLayer*>& /*i_ViewerLayers*/,
									 float /*i_fSimTime*/)
	{
		return 0;
	}

	void g3dPickRendererOptiX::ReleaseResources()
	{
	}

	void g3dPickRendererOptiX::SetPickViewport(g2dRenderTarget* /*i_pTarget*/)
	{
	}

	void g3dPickRendererOptiX::RestoreViewport()
	{
	}

	//	Nothing picked: o_PickInfo is left as the caller made it
	void g3dPickRendererOptiX::GetPickInfo(g2dRenderTarget* /*i_pWindow*/, g3dPickInfo& /*o_PickInfo*/)
	{
	}
}
