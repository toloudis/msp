/****************************************************************************\
**	shdwPassOpacity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassOpacity.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
//#include "Graphics/eff/effStrandHairData.hpp"
#include "GraphicsDX11/eff/effStrandHair.hpp"
#include "GraphicsDX11/Mat/matVolumeTexture.hpp"

//define to have depths output between 0-1
//#define USE_NORMALIZED_DEPTHS

//#undef HAIR_SUPPORTED

namespace
{
	matMaterial l_OpacityMat;

	g3dBlendStateMgr::BlendState st_OldBlend;

	g3dBlendStateMgr::BlendState* st_Render = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Equal_NS = NULL;

//	ID3D11UnorderedAccessView* l_pOpacityView = NULL;
	matTexture* l_pOpacityView = NULL;
}; // namespace

shdwPassOpacity::shdwPassOpacity(matRenderTargetTexture* (&i_pTargets)[8], const camCamera* i_pCamera, matRenderTargetTexture* i_pSrcDepth )
	: m_sceneInfo(NULL), m_pOrigCamera( i_pCamera ), m_pSrcDepthBuffer( i_pSrcDepth )
{
	m_pRenderTarget = i_pTargets[0];

	for( int i = 0; i < 8; i++ )
	{
		m_pTargets[i] = i_pTargets[i];
	}

	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();

	//copy the camera so we can modify it for overscan;
	m_Camera = camCamera( *i_pCamera );

	// lazy init so that this material can be reused across instantiations.
	if (!l_OpacityMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/OpacityRender.fx"), matShaderMgr::GetSpecialEffect("OpacityRender.fx"));
		l_OpacityMat.SetShaderParams(p);
	}
	m_OverscanPixels = 0;
}

shdwPassOpacity::~shdwPassOpacity()
{
}

void shdwPassOpacity::SetCamera( const camCamera* i_pCamera )//sets a new camera
{
//	m_pOrigCamera = i_pCamera;
	//copy the camera so we can modify it for overscan;
	m_Camera = camCamera( *i_pCamera );
	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();
}


void shdwPassOpacity::SetSrcDepth( matRenderTargetTexture* i_src )
{
	m_pSrcDepthBuffer = i_src;
}

void shdwPassOpacity::CalculateNodeBounds()
{
	// put bounds into cam space
	maMatrix4x4 camMat;
	m_Camera.GetCameraMatrix( camMat );
	maAxisBox nodeBounds;

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortDX11* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			const maAxisBox& bounds = it->m_pSceneNode->GetWorldBox();
			maAxisBox camBounds = g3dDX11Util::XForm( bounds, camMat );
			nodeBounds.Union( camBounds );
		}
	}
#endif//HAIR_SUPPORTED

	// get near/far and compare with camera near/far.
	float bn = nodeBounds.GetMinZ();
	float bf = nodeBounds.GetMaxZ();
	m_Near = m_Camera.GetNearClip();
	m_Far = m_Camera.GetFarClip();
	if( bn > m_Near) m_Near = bn;
	if( bf < m_Far) m_Far = bf;
}

int shdwPassOpacity::DrawNodes()
{
	int numTriangles = 0;

	int i,n;
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		numTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		numTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		numTriangles += RenderNode(currentNode);
	}

	//transparent nodes
	g3dTransparencySortDX11* pTranspNodes = m_sceneInfo->GetTransparentNodes();
	const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
	TranspNodeVector::const_iterator it, end = tNodes.end();

	for (it = tNodes.begin(); it != end; ++it)
	{
		sNodePlusState NS;
		NS.m_pNode = it->m_pSceneNode;
		NS.m_StateCache = it->m_RenderStateCache;
		numTriangles += RenderNode( NS );
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortDX11* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			numTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	return numTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders non-transparent objects depth values into the target
// Note that the target will always be cleared.
//----------------------------------------------------------------------------------------
int shdwPassOpacity::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassOpacity::Render" );
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g2dD3D11RenderTargetPtr	CTargets[8];

	for( int t = 0; t < 8; t++ )
	{
		CTargets[t] = m_pTargets[t] ? m_pTargets[t]->GetColorBuffer() : NULL;
	}
	g2dDX11Global::g_pDeviceContext->OMSetRenderTargets(8, CTargets, m_pTargets[0]->GetDepthBuffer() );

	//clear the color to 0's, and depth to 1;
	m_pRenderTarget->Clear( maFloatRGBA(), true );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );
	g3dBlendStateMgr::SetBlendState( st_Render );

	CalculateNodeBounds();

	m_stats.m_nTriangles += DrawNodes();

	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState( &st_OldBlend );

	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );


	//g3dLightMgrDX11::Implementation()->DisableAllLights();
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

int shdwPassOpacity::RenderNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassOpacity::RenderNode" );
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_OpacityMat;
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

	if( l_pOpacityView )
	{
//		pEffect->SetTechnique("OpacityShadowMap");
		pEffect->SetTechnique("OpacityShadowVolume");

		matVolumeTexture* pOpacityVol = dynamic_cast<matVolumeTexture*>(l_pOpacityView);
		ID3D11UnorderedAccessView* pOpacityVolView = pOpacityVol->GetUnorderedAccessView();
		pD3DEffect->GetVariableByName("OpacityShadowBuffer3D")->AsUnorderedAccessView()->SetUnorderedAccessView( pOpacityVolView );
	}
	else
	{
		const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
		if( pOrigMaterial->GetHasTransparency() )
		{
			float val = 0;
			matTexture* pTex = NULL;
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

			if( pTex )
			{
				pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( true );
				pD3DEffect->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pTex) );
				pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat( val );
//				pD3DEffect->SetBool( "hasDiffuseMap", true );
//				pD3DEffect->SetTexture( "diffuseMap" , g3dDX11TextureUtil::GetD3DTexture(pTex));
//				pD3DEffect->SetFloat( "g_Transparency", val );
			}
		}
		else
		{
//			pD3DEffect->SetBool( "hasDiffuseMap", false );
			pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( false );
		}

		if( m_pTargets[7] )	//set 32 layer since we have multiple targets
		{
			pEffect->SetTechnique("OpacityShadowMap32");
		}
		else if( m_pTargets[3] )	//set 16 layer since we have multiple targets
		{
			pEffect->SetTechnique("OpacityShadowMap16");
		}
		else
		{
			pEffect->SetTechnique("OpacityShadowMap4");
		}
	}

//	pD3DEffect->GetVariableByName("OpacityShadowTexture3D")->AsUnorderedAccessView()->SetUnorderedAccessView( l_pOpacityView );

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
// if no source is specified then all depths will be rendered
//   (an optional transparency value will be tested to remove non solid pixels)
// otherwise one of two depth peeling modes will be used
//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
//----------------------------------------------------------------------------------------
int shdwPassOpacity::RenderHairNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassOpacity::RenderHairNode" );
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_OpacityMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DX11Effect* pD3DEffect = i_pEffect->GetD3DXEffect();

//	effStrandHairData HairData;
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

	HairData.m_pDepthTexture = m_pSrcDepthBuffer;


//	effStrandHair* pHairEffect = dynamic_cast<effStrandHair*>(pEffect);

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);


/*
	if( pHairEffect )
	{
		HairData.m_ZNear = m_Near;
		HairData.m_ZFar = m_Far;
		HairData.m_pDepthTexture = m_pSrcDepthBuffer;
		pHairEffect->SetHairData( HairData );
	}
*/
	if( l_pOpacityView )
	{
//		pEffect->SetTechnique("HairOpacityShadowMap");
		pEffect->SetTechnique("HairOpacityShadowVolume");

		matVolumeTexture* pOpacityVol = dynamic_cast<matVolumeTexture*>(l_pOpacityView);
		ID3D11UnorderedAccessView* pOpacityVolView = pOpacityVol->GetUnorderedAccessView();
		pD3DEffect->GetVariableByName("OpacityShadowBuffer3D")->AsUnorderedAccessView()->SetUnorderedAccessView( pOpacityVolView );
	}
	else
	{
		const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
		if( pOrigMaterial->GetHasTransparency() )
		{
			float val = 1;
			matTexture* pTex = NULL;
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

			if( pTex )
			{
//				pD3DEffect->SetBool( "hasDiffuseMap", true );
//				pD3DEffect->SetTexture( "diffuseMap" , g3dDX11TextureUtil::GetD3DTexture(pTex));

				pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( true );
				pD3DEffect->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pTex) );
			}
//			pD3DEffect->SetFloat( "g_Transparency", val );
			pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat( val );
		}
		else
		{
//			pD3DEffect->SetBool( "hasDiffuseMap", false );
			pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( false );
		}


		if( m_pTargets[7] )	//set 32 layer since we have multiple targets
		{
			pEffect->SetTechnique("HairOpacityShadowMap32");
		}
		else if( m_pTargets[3] )	//set 16 layer since we have multiple targets
		{
			pEffect->SetTechnique("HairOpacityShadowMap16");
		}
		else
		{
			pEffect->SetTechnique("HairOpacityShadowMap4");
		}
	}

	// shading data
	pEffect->SetupMaterial(pMaterial);
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}


void shdwPassOpacity::SetupNodeBounds( g3dSceneNode* i_pSceneNode )
{
	const maAxisBox& bounds = i_pSceneNode->GetWorldBox();
	// put bounds into cam space
	maMatrix4x4 camMat;
	m_Camera.GetCameraMatrix( camMat );
	maAxisBox camBounds = g3dDX11Util::XForm(bounds, camMat );
	// get near/far and compare with camera near/far.
	float bn = camBounds.GetMinZ();
	float bf = camBounds.GetMaxZ();
	m_Near = m_Camera.GetNearClip();
	m_Far = m_Camera.GetFarClip();
	if( bn > m_Near) m_Near = bn;
	if( bf < m_Far) m_Far = bf;
	// now m_Near and m_Far are the tightest allowable camera space z bounds. 
	// we are going to map [near,far] to [-1,1] to maximize fp precision
	// z' = ((z-N)/(F-N)) * 2 - 1
	// in the shader! and all shaders that need to extract z:
	// z = ((z'+1)*0.5)*(F-N) + N
}

void shdwPassOpacity::AdjustCameraOverscan()
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

void shdwPassOpacity::InitStates()
{
	st_Render = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassOpacity::CleanupStates()
{
	SAFE_DELETE( st_Render );
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_Less_NS );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Equal_NS );
}

void shdwPassOpacity::SetOpacityVolume( matTexture* i_pOpacityView )
{
	l_pOpacityView = i_pOpacityView;
}

void shdwPassOpacity::SetBounds( float Min, float Max )
{
	m_Near = Min;
	m_Far = Max;

	//Clamps within camera range
	if( m_Near < m_Camera.GetNearClip() ) m_Near = m_Camera.GetNearClip();
	if( m_Far > m_Camera.GetFarClip() ) m_Far = m_Camera.GetFarClip();
}