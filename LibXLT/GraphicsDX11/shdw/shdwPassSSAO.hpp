/****************************************************************************\
**	shdwPassSSAO.hpp
**
**  This pass generates Screen Space Ambient Occlusion with support 
**  for peeled depth for overlapped object and an enlarged depth buffer
**  for occlusions near the fram edge.  An optional blur pass uses a separated
**  Bilateral filter that is depth aware (to not blur edges/seams).
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PASSSSAO_HPP
#error shdwPassSSAO.hpp multiply included
#endif
#define SHDW_PASSSSAO_HPP

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

class shdwPassSSAO : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassSSAO();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassSSAO(matRenderTargetTexture* i_NearDepthBuffer, matRenderTargetTexture* i_DepthBuffer, matRenderTargetTexture* i_DepthBuffer2, 
		g2dRenderTarget* i_FullSceneColors,	matRenderTargetTexture* i_pAOSurface, matRenderTargetTexture* i_pTmpSurface, const ssaoParams& i_SSAOParams);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassSSAO();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetBuffers(matRenderTargetTexture* i_NearDepthBuffer, matRenderTargetTexture* i_DepthBuffer, matRenderTargetTexture* i_DepthBuffer2, 
		g2dRenderTarget* i_FullSceneColors,	matRenderTargetTexture* i_pAOSurface, matRenderTargetTexture* i_pTmpSurface, const ssaoParams& i_SSAOParams);

	//--------------------------------------------------------------------
	// free any locally allocated shared (re-entrant?) resources
	//--------------------------------------------------------------------
	static void CleanUp();

	static void InitStates();
	static void CleanupStates();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render(float i_time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_Traversal) {m_SceneInfo = i_Traversal;}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetCamera(const camCamera* i_pCamera) {m_pCamera = i_pCamera;}

protected:
	shdwPassTraversal* m_SceneInfo;
	const camCamera* m_pCamera;

	ssaoParams m_Params;

	matRenderTargetTexture* m_pNearDepthBuffer;
	matRenderTargetTexture* m_pDepthBuffer;
	matRenderTargetTexture* m_pPrevDepthBuffer;	//used for depth peeling

	matRenderTargetTexture* m_AOTargetTex;
	matRenderTargetTexture* m_TempTargetTex;
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AccumulateAOPass( g2dRenderTarget* i_pAOTarget, int i_nLayers );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DrawStencilPass(g2dRenderTarget* i_pTarget);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ClearStencil(g2dRenderTarget* i_pTarget);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void BlurAO(g2dRenderTarget* i_pRenderTarget);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void BlendAO(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pAOSrc );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderNodeStencil(const sNodePlusState& i_Node);
};

