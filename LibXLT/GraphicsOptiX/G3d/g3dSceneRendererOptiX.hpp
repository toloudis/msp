/*****************************************************************************
**  g3dSceneRendererOptiX.hpp
**
**      g3dSceneRendererOptiX renders a scene into a render target with
**	OptiX; g3dPickRendererOptiX finds what is under a pixel.
**
**	Stub: render nothing (0 triangles) and pick nothing. Every renderer
**	type g3dSceneRendererCreate offers is one of these for now.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DSCENERENDEREROPTIX_HPP
#error g3dSceneRendererOptiX.hpp multiply included
#endif
#define GOX_G3DSCENERENDEREROPTIX_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif

#include <vector>

namespace gox
{
	class g3dSceneRendererOptiX : public g3dSceneRenderer
	{
		public:
			virtual int Render(g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
							   const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime);
			virtual void ReleaseResources();
	};

	class g3dPickRendererOptiX : public g3dPickRenderer
	{
		public:
			virtual int Render(g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
							   const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime);
			virtual void ReleaseResources();
			virtual void SetPickViewport(g2dRenderTarget* i_pTarget);
			virtual void RestoreViewport();
			virtual void GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo);
	};
}
