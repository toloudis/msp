/****************************************************************************\
**	shdwOpacityMapRendererVk.hpp
**
**	Renders a projected light's opacity shadow maps (hair shadows): the
**	scene seen from the light, each fragment's opacity added into the
**	depth layer it falls in (shdwPassOpacity).
**
**	GraphicsVulkan port of GraphicsDX11's shdwOpacityMapRendererDX11.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_OPACITYMAPRENDERERVK_HPP
#error shdwOpacityMapRendererVk.hpp multiply included
#endif
#define GVK_SHDW_OPACITYMAPRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effTexturedData;
class g2dRenderTarget;
class g3dProjectedLight;

namespace gvk
{

class matRenderTargetTexture;

class shdwOpacityMapRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwOpacityMapRendererVk( g3dProjectedLight* pProjLight );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwOpacityMapRendererVk();

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

	g3dProjectedLight* m_pProjLight;

	shdwPassTraversal m_SceneDatabase;
};

}
