/****************************************************************************\
**	g3dTextureRendererDX11.hpp
**
**	The g3dTextureRendererDX11 is a renderer that uses the simplest rendering
**	setting to render ramp texture
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_TEXTURERENDERERDX11_HPP
#error g3dTextureRendererDX11.hpp multiply included
#endif
#define G3D_TEXTURERENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

class g3dTextureRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dTextureRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dTextureRendererDX11();

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

