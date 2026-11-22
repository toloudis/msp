/****************************************************************************\
**	shdwPassSSGI.hpp
**
**  This pass generates Screen Space Global Illumination with support 
**  for color object 
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PASSSSGI_HPP
#error shdwPassSSGI.hpp multiply included
#endif
#define SHDW_PASSSSGI_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class effMaskAlphaData;

class shdwPassSSGI : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassSSGI(matRenderTargetTexture* i_NearDepthBuffer, 
		matRenderTargetTexture* i_DepthBuffer, 
		matRenderTargetTexture* i_DepthBuffer2, 
		g2dRenderTarget* i_FullSceneColors,	
		matRenderTargetTexture* i_pGISurface, 
		matRenderTargetTexture* i_pTmpSurface, 
		const ssgiParams& i_SSGIParams,
		matRenderTargetTexture* i_FullSceneColorsTex);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassSSGI();

	//--------------------------------------------------------------------
	// free any locally allocated shared (re-entrant?) resources
	//--------------------------------------------------------------------
	static void CleanUp();

	static void InitStates();
	static void CleanupStates();

	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_Traversal) {m_SceneInfo = i_Traversal;}
	void SetCamera(const camCamera* i_pCamera) {m_pCamera = i_pCamera;}

protected:
	shdwPassTraversal* m_SceneInfo;
	const camCamera* m_pCamera;

	ssgiParams m_Params;

	matRenderTargetTexture* m_pColorBuffer;
	matRenderTargetTexture* m_pNearDepthBuffer;
	matRenderTargetTexture* m_pDepthBuffer;
	matRenderTargetTexture* m_pPrevDepthBuffer;	//used for depth peeling

	matRenderTargetTexture* m_GITargetTex;
	matRenderTargetTexture* m_TempTargetTex;
	
	void AccumulateGIPass( g2dRenderTarget* i_pGITarget, int i_nLayers );
	void DrawStencilPass(g2dRenderTarget* i_pTarget);
	void ClearStencil(g2dRenderTarget* i_pTarget);
	void BlurGI(g2dRenderTarget* i_pRenderTarget);
	void BlendGI(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pGISrc );
	int RenderNodeStencil(const sNodePlusState& i_Node);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void CleanupMultisample();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void InitMultisample();

	//void CopyToBackBuf(matRenderTargetTexture* pSrcTex, bool bDoBlend, bool isRGB, g2dRenderTarget* pDstTarget);
};
