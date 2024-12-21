/*****************************************************************************
**  shdwPassAlphaFill.hpp
**
**      shdwPassAlphaFill is a post process to overwrite the alpha channel
**		to both transparent/opaque/mask black objects
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassAlphaFill.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "GraphicsDX11/G3d/g3dSceneGlobal.hpp"


namespace
{
	matMaterial l_AlphaFillMat;

	g3dBlendStateMgr::BlendState* stp_SimpleVisible = NULL;
	g3dBlendStateMgr::BlendState* stp_SimpleInvisible = NULL;
	g3dBlendStateMgr::BlendState* stp_SimpleVisibleRGBA = NULL;
	g3dBlendStateMgr::BlendState* stp_SimpleInvisibleRGBA = NULL;
	g3dBlendStateMgr::BlendState* stp_CompositeRev = NULL;
	g3dBlendStateMgr::BlendState* stp_CompositeRevG = NULL;
	g3dBlendStateMgr::BlendState* stp_CompositeRevB = NULL;
	g3dBlendStateMgr::BlendState* stp_CopyTarget = NULL;
	g3dBlendStateMgr::BlendState* stp_CopyTargetRGBA = NULL;
	g3dBlendStateMgr::BlendState* stp_CopyTargetMask = NULL;
	g3dBlendStateMgr::BlendState* stp_CopyAlpha = NULL;
	g3dBlendStateMgr::BlendState* stp_Restore = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Disable_NS = NULL;
}; // namespace

void shdwPassAlphaFill::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	stp_SimpleVisible = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA);
	stp_SimpleInvisible = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA);
	stp_SimpleVisibleRGBA = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_SimpleInvisibleRGBA = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);
	// destAlpha = (1-srcAlpha)*destAlpha
	stp_CompositeRev = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_DEST_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA );
	// destGreen = (1-srcGreen)*destGreen 
	stp_CompositeRevG = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_GREEN );
	// destBlue = (1-srcBlue)*destBlue 
	stp_CompositeRevB = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_BLUE );
	// destAlpha = srcAlpha + destAlpha
	stp_CopyTarget = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA );
	stp_CopyTargetRGBA = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	// destAlpha = srcAlpha*destAlpha
	stp_CopyTargetMask = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA );
	// destAlpha = srcAlpha
	stp_CopyAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALPHA );
	stp_Restore = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);;

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState(FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
}

void shdwPassAlphaFill::CleanupStates()
{
	delete dsp_Disable_NS;
	delete dsp_Test_Write_LessE_NS;
	delete dsp_Test_LessE_NS;

	delete stp_Restore;
	delete stp_SimpleInvisible;
	delete stp_SimpleVisible;
	delete stp_SimpleInvisibleRGBA;
	delete stp_SimpleVisibleRGBA;
	delete stp_CompositeRev;
	delete stp_CompositeRevG;
	delete stp_CompositeRevB;
	delete stp_CopyTarget;
	delete stp_CopyTargetRGBA;
	delete stp_CopyTargetMask;
	delete stp_CopyAlpha;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAlphaFill::shdwPassAlphaFill()
: m_sceneInfo(NULL), m_isBinaryAlpha(false)
{	
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAlphaFill::shdwPassAlphaFill(g2dRenderTarget* i_destination, bool i_isBinaryAlpha)
	: m_sceneInfo(NULL), m_isBinaryAlpha(i_isBinaryAlpha)
{
	SetRenderTarget(i_destination);

	// lazy init so that this material can be reused across instantiations.
	if (!l_AlphaFillMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthMap.fx"), matShaderMgr::GetSpecialEffect("DepthMap.fx"));
		l_AlphaFillMat.SetShaderParams(p);
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAlphaFill::~shdwPassAlphaFill()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassAlphaFill::SetTransparentPassInfo(g3dLayer* i_pLayer,
		const camCamera* i_pCamera,
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_ScratchTarget0,
		matRenderTargetTexture* i_ScratchTarget1,
		matRenderTargetTexture* i_ScratchTarget2,
		matRenderTargetTexture* i_AAScratchTarget, 
		bool i_bAA)
{
	m_pLayer = i_pLayer;
	m_pCamera = i_pCamera;
	m_depthTarget1 = i_depthTarget1;		// store depth in alpha fill pass
	m_ScratchTarget0 = i_ScratchTarget0;	// store opaque objects alpha value, including opaque mask objects
	m_ScratchTarget1 = i_ScratchTarget1;	// store transparent objects alpha value
	m_ScratchTarget2 = i_ScratchTarget2;	// store transparent mask objects alpha value
	m_bAA = i_bAA;
	m_AAScratchTarget = i_AAScratchTarget;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassAlphaFill::Render(float i_time)
{
	m_stats.Reset();

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassAlphaFill::Render" );

	int i,n;

	// Render depth/alpha onto scratch target, including mask objects
	if (m_bAA) 
	{
		// in the case of AA we will do all the work in the AAScratchTarget.
		// the other scratch targets are non-antialiased.
		// we will use the g,b,and a channels of the scratch target.
		// a = opaque alphas, and non-depth peel transp alphas
		// g = transp nodes for depth peel
		// b = transp mask nodes for depth peel

		// alpha, depth peel, depth peel, opaque alpha
		m_AAScratchTarget->Clear(maFloatRGBA(0,1,1,0), true);
		m_AAScratchTarget->MakeCurrent();
	}
	else
	{
		m_depthTarget1->ClearDepthStencil();
		m_depthTarget1->MakeCurrent();
		m_ScratchTarget0->Clear(maFloatRGBA(0,0,0,0));
		m_ScratchTarget0->MakeCurrent();
	}

	g3dDX11Util::AllowAdditiveChanges(false);
	// turn off all color writes! this is alpha write only!
	// See DepthMap.fx

	// For opaque objects, write the z value
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	// Simply enable alpha to coverage for in all circumstance. 
	g3dBlendStateMgr::SetBlendState(m_bAA ? stp_SimpleVisibleRGBA : stp_SimpleVisible);

	// write all the opaque visible object's alpha to 1
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}

	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}
	
	// Render invisible opaque object that mask alpha channel black
	// Render alpha to 0
	g3dBlendStateMgr::SetBlendState(m_bAA ? stp_SimpleInvisibleRGBA : stp_SimpleInvisible);
	
	const nodeCacheList& invisibleMaskBlackNodes = m_sceneInfo->GetInvisibleMaskBlackNodes();
	n = invisibleMaskBlackNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = invisibleMaskBlackNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());		
	}

	if( g3dPrefs::CurrentPrefs().m_TransparencyMode == 1 && !m_isBinaryAlpha)
	{
		//force depth peeled transparency to have no back face culling
		bool bCullSaved = g3dDX11Util::GetCullEnable();
		g3dDX11Util::SetCullEnable( true );

		if (m_bAA)
		{
			// lock in current alphas to scratch target 0
			// because we will comp into that at the end.
			m_AAScratchTarget->Resolve(m_ScratchTarget0);
			m_AAScratchTarget->MakeCurrent();

			// scratch target 1 is G channel.
			// Setup alpha texture

			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

			// Render visible transparent nodes alpha to its G value
			g3dBlendStateMgr::SetBlendState(stp_CompositeRevG);
			g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
			const TranspNodeVector& transNodes = transparencySort->GetTransparentNodes();
			n = transNodes.size();
			for (i = 0; i < n; i++)
			{
				m_stats.m_nTriangles += RenderTransparentNode(transNodes[i].m_pSceneNode, m_sceneInfo->GetRenderStateCache(), transNodes[i].m_RenderStateCache, m_isBinaryAlpha);
			}

			// scratch target 2 is B channel

			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

			// Render visible transparent nodes alpha to its B value
			g3dBlendStateMgr::SetBlendState(stp_CompositeRevB);
			const TranspNodeVector& transMaskNodes = transparencySort->GetTransparentMaskNodes();
			n = transMaskNodes.size();
			for (i = 0; i < n; i++)
			{
				m_stats.m_nTriangles += RenderTransparentNode(transMaskNodes[i].m_pSceneNode, m_sceneInfo->GetRenderStateCache(), transMaskNodes[i].m_RenderStateCache, m_isBinaryAlpha);
			}
			//matTextureMgr::SaveTextureToFile(m_ScratchTarget2, fsLocator(itString("C:\\Projects\\mask.png")));

			g3dBlendStateMgr::SetBlendState(stp_Restore);

			m_AAScratchTarget->Resolve(m_ScratchTarget1);
//			matTextureMgr::SaveTextureToFile( m_ScratchTarget1, fsLocator(itString("E:\\temp1.dds")) );
			m_AAScratchTarget->Resolve(m_ScratchTarget2);

			// composite G into A (destA = srcG+destA)
			// clear render targets in case we need to bind one as texture!
			g2dDX11Global::SetRenderTargets(NULL,NULL);
//			matTextureMgr::SaveTextureToFile( m_ScratchTarget0, fsLocator(itString("E:\\temp0PreComp.dds")) );
			CompositeTexture(m_ScratchTarget1, m_ScratchTarget0);

			// composite B into A (destA = srcB*destA)
			// clear render targets in case we need to bind one as texture!
			g2dDX11Global::SetRenderTargets(NULL,NULL);
//			matTextureMgr::SaveTextureToFile( m_ScratchTarget0, fsLocator(itString("E:\\temp0Comp.dds")) );
			CompositeTextureMask(m_ScratchTarget2, m_ScratchTarget0);
		}
		else 
		{
			// Setup alpha texture
			m_ScratchTarget1->Clear(maFloatRGBA(0,0,0,1));
			m_ScratchTarget1->MakeCurrent();
			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

			// Render visible transparent nodes alpha to its alpha value
			g3dBlendStateMgr::SetBlendState(stp_CompositeRev);
			g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
			const TranspNodeVector& transNodes = transparencySort->GetTransparentNodes();
			n = transNodes.size();
			for (i = 0; i < n; i++)
			{
				m_stats.m_nTriangles += RenderTransparentNode(transNodes[i].m_pSceneNode, m_sceneInfo->GetRenderStateCache(), transNodes[i].m_RenderStateCache, m_isBinaryAlpha);
			}

			m_ScratchTarget2->Clear(maFloatRGBA(0,0,0,1));
			m_ScratchTarget2->MakeCurrent();
			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

			// Render visible transparent nodes alpha to its alpha value
			g3dBlendStateMgr::SetBlendState(stp_CompositeRev);
			const TranspNodeVector& transMaskNodes = transparencySort->GetTransparentMaskNodes();
			n = transMaskNodes.size();
			for (i = 0; i < n; i++)
			{
				m_stats.m_nTriangles += RenderTransparentNode(transMaskNodes[i].m_pSceneNode, m_sceneInfo->GetRenderStateCache(), transMaskNodes[i].m_RenderStateCache, m_isBinaryAlpha);
			}
			//matTextureMgr::SaveTextureToFile(m_ScratchTarget2, fsLocator(itString("C:\\Projects\\mask.png")));

			CompositeTexture(m_ScratchTarget1, m_ScratchTarget0);
			CompositeTextureMask(m_ScratchTarget2, m_ScratchTarget0);
		}
		g3dDX11Util::SetCullEnable( bCullSaved );
	}
	else
	{
		// Set render state for transparent visible objects
		g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

		// Render visible transparent nodes alpha to its alpha value
		g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
		transparencySort->SortTransparentNodes(true);
		const TranspNodeVector& transNodes = transparencySort->GetTransparentNodes(true);
		n = transNodes.size();
		for (i = 0; i < n; i++)
		{
			if (transNodes[i].m_bIsMask)
				g3dBlendStateMgr::SetBlendState(stp_SimpleInvisible);
			else
				g3dBlendStateMgr::SetBlendState(stp_SimpleVisible);

			m_stats.m_nTriangles += RenderTransparentNode(transNodes[i].m_pSceneNode, m_sceneInfo->GetRenderStateCache(), transNodes[i].m_RenderStateCache, m_isBinaryAlpha);
		}

		if (m_bAA)
			m_AAScratchTarget->Resolve(m_ScratchTarget0);
	}
	// last step: put alphas into render target, clamped
	CopyTextureAlpha(m_ScratchTarget0, m_pRenderTarget);

	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(stp_Restore);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dDX11Util::AllowAdditiveChanges(true);

	D3DPERF_EndEvent();

	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//  Draw opaque node
//--------------------------------------------------------------------
int shdwPassAlphaFill::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateTraverser)
{
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateTraverser->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassAlphaFill::DrawNode" );
	g3dFogDX11::EnableFog( false );

	// resolve material/effect
	const matMaterial* pMaterial = &l_AlphaFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	
	ID3DX11Effect* pEffectDX = dynamic_cast<effShaderBaseDX11*>(pEffect)->GetD3DXEffect();

	// e_Environment is a different type of ambient pass that is ambient-only.
	pEffect->SetTechnique(m_bAA ? "ZFillRGBA" : "ZFill");

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );
	D3DPERF_EndEvent();	

	return nTriangles;
}


//--------------------------------------------------------------------
//  Draw transparent node
//--------------------------------------------------------------------
int shdwPassAlphaFill::RenderTransparentNode(const g3dSceneNode* i_pNode, g3dRenderStateTraverser* i_pRenderStateTraverser, 
											 const g3dRenderStateCache i_RenderStateCache, bool i_isBinaryAlpha)
{
	int nTriangles = 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassAlphaFill::DrawTransparentNode" );

	g3dDrawStyleUtilDX11::SetDrawStyle(i_pNode->GetDrawStyle());
	i_pRenderStateTraverser->SetRenderState( i_RenderStateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );
	
	g3dFogDX11::EnableFog( false );

	// resolve material/effect
	const matMaterial* pAlphaMaterial = &l_AlphaFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pAlphaMaterial);
	
	ID3DX11Effect* pEffectDX = dynamic_cast<effShaderBaseDX11*>(pEffect)->GetD3DXEffect();

	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);

	//setup transparency parameters
//#define NEWTRANS
#ifdef NEWTRANS 
	float val = 1.0f;
	matTexture* pTex = NULL;
	if( pMaterial && pMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pEffectDX->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pEffectDX->GetVariableByName("g_transparency")->AsScalar()->SetFloat(val);
#else
	pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool(false);
	pEffectDX->GetVariableByName("g_transparency")->AsScalar()->SetFloat(1.0f);
	// Get the transparent map from the material
	if ( pMaterial )
	{
		// first check shader params
		if (pMaterial->GetShaderParams() != NULL)
		{
			if (pMaterial->GetShaderParams()->m_pTransparencyMap)
			{
				if (pMaterial->GetShaderParams()->m_pTransparencyMap->GetTexture())
				{
					pEffectDX->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
						g3dDX11TextureUtil::GetD3DTexture(pMaterial->GetShaderParams()->m_pTransparencyMap->GetTexture()));
					pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool(true);
				}
			}
			if (pMaterial->GetShaderParams()->m_pTransparency)
			{
				pEffectDX->GetVariableByName("g_transparency")->AsScalar()->SetFloat(pMaterial->GetShaderParams()->m_pTransparency->GetProperty().GetValue());
			}
		}
		// then check effectdata
		else if (pMaterial->GetEffectData() != NULL)
		{
			pEffectDX->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
						g3dDX11TextureUtil::GetD3DTexture(pMaterial->GetEffectData()->GetTransparencyTexture()));
			if (pMaterial->GetEffectData()->GetTransparencyTexture())
				pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool(true);
			pEffectDX->GetVariableByName("g_transparency")->AsScalar()->SetFloat(pMaterial->GetEffectData()->GetTransparencyValue());
		}
	}
#endif
	if (i_isBinaryAlpha)
	{
		pEffect->SetTechnique(m_bAA ? "BinaryAlphaFillRGBA" : "BinaryAlphaFill");
	}
	else
	{
		pEffect->SetTechnique(m_bAA ? "AlphaFillRGBA" : "AlphaFill");
	}

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_pNode, pAlphaMaterial, pEffect );

	D3DPERF_EndEvent();	

	return nTriangles;
}

//copies one texture alpha component into target texture alpha component, with ref on
void shdwPassAlphaFill::CompositeTexture(matTexture* src, g2dRenderTarget* tgt)
{
	UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassAlphaFill::CompositeTexture" );

	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = NULL;
	if (m_bAA)
		pEffectTechnique = pEffect->GetTechniqueByName("SimpleCopyInvGAlpha");
	else
		pEffectTechnique = pEffect->GetTechniqueByName("SimpleCopyInvAlpha");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions( w, h);

	// Copy color
	g3dBlendStateMgr::SetBlendState( m_bAA ? stp_CopyTargetRGBA : stp_CopyTarget );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	D3DX11_TECHNIQUE_DESC techDesc;
	pEffectTechnique->GetDesc( &techDesc );
	for (uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pEffectTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);

		// alpha blending/z writing control by callar
		ID3D11ShaderResourceView* aRes[1] = { g3dDX11TextureUtil::GetD3DTexture(src)};
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, aRes);

		g3dDX11Util::DrawFullScreenQuad( w, h );
	}

	// Restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	ID3D11ShaderResourceView* nullTex[1] = { NULL };
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
//  Composit alpha to rendertarget
//--------------------------------------------------------------------
void shdwPassAlphaFill::CompositeTextureMask(matTexture* src, g2dRenderTarget* tgt)
{
	g3dBlendStateMgr::SetBlendState( stp_CopyTargetMask );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );
	
	g3dDX11Util::CopyTexToTargetTechnique( src, tgt, m_bAA ? std::string("BlueChannel") : std::string("SimpleCopy") ); //(copy needed to overcome MSAA sampling limitation)
}

//--------------------------------------------------------------------
//  Copy alpha to rendertarget
//--------------------------------------------------------------------
void shdwPassAlphaFill::CopyTextureAlpha(matTexture* src, g2dRenderTarget* tgt)
{
	g3dBlendStateMgr::SetBlendState( stp_CopyAlpha );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );
	
	// clamp alphas in 0..1
	g3dDX11Util::CopyTexToTargetTechnique( src, tgt, std::string("SimpleCopyAlphaSat") ); //(copy needed to overcome MSAA sampling limitation)
}
