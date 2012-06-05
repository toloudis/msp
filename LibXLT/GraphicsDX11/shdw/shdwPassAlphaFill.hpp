/*****************************************************************************
**  shdwPassAlphaFill.hpp
**
**      shdwPassAlphaFill is a post process to overwrite the alpha channel
**		to both transparent/opaque/mask black objects
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSALPHAFILL_HPP
#error shdwPassAlphaFill.hpp multiply included
#endif
#define SHDW_PASSALPHAFILL_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

#ifndef G3D_BLENDSTATEMGR_HPP
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#endif

#ifndef G3D_DEPTHSTENCILSTATEMGR_HPP
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#endif


class shdwPassAlphaFill : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassAlphaFill();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassAlphaFill(g2dRenderTarget* i_destination, bool i_isBinaryAlpha = false);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassAlphaFill();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetTransparentPassInfo(g3dLayer* i_pLayer,
		const camCamera* i_pCamera,
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_ScratchTarget0,
		matRenderTargetTexture* i_ScratchTarget1,
		matRenderTargetTexture* i_ScratchTarget2,
		matRenderTargetTexture* i_AAScratchTarget, 
		bool i_bAA);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

protected:

	//--------------------------------------------------------------------
	//  Draw all transparent mask black nodes
	//--------------------------------------------------------------------
	int RenderTransparentMaskBlack();

	//--------------------------------------------------------------------
	//  Draw one opaque node
	//--------------------------------------------------------------------
	int RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateTraverser);

	//--------------------------------------------------------------------
	//  Draw one transparent node
	//--------------------------------------------------------------------
	int RenderTransparentNode(const g3dSceneNode* i_pNode, g3dRenderStateTraverser* i_pRenderStateTraverser, 
									const g3dRenderStateCache i_RenderStateCache, 
									bool i_isBinaryAlpha = false);

	//--------------------------------------------------------------------
	//  Copy alpha component after depth peels
	//--------------------------------------------------------------------
	void CompositeTexture(matTexture* src, g2dRenderTarget* tgt);

	//--------------------------------------------------------------------
	//  Composit alpha to rendertarget
	//--------------------------------------------------------------------
	void CompositeTextureMask(matTexture* src, g2dRenderTarget* tgt);

	//--------------------------------------------------------------------
	//  Copy alpha to rendertarget
	//--------------------------------------------------------------------
	void CopyTextureAlpha(matTexture* src, g2dRenderTarget* tgt);


	shdwPassTraversal* m_sceneInfo;
	bool m_isBinaryAlpha;

	g3dLayer* m_pLayer;
	const camCamera* m_pCamera;
	matRenderTargetTexture* m_depthTarget1;
	matRenderTargetTexture* m_ScratchTarget0;
	matRenderTargetTexture* m_ScratchTarget1;
	matRenderTargetTexture* m_ScratchTarget2;
	matRenderTargetTexture* m_AAScratchTarget;
	bool m_bAA;
};