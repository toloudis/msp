/****************************************************************************\
**	shdwReflectionsOnlyRendererDX11.hpp
**
**	The shdwReflectionsOnlyRendererDX11 is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_REFLECTIONSONLYRENDERERDX11_HPP
#error shdwReflectionsOnlyRendererDX11.hpp multiply included
#endif
#define SHDW_REFLECTIONSONLYRENDERERDX11_HPP

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
class g2dWindowDX11;

class shdwReflectionsOnlyRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwReflectionsOnlyRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwReflectionsOnlyRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Sometimes we need to access the pre-buffer pixels that don't get 
	//	drawn to a window.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer();

	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;

	g2dWindowDX11* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);
	
	shdwPassTraversal m_SceneDatabase;

	void CopyToBackBuf(matRenderTargetTexture* pTex, bool bDoBlend = false, bool isRGB=true);
	int RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache);
};

