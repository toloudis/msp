/****************************************************************************\
**	shdwUVRendererVk.hpp
**
**	The shdwUVRendererVk is a rendering algorithm that uses a floating
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_UVRENDERERVK_HPP
#error shdwUVRendererVk.hpp multiply included
#endif
#define GVK_SHDW_UVRENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

class g3dSceneNode;

namespace gvk
{

class shdwUVRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwUVRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwUVRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	void ReleaseResources();

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;

	g2dRenderTarget* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	void DrawNode(const g3dSceneNode* i_pNode);
};

}
