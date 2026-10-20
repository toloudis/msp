/*****************************************************************************
**  shdwPassAlphaFill.hpp
**
**      shdwPassAlphaFill is a post process to overwrite the alpha channel
**		to both transparent/opaque/mask black objects
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSALPHAFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassAlphaFill.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{


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
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	stp_SimpleVisible = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT);
	stp_SimpleInvisible = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT);
	stp_SimpleVisibleRGBA = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_SimpleInvisibleRGBA = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	// destAlpha = (1-srcAlpha)*destAlpha
	stp_CompositeRev = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_DST_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT );
	// destGreen = (1-srcGreen)*destGreen 
	stp_CompositeRevG = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_G_BIT );
	// destBlue = (1-srcBlue)*destBlue 
	stp_CompositeRevB = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_B_BIT );
	// destAlpha = srcAlpha + destAlpha
	stp_CopyTarget = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT );
	stp_CopyTargetRGBA = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	// destAlpha = srcAlpha*destAlpha
	stp_CopyTargetMask = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT );
	// destAlpha = srcAlpha
	stp_CopyAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_A_BIT );
	stp_Restore = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);;

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState(false, false, VK_COMPARE_OP_NEVER, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
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
	G3D_PROFILE_SCOPE("PassAlphaFill");
	m_stats.Reset();


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

	g3dVkUtil::AllowAdditiveChanges(false);
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
		bool bCullSaved = g3dVkUtil::GetCullEnable();
		g3dVkUtil::SetCullEnable( true );

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
			g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
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
			g2dVkGlobal::SetRenderTargets(NULL,NULL);
//			matTextureMgr::SaveTextureToFile( m_ScratchTarget0, fsLocator(itString("E:\\temp0PreComp.dds")) );
			CompositeTexture(m_ScratchTarget1, m_ScratchTarget0);

			// composite B into A (destA = srcB*destA)
			// clear render targets in case we need to bind one as texture!
			g2dVkGlobal::SetRenderTargets(NULL,NULL);
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
			g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
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
		g3dVkUtil::SetCullEnable( bCullSaved );
	}
	else
	{
		// Set render state for transparent visible objects
		g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

		// Render visible transparent nodes alpha to its alpha value
		g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
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
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(stp_Restore);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dVkUtil::AllowAdditiveChanges(true);


	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//  Draw opaque node
//--------------------------------------------------------------------
int shdwPassAlphaFill::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateTraverser)
{
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateTraverser->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	g3dFogVk::EnableFog( false );

	// resolve material/effect
	const matMaterial* pMaterial = &l_AlphaFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	
	fxEffect* pEffectDX = dynamic_cast<effShaderBaseVk*>(pEffect)->GetFxEffect();

	// e_Environment is a different type of ambient pass that is ambient-only.
	pEffect->SetTechnique(m_bAA ? "ZFillRGBA" : "ZFill");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}


//--------------------------------------------------------------------
//  Draw transparent node
//--------------------------------------------------------------------
int shdwPassAlphaFill::RenderTransparentNode(const g3dSceneNode* i_pNode, g3dRenderStateTraverser* i_pRenderStateTraverser, 
											 const g3dRenderStateCache i_RenderStateCache, bool i_isBinaryAlpha)
{
	int nTriangles = 0;


	g3dDrawStyleUtilVk::SetDrawStyle(i_pNode->GetDrawStyle());
	i_pRenderStateTraverser->SetRenderState( i_RenderStateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );
	
	g3dFogVk::EnableFog( false );

	// resolve material/effect
	const matMaterial* pAlphaMaterial = &l_AlphaFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pAlphaMaterial);
	
	fxEffect* pEffectDX = dynamic_cast<effShaderBaseVk*>(pEffect)->GetFxEffect();

	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);

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

	pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? true : false);
	pEffectDX->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dVkTextureUtil::GetImage(pTex) : NULL);
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
						g3dVkTextureUtil::GetImage(pMaterial->GetShaderParams()->m_pTransparencyMap->GetTexture()));
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
						g3dVkTextureUtil::GetImage(pMaterial->GetEffectData()->GetTransparencyTexture()));
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
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_pNode, pAlphaMaterial, pEffect );


	return nTriangles;
}

//copies one texture alpha component into target texture alpha component, with ref on
void shdwPassAlphaFill::CompositeTexture(matTexture* src, g2dRenderTarget* tgt)
{
	uint32_t uiPass;

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
	int technique = -1;
	if (m_bAA)
		technique = pEffect->FindTechnique("SimpleCopyInvGAlpha");
	else
		technique = pEffect->FindTechnique("SimpleCopyInvAlpha");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions( w, h);

	// Copy color
	g3dBlendStateMgr::SetBlendState( m_bAA ? stp_CopyTargetRGBA : stp_CopyTarget );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for (uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);

		// alpha blending/z writing control by callar
		vkdImage* aRes[1] = { g3dVkTextureUtil::GetImage(src)};
		vkdGlobal::Context().SetShaderResources(0, 1, aRes);

		g3dVkUtil::DrawFullScreenQuad( w, h );
	}

	// Restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	vkdImage* nullTex[1] = { NULL };
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

}

//--------------------------------------------------------------------
//  Composit alpha to rendertarget
//--------------------------------------------------------------------
void shdwPassAlphaFill::CompositeTextureMask(matTexture* src, g2dRenderTarget* tgt)
{
	g3dBlendStateMgr::SetBlendState( stp_CopyTargetMask );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );
	
	g3dVkUtil::CopyTexToTargetTechnique( src, tgt, m_bAA ? std::string("BlueChannel") : std::string("SimpleCopy") ); //(copy needed to overcome MSAA sampling limitation)
}

//--------------------------------------------------------------------
//  Copy alpha to rendertarget
//--------------------------------------------------------------------
void shdwPassAlphaFill::CopyTextureAlpha(matTexture* src, g2dRenderTarget* tgt)
{
	g3dBlendStateMgr::SetBlendState( stp_CopyAlpha );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );
	
	// clamp alphas in 0..1
	g3dVkUtil::CopyTexToTargetTechnique( src, tgt, std::string("SimpleCopyAlphaSat") ); //(copy needed to overcome MSAA sampling limitation)
}

}
