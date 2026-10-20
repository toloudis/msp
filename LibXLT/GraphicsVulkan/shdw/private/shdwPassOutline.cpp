/****************************************************************************\
**	shdwPassOutline.cpp
**
**		see .hpp
**
**  John Schwab
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSOUTLINE_HPP
#include "GraphicsVulkan/shdw/shdwPassOutline.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
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
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassOutline::shdwPassOutline(
	matRenderTargetTexture* i_outlineTarget,
	matRenderTargetTexture* i_AATarget,
	g2dRenderTarget* i_destination,
	shdwPassTraversal* i_SceneInfo)
:	m_sceneInfo(i_SceneInfo),
	m_outlineTarget(i_outlineTarget),
	m_AATarget(i_AATarget)
{
	SetRenderTarget(i_destination);

	m_pDepthMat = new matMaterial("DepthRender.fx");
	m_pNormalMat = new matMaterial("NormalMap.fx");
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassOutline::~shdwPassOutline()
{
	delete m_pNormalMat;
	delete m_pDepthMat;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassOutline::SetBuffers( matRenderTargetTexture* i_outlineTarget,
								 matRenderTargetTexture* i_AATarget,
								 g2dRenderTarget* i_destination )
{
	m_outlineTarget = i_outlineTarget;
	m_AATarget = i_AATarget;
	SetRenderTarget(i_destination);
}

//----------------------------------------------------------------------------------------
// clear outline buffer
// for each object:
//	draw into alpha with addition
// blend with target using outline effect
//----------------------------------------------------------------------------------------
int shdwPassOutline::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassOutline");
	m_stats.Reset();

	if( !g3dSingleLightRendering::GetDoSingleLightRendering()) return 0;
	if( g3dSingleLightRendering::GetDoDOFPrepPass()) return 0;

	nodeCacheList outlineNodes = m_sceneInfo->GetOutlineNodes();
	if( outlineNodes.empty()) return 0;

	// no lights?
	if( g3dLightMgrVk::Implementation()->GetLights().size() <= 0 ) return 0;

	// Disable all lights and then turn them on one at a time
	g3dLightMgrVk::Implementation()->DisableAllLights();

	g3dSingleLightRendering::SetActiveLight( NULL );
	g3dVkUtil::AllowAdditiveChanges( false);

	m_outlineTarget->MakeCurrent();
	m_outlineTarget->Clear( maFloatRGBA());
	m_AATarget->Clear(maFloatRGBA());

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

	// Render the outlined objects.
	// Each object can have its own params that describe
	// the way its outline is composited.
	int num_nodes = outlineNodes.size();

	const g3dSceneNode* pNode = NULL;
	const g3dFragment* pFragment = NULL;
	for (int n=0; n<num_nodes; n++)
	{
		pNode = outlineNodes[n].m_pNode;
		pFragment = pNode->GetFragment();
		if( pFragment->IsShadowHull() )	continue;
		const matMaterial* pmat = pFragment->GetMaterial();
		if( !pmat ) continue;
		if( !pmat->GetHasOutline() ) continue;

		// Depth disparity Outline (silhouette)
		if( pmat->GetOutlineData().m_bUseDepths )
		{
			g3dBlendStateMgr::SetBlendState(st_NoBlend);
			m_outlineTarget->Clear( maFloatRGBA(0,0,0,0));

			g3dVkUtil::SetOverrideMaterial( m_pDepthMat );	//override object material to generate depth data.

			DrawNode( pNode, false, NULL, NULL);

			g3dVkUtil::SetOverrideMaterial(NULL);

			// now render using the outline shader to smear out the outline.
			// draw to m_tempTarget. this target contains the pre-outline rendered scene.
			// we will blend m_outlineTarget into this.
			g3dBlendStateMgr::SetBlendState(st_MulBlend);

//			g3dVkUtil::CopyTexToTarget( m_outlineTarget, m_pRenderTarget );

//			g3dRenderOutline postOutline( m_outlineTarget, m_pRenderTarget, pNode->GetFragment()->GetMaterial(), false );
			g3dRenderOutline postOutline( m_outlineTarget, m_AATarget, pNode->GetFragment()->GetMaterial(), false );
			m_stats.m_nTriangles += postOutline.Render( i_time );
		}
		//Normal dihedral angle Inlines
		if( pmat->GetOutlineData().m_bUseNormals )
		{
			g3dBlendStateMgr::SetBlendState(st_MulNoBlend);
			m_outlineTarget->Clear( maFloatRGBA(0,0,1,0));

			g3dVkUtil::SetOverrideMaterial( m_pNormalMat );	//override object material to generate normals data.

			DrawNode( pNode, false, NULL, NULL);

			g3dVkUtil::SetOverrideMaterial(NULL);

			// now render using the outline shader to smear out the outline.
			// draw to m_tempTarget. this target contains the pre-outline rendered scene.
			// we will blend m_outlineTarget into this.
			g3dBlendStateMgr::SetBlendState(st_MulBlend);
			g3dRenderOutline postOutline( m_outlineTarget, m_AATarget, pNode->GetFragment()->GetMaterial(), true );
			m_stats.m_nTriangles += postOutline.Render( i_time );
		}
	}

	g3dBlendStateMgr::SetBlendState(st_MulBlend);

	g3dRenderAA postAA( m_AATarget, m_pRenderTarget );
	m_stats.m_nTriangles += postAA.Render( i_time );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dVkUtil::AllowAdditiveChanges( true );

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassOutline::DrawNode(const g3dSceneNode* i_pNode, bool i_bLit, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique(matShaderEffect::e_Default);
	pEffect->SetTechnique( "WorldSpaceNormal" );

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	if (i_bLit && pEffect->DoesLighting())
	{
		DBG_ASSERT(i_pLight != NULL, "null light in shdwPassOutline drawnode");
		pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());
	}

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	m_stats.m_nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

void shdwPassOutline::InitStates()
{
	const VkColorComponentFlags writeAll = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		writeAll);
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		writeAll );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState ( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		writeAll );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassOutline::CleanupStates()
{
	delete st_NoBlend;				st_NoBlend = NULL;
	delete st_MulBlend;				st_MulBlend = NULL;
	delete st_MulNoBlend;			st_MulNoBlend = NULL;
	delete ds_Test_LessE_NS;		ds_Test_LessE_NS = NULL;
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
}

}
