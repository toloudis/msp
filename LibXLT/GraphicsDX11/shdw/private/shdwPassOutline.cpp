#include "GraphicsDX11/shdw/shdwPassOutline.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

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
	m_stats.Reset();

	if( !g3dSingleLightRendering::GetDoSingleLightRendering()) return 0;
	if( g3dSingleLightRendering::GetDoDOFPrepPass()) return 0;

	nodeCacheList outlineNodes = m_sceneInfo->GetOutlineNodes();
	if( outlineNodes.empty()) return 0;

	// no lights?
	if( g3dLightMgrDX11::Implementation()->GetLights().size() <= 0 ) return 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0xff, 0, 0, 0xff ), L"shdwPassOutline::Render" );

	// Disable all lights and then turn them on one at a time
	g3dLightMgrDX11::Implementation()->DisableAllLights();

	g3dSingleLightRendering::SetActiveLight( NULL );
	g3dDX11Util::AllowAdditiveChanges( false);

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

			g3dDX11Util::SetOverrideMaterial( m_pDepthMat );	//override object material to generate depth data.

			DrawNode( pNode, false, NULL, NULL);

			g3dDX11Util::SetOverrideMaterial(NULL);

			// now render using the outline shader to smear out the outline.
			// draw to m_tempTarget. this target contains the pre-outline rendered scene.
			// we will blend m_outlineTarget into this.
			g3dBlendStateMgr::SetBlendState(st_MulBlend);

//			g3dDX11Util::CopyTexToTarget( m_outlineTarget, m_pRenderTarget );

//			g3dRenderOutline postOutline( m_outlineTarget, m_pRenderTarget, pNode->GetFragment()->GetMaterial(), false );
			g3dRenderOutline postOutline( m_outlineTarget, m_AATarget, pNode->GetFragment()->GetMaterial(), false );
			m_stats.m_nTriangles += postOutline.Render( i_time );
		}
		//Normal dihedral angle Inlines
		if( pmat->GetOutlineData().m_bUseNormals )
		{
			g3dBlendStateMgr::SetBlendState(st_MulNoBlend);
			m_outlineTarget->Clear( maFloatRGBA(0,0,1,0));

			g3dDX11Util::SetOverrideMaterial( m_pNormalMat );	//override object material to generate normals data.

			DrawNode( pNode, false, NULL, NULL);

			g3dDX11Util::SetOverrideMaterial(NULL);

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

	g3dDX11Util::AllowAdditiveChanges( true );

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassOutline::DrawNode(const g3dSceneNode* i_pNode, bool i_bLit, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0xff, 0, 0, 0xff ), L"shdwPassOutline::DrawNode" );
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique(matShaderEffect::e_Default);
	pEffect->SetTechnique( "WorldSpaceNormal" );

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	if (i_bLit && pEffect->DoesLighting())
	{
		DBG_ASSERT(i_pLight != NULL, "null light in shdwPassOutline drawnode");
		pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());
	}

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	m_stats.m_nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	D3DPERF_EndEvent();
}

void shdwPassOutline::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState ( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassOutline::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( st_MulNoBlend );
	SAFE_DELETE( ds_Test_LessE_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}