/****************************************************************************\
**	shdwNormalRendererDX11.hpp
**
**	The shdwNormalRendererDX11 is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_NORMALRENDERERDX11_HPP
#error shdwNormalRendererDX11.hpp multiply included
#endif
#define SHDW_NORMALRENDERERDX11_HPP

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
class g2dWindowDX11;

class shdwNormalRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwNormalRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwNormalRendererDX11();

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
	void CopyToBackBuf(matRenderTargetTexture* pTex);

	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;

	g2dWindowDX11* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);
	
	shdwPassTraversal m_SceneDatabase;

	matMaterial* m_pNormalMat;
	effTexturedData* m_pNormalData;
	matMaterial* m_pHairMat;
};

