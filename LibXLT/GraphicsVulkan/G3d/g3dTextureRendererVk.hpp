/****************************************************************************\
**	g3dTextureRendererVk.hpp
**
**	The g3dTextureRendererVk is a renderer that uses the simplest rendering
**	setting to render ramp texture
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_G3D_TEXTURERENDERERVK_HPP
#error g3dTextureRendererVk.hpp multiply included
#endif
#define GVK_G3D_TEXTURERENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

namespace gvk
{

class g3dTextureRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dTextureRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dTextureRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	void ReleaseResources();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.

	int m_Width, m_Height;

	g2dRenderTarget* m_pWindow;
};

}
