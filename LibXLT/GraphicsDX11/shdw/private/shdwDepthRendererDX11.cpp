/****************************************************************************\
**	shdwDepthRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwDepthRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwDepthRendererDX11::shdwDepthRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	m_pAlphaMaskMat = new matMaterial("DepthRender.fx");
	m_pMaskAlphaData = dynamic_cast<effMaskAlphaData*>(m_pAlphaMaskMat->GetEffectData());
	DBG_ASSERT(m_pMaskAlphaData, "Could not load shader needed for Depth rendering");

	m_pHairMat = new matMaterial("HairDefault.fx");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwDepthRendererDX11::~shdwDepthRendererDX11()
{
	ReleaseResources();

	delete m_pHairMat;
	delete m_pAlphaMaskMat;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwDepthRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwDepthRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwDepthRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

	// init and clear main render surface
	i_pWindow->MakeCurrent();
//	m_HDRRenderTargetTex->MakeCurrent();
//	m_HDRRenderTargetTex->Clear(maFloatRGBA(0,0,0,0));
	i_pWindow->ClearDepthStencil();

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	//g3dSceneRenderUtil::update_world_data( layers[0]->GetRootNode() );
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	g3dRenderStateTraverser* stateTraverser = m_SceneDatabase.GetRenderStateCache();

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	const g3dSceneNode* pNode = NULL;
	g3dSceneNode::DrawStyle draw_style;

	int i,n;

	// blend for ambient pass
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// simple stupid draw loop

	// 1. render eye-space depths into buffer.
	m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
	shdwPassDepth depthPass(m_pFrameBuffer->DepthBuffer(), &i_Camera);
	depthPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPass.Render(i_fSimTime);

	// 2. COPY IT INTO THE BACKBUFFER, map to grayscale
	i_pWindow->MakeCurrent();
//	CopyToBackBuf(m_pDepthBuffer, i_Camera);

	// Force our material for all renderers
	g3dDX11Util::SetOverrideMaterial(m_pAlphaMaskMat);

	// Render the layers

	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = nonShadowNodes[i].m_pNode;
		draw_style = nonShadowNodes[i].m_drawStyle;

		g3dDrawStyleUtilDX11::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = shadowNodes[i].m_pNode;
		draw_style = shadowNodes[i].m_drawStyle;

		g3dDrawStyleUtilDX11::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = nonSolidNodes[i].m_pNode;
		draw_style = nonSolidNodes[i].m_drawStyle;

		g3dDrawStyleUtilDX11::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortDX11* pHairNodes = m_SceneDatabase.GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			DrawHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	if( g3dPrefs::CurrentPrefs().m_bEnableTransparent )
	{
		g3dTransparencySortDX11* pTranspNodes = m_SceneDatabase.GetTransparentNodes();
		const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();
		for (it = tNodes.begin(); it != end; ++it)
		{
			DrawNode(it->m_pSceneNode);
		}
	}

	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
/*
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		// draw transparent
		shdwPassTransparent transparentPass(layers[0], &i_Camera, NULL, 
			NULL, m_HDRRenderTargetTex);
		transparentPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += transparentPass.Render(i_fSimTime);
	}
*/

	// Restore override material
	g3dDX11Util::SetOverrideMaterial(NULL);

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	// reset d3d state
	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwDepthRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

void shdwDepthRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwDepthRendererDX11::DrawNode" );

	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DX11Effect* pD3DEffect = i_pEffect->GetD3DXEffect();

	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_pNode->GetFragment()->GetMaterial();
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

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	D3DPERF_EndEvent();
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
void shdwDepthRendererDX11::DrawHairNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwDepthRendererDX11::DrawHairNode" );
	// set the minimal state necessary to draw depth.

	// resolve material/effect
	const matMaterial* pMaterial = m_pHairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DX11Effect* pD3DEffect = i_pEffect->GetD3DXEffect();

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetupMaterial( pMaterial );

#ifdef USE_NORMALIZED_DEPTHS
	pEffect->SetTechnique("ViewSpaceDepthN");
#else
	pEffect->SetTechnique("ViewSpaceDepth");
#endif

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
}


//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwDepthRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->DepthBuffer();
}

void shdwDepthRendererDX11::GetTightestNearFar(const camCamera& i_Camera, float& o_CamNear, float& o_CamFar)
{
	o_CamNear = i_Camera.GetNearClip();
	o_CamFar = i_Camera.GetFarClip();

	// get scene bounds.
	maAxisBox b;
	b.Union(m_SceneDatabase.m_NonShadowNodesBounds);
	b.Union(m_SceneDatabase.m_NonSolidNodesBounds);
	b.Union(m_SceneDatabase.m_ShadowNodesBounds);

	// sort bounding box pts by dist to cam.
	// assumes cam pos and dir are in same space as bounds.

	maPoint3d camPos = i_Camera.GetPosition();
	maVector3d camDir = i_Camera.GetDirection();
	maPlane camPlane(camDir, camPos);

	maPoint3d bounds[8];
	b.GetBoxPoints(bounds);
	float distances[8];
	// negative distances are behind camera.
	bool any_neg = false;
	float minDist = o_CamFar, maxDist = o_CamNear;
	for (int i = 0; i < 8; i ++)
	{
		distances[i] = camPlane.TestPoint(bounds[i]);
		if (distances[i] < minDist)
			minDist = distances[i];
		if (distances[i] > maxDist)
			maxDist = distances[i];
	}

	if (minDist > o_CamNear)
		o_CamNear = minDist;
	if (maxDist < o_CamFar)
		o_CamFar = maxDist;
}

g2dPFD::PixelFormat shdwDepthRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_Float32;
}

void shdwDepthRendererDX11::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwDepthRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_MulNoBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}