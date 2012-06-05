#include "GraphicsDX11/shdw/shdwPassDOF.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderEffect.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"
#include "Graphics/G3d/g3dFragment.hpp"

namespace
{
	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_BlendDOF = NULL;
	g3dBlendStateMgr::BlendState* st_Restore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

shdwPassDOF::shdwPassDOF()
	: m_sceneInfo(NULL)
{
}

shdwPassDOF::~shdwPassDOF()
{
}

int shdwPassDOF::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassDOF::Render" );

	m_stats.Reset();

	// FILL ALPHA ONLY WITH BLURRINESS FACTOR
	g3dSingleLightRendering::SetDoDOFPrepPass(true);

	// set all alphas to 127 (0.5) - no blur
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetVertexShader(NULL);
//	g2dDX11Global::g_pDevice->SetPixelShader(NULL);
	g3dBlendStateMgr::SetBlendState(st_Initial, maFloatRGBA(0.5f,0.5f,0.5f,0.5f)); // or should it be 0x80808080?

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	
	g3dDX11Util::DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dBlendStateMgr::SetBlendState(st_BlendDOF);

	if (m_sceneInfo->GetLayer() == NULL)
		goto DOFPassCleanup;

	// draw geometry
	
	const camCamera* camera = m_sceneInfo->GetCamera();
	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(*camera);
	// Z Buffering
//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}

	// is there a better way to handle transparent stuff for DOF?
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dSingleLightRendering::SetDoTransparentPass(true);

//		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

		g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
		const TranspNodeVector& transpNodes = transparencySort->GetTransparentNodes();
		n = transpNodes.size();
		for (i = 0; i < n; i++)
		{
			const TranspNode& currentNode = transpNodes[i];
			
			m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pSceneNode);
		}

//		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dSingleLightRendering::SetDoTransparentPass(false);
	}

DOFPassCleanup:
	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
	g3dSingleLightRendering::SetDoDOFPrepPass(false);
	
	g3dBlendStateMgr::SetBlendState(st_Restore);
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassDOF::DrawNodeDOF(const g3dSceneNode* i_pNode)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassDOF::DrawNodeDOF" );
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique(matShaderEffect::e_DOFPrep);		
	pEffect->SetupDOFPrep();

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	pEffect->SetupMaterial( pMaterial );

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	D3DPERF_EndEvent();
	return nTriangles;
}

void shdwPassDOF::InitStates()
{
	st_Initial = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_BLEND_FACTOR,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_BLEND_FACTOR,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALPHA );
	st_BlendDOF = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALPHA);
	st_Restore = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassDOF::CleanupStates()
{
	SAFE_DELETE( st_Initial );
	SAFE_DELETE( st_BlendDOF );
	SAFE_DELETE( st_Restore );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
}