/****************************************************************************\
**	shdwNormalRendererVk.hpp
**
**	The shdwNormalRendererVk is a rendering algorithm that uses a floating
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	GraphicsVulkan port of GraphicsDX11's shdwNormalRendererDX11.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_NORMALRENDERERVK_HPP
#error shdwNormalRendererVk.hpp multiply included
#endif
#define GVK_SHDW_NORMALRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

class effTexturedData;
class g2dRenderTarget;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class g2dWindowVk;

class shdwNormalRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwNormalRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwNormalRendererVk();

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

	g2dWindowVk* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	shdwPassTraversal m_SceneDatabase;

	matMaterial* m_pNormalMat;
	effTexturedData* m_pNormalData;
	matMaterial* m_pHairMat;
};

}
