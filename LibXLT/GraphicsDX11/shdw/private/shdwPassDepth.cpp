/****************************************************************************\
**	shdwPassDepth.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"

namespace
{
	matMaterial l_DepthMat;
	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* stp_RenderInit = NULL;
	g3dBlendStateMgr::BlendState* stp_NoColor = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassDepth::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	stp_RenderInit = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_NoColor = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 0 );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
}

void shdwPassDepth::CleanupStates()
{
	delete stp_RenderInit;
	delete stp_NoColor;
	delete dsp_Disable_NS;
	delete dsp_Test_Write_LessE_NS;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassDepth::shdwPassDepth()
: m_sceneInfo(NULL), m_pOrigCamera( NULL ), 
m_pSrcDepthBuffer( NULL ), m_bCompareLess( false ), m_bRenderInvisible( false ),
m_bOnlyDepths( false )
{
	m_eTechnique = eDefault;
}

shdwPassDepth::shdwPassDepth( g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera, bool i_bRenderInvisible, matTexture* i_pSrcDepth, eTechnique i_eTech, bool i_bOnlyDepths )
	: m_sceneInfo(NULL), m_pOrigCamera( i_pCamera ), 
	m_pSrcDepthBuffer( i_pSrcDepth ), m_eTechnique( i_eTech ), m_bRenderInvisible(i_bRenderInvisible),
	m_Camera( *i_pCamera ), m_bOnlyDepths( i_bOnlyDepths )
{
	m_pRenderTarget = i_pDepthTarget;
	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();

	//copy the camera so we can modify it for overscan;
//	m_Camera = camCamera( *i_pCamera );

	// lazy init so that this material can be reused across instantiations.
	if (!l_DepthMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthRender.fx"), matShaderMgr::GetSpecialEffect("DepthRender.fx"));
		l_DepthMat.SetShaderParams(p);
	}

	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}

	m_OverscanPixels = 0;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassDepth::~shdwPassDepth()
{
}

//----------------------------------------------------------------------------------------
// This function renders non-transparent objects depth values into the target
// Note that the target will always be cleared.
//----------------------------------------------------------------------------------------
int shdwPassDepth::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassDepth::Render" );
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	int i,n;
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	
	/*if (m_bRenderInvisible)
	{
		const nodeCacheList& invisibleMaskBlackNodes = m_sceneInfo->GetInvisibleMaskBlackNodes();
		n = invisibleMaskBlackNodes.size();
		for (i = 0; i < n; i++)
		{
			const sNodePlusState& currentNode = invisibleMaskBlackNodes[i];
			m_stats.m_nTriangles += RenderNode(currentNode);		
		}
	}*/

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders only transparent objects depth values into the target
// Note that the target will only be cleared only in reversed mode (near to far)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderTransparent()
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassDepth::RenderTransparent" );
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	g3dTransparencySortDX11* pTranspNodes = m_sceneInfo->GetTransparentNodes();
	const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
	TranspNodeVector::const_iterator it, end = tNodes.end();

	for (it = tNodes.begin(); it != end; ++it)
	{
		sNodePlusState NS;
		NS.m_pNode = it->m_pSceneNode;
		NS.m_StateCache = it->m_RenderStateCache;
		m_stats.m_nTriangles += RenderNode( NS );
	}

	/*if (m_bRenderInvisible)
	{
		const nodeCacheList& invisibleMaskBlackTransparentNodes = m_sceneInfo->GetInvisibleMaskTransparentNodes();
		int n = invisibleMaskBlackTransparentNodes.size();
		for (int i = 0; i < n; i++)
		{
			const sNodePlusState& currentNode = invisibleMaskBlackTransparentNodes[i];
			m_stats.m_nTriangles += RenderNode(currentNode);
		}
	}*/

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders only hair objects depth values into the target
// Note that the target will only be cleared in reversed mode (near to far)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderHair()
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassDepth::RenderHair" );
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortDX11* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			m_stats.m_nTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}


//----------------------------------------------------------------------------------------
// This function renders a single node
// if no source is specified then all depths will be rendered
//   (an optional transparency value will be tested to remove non solid pixels)
// otherwise one of two depth peeling modes will be used
//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_DepthMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DX11Effect* pD3DEffect = i_pEffect->GetD3DXEffect();

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	if( pOrigMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pOrigMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pOrigMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pD3DEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pD3DEffect->GetVariableByName("TransparencyMap")->AsShaderResource()->SetResource(
						(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat(val);

	if( m_pSrcDepthBuffer )
	{
		pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(
						g3dDX11TextureUtil::GetD3DTexture(m_pSrcDepthBuffer));
	}

	int dWidth, dHeight;
	m_pRenderTarget->GetDimensions( dWidth, dHeight );

	float InvResololution[2] = {0};
	InvResololution[0] = 1.0f / dWidth;
	InvResololution[1] = 1.0f / dHeight;
	pD3DEffect->GetVariableByName("g_InvScreenSize")->AsVector()->SetFloatVector(InvResololution);

	float bias = 0.0f;
//	float AlphaLevel = 254.0f/255.0f;

	switch( m_eTechnique )
	{
		case eDefault:
		{
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenPeelLess:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelLess");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenPeelGreater:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelGreater");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenBiased:
		{
			bias = 0.00001f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenAlpha:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepth");
			break;
		}
		case eViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelLess");
			break;
		}
		case eViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelGreater");
			break;
		}
		case eNView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepth");
			break;
		}
		case eNViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelLess");
			break;
		}
		case eNViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelGreater");
			break;
		}
	}
	pD3DEffect->GetVariableByName("g_bias")->AsScalar()->SetFloat(bias);
//	pD3DEffect->GetVariableByName("g_alphaThreshold")->AsScalar()->SetFloat(AlphaLevel);


	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	//unbind texture to prevent D3D read/write dependency error
	//pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(NULL);
	ID3D11ShaderResourceView* nullTex[2] = {NULL,NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 2, nullTex);

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DX11Effect* pD3DEffect = i_pEffect->GetD3DXEffect();

	effStrandHairData& HairData = pMaterial->StrandHairData();
	HairData.Default();

	int W, H;
	m_pRenderTarget->GetDimensions( W, H );
	HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}

	HairData.m_ZNear = m_Near;
	HairData.m_ZFar = m_Far;

	//set depth map
	HairData.m_pDepthTexture = m_pSrcDepthBuffer;

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	if( pOrigMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pOrigMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pOrigMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pD3DEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pD3DEffect->GetVariableByName("TransparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat(val);

	if( m_pSrcDepthBuffer )
	{
		pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(
		g3dDX11TextureUtil::GetD3DTexture(m_pSrcDepthBuffer));
	}

	int dWidth, dHeight;
	m_pRenderTarget->GetDimensions( dWidth, dHeight );

	float InvResololution[2];
	InvResololution[0] = 1.0f / dWidth;
	InvResololution[1] = 1.0f / dHeight;
	pD3DEffect->GetVariableByName("g_InvScreenSize")->AsVector()->SetFloatVector(InvResololution);

	float bias = 0.0f;
//	float AlphaLevel = 254.0f/255.0f;

	switch( m_eTechnique )
	{
		case eDefault:
		{
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenPeelLess:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelLess");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenPeelGreater:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelGreater");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenBiased:
		{
			bias = 0.00001f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenAlpha:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepth");
			break;
		}
		case eViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelLess");
			break;
		}
		case eViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelGreater");
			break;
		}
		case eNView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepth");
			break;
		}
		case eNViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelLess");
			break;
		}
		case eNViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelGreater");
			break;
		}
	}

	pD3DEffect->GetVariableByName("g_bias")->AsScalar()->SetFloat(bias);
//	pD3DEffect->GetVariableByName("g_alphaThreshold")->AsScalar()->SetFloat(AlphaLevel);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	//unbind texture to prevent D3D read/write dependancy error
	pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(NULL);

	return nTriangles;
}


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::SetupMinimalNearFar()
{
#if 0
	// total bounds of all objects to be rendered
	maAxisBox bounds;
	bounds.Union(m_sceneInfo->m_NonShadowNodesBounds);
	bounds.Union(m_sceneInfo->m_ShadowNodesBounds);
	bounds.Union(m_sceneInfo->m_NonSolidNodesBounds);
	// put bounds into cam space
	maAxisBox camBounds = g3dDX11Util::XForm(bounds, g3dSceneGlobal::GetCameraTransform());
	// get near/far and compare with camera near/far.
	float bn = camBounds.GetMinZ();
	float bf = camBounds.GetMaxZ();
	m_Near = m_pCamera->GetNearClip();
	m_Far = m_pCamera->GetFarClip();
	if (bn > m_Near)
		m_Near = bn;
	if (bf < m_Far)
		m_Far = bf;
	// now m_Near and m_Far are the tightest allowable camera space z bounds. 
	// we are going to map [near,far] to [-1,1] to maximize fp precision
	// z' = ((z-N)/(F-N)) * 2 - 1
	// in the shader! and all shaders that need to extract z:
	// z = ((z'+1)*0.5)*(F-N) + N

	matShaderEffect* pEffect = matShaderMgr::GetEffect(l_DepthMat);
	int i = pEffect->GetParamIndex("ZNear");
	pEffect->SetFloat(i, m_Near);
	i = pEffect->GetParamIndex("ZFar");
	pEffect->SetFloat(i, m_Far);
#endif
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::AdjustCameraOverscan()
{
	int Width, Height;
	m_pRenderTarget->GetDimensions( Width, Height );
	
	float WidthRatio = Width / (float)(Width-(m_OverscanPixels<<1));
	float HeightRatio = Height / (float)(Height-(m_OverscanPixels<<1));

	// add buffer to subviewport
	// assuming the standard vprt is -1..1, and we now have -widthRatio..widthRatio,
	// and assuming widthRatio and heightratio >= 1 :
	float dx = WidthRatio-1.0f;
	float dy = HeightRatio-1.0f;
	float vt=-1,vb=1,vl=-1,vr=1;
	m_pOrigCamera->GetSubViewport(vt,vb,vl,vr);
	m_Camera.SetSubViewport(vt-dy, vb+dy, vl-dx, vr+dx);
	m_Camera.Set();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::ColorTexture(g2dRenderTarget* i_pRenderTarget, maFloatRGBA& i_Color )
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("HDRLighting.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = pEffect->GetTechniqueByName("SimpleColor");

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );
	g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);

	pEffect->GetVariableByName("g_ColorTint")->AsVector()->SetFloatVector( i_Color.Ptr() );

	pEffectTechnique->GetPassByIndex(0)->Apply(0, g2dDX11Global::g_pDeviceContext);
	g3dDX11Util::DrawFullScreenQuad( w,h );
	

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState( &st_OldBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
}