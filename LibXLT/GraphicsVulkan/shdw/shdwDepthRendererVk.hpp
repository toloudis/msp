/****************************************************************************\
**	shdwDepthRendererVk.hpp
**
**	The shdwDepthRendererVk is a rendering algorithm that uses a floating
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	GraphicsVulkan port of GraphicsDX11's shdwDepthRendererDX11.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_DEPTHRENDERERVK_HPP
#error shdwDepthRendererVk.hpp multiply included
#endif
#define GVK_SHDW_DEPTHRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

class effMaskAlphaData;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;

class shdwDepthRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwDepthRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwDepthRendererVk();

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

	g2dRenderTarget* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	shdwPassTraversal m_SceneDatabase;

	//void CopyToBackBuf(matRenderTargetTexture* pTex, const camCamera& i_Camera);
	void GetTightestNearFar(const camCamera& i_Camera, float& o_CamNear, float& o_CamFar);

	matMaterial* m_pAlphaMaskMat;
	effMaskAlphaData* m_pMaskAlphaData;
	matMaterial* m_pHairMat;

	void DrawNode(const g3dSceneNode* i_pNode);
	void DrawHairNode(const sNodePlusState& i_Node);

};

}
