/****************************************************************************\
**	shdwOpacityMapRendererDX11.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_OPACITYMAPRENDERERDX11_HPP
#error shdwOpacityMapRendererDX11.hpp multiply included
#endif
#define SHDW_OPACITYMAPRENDERERDX11_HPP

#ifndef SHDW_SCENERENDERERDX11_HPP
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp" 
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class effTexturedData;
class g2dRenderTarget;
class g3dProjectedLight;

class shdwOpacityMapRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwOpacityMapRendererDX11( g3dProjectedLight* pProjLight );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwOpacityMapRendererDX11();

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

