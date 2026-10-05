/****************************************************************************\
**	shdwAOVolumesRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwAOVolumesRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/Eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassAlphaFill.hpp"
#include "GraphicsDX11/shdw/shdwPassAOVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"

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
	g2dPFD l_normalPFD(g2dPFD::e_RGBA32f, 128);

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Restore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOVolumesRendererDX11::shdwAOVolumesRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOVolumesRendererDX11::~shdwAOVolumesRendererDX11()
{
	ReleaseResources();

//	delete m_pHairMat;
//	delete m_pNormalMat;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwAOVolumesRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwAOVolumesRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwAOVolumesRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

	g2dRenderTarget* aoTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		aoTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		aoTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}

	// init and clear main render surface
	i_pWindow->MakeCurrent();
//	m_HDRRenderTargetTex->MakeCurrent();
//	m_pNormalBuffer->Clear(maFloatRGBA(.5,.5,0,0));
	i_pWindow->Clear(maFloatRGBA(1,1,1,1), true);
/*	HRESULT op_result = g2dDX11Global::g_pDevice->Clear(	0,
		NULL,
		( g2dDX11Global::g_bHasStencil )? D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL : D3DCLEAR_ZBUFFER,
		0, 
		1.0f,
		0);
	CHECK_D3D_ERROR(op_result, "Couldn't clear zbuffer");*/

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	//g3dSceneRenderUtil::update_world_data( layers[0]->GetRootNode() );
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	/*const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	g3dRenderStateTraverser* stateTraverser = m_SceneDatabase.GetRenderStateCache();*/

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	//const g3dSceneNode* pNode = NULL;
	//g3dSceneNode::DrawStyle draw_style;
	//int i,n;

	// simple stupid draw loop
	// blend in the AO stuff
	if (g3dPrefs::CurrentPrefs().m_bEnableSSAO)
	{

		ssaoParams p;
		i_Scene.GetSSAOSettings(p);
		
		// target, positions, normals

		shdwPassAOVolumes aoPass(i_pWindow, m_pFrameBuffer->HDRScratchTex2(), m_pFrameBuffer->HDRScratchTex1(), 
			m_pFrameBuffer->HDRScratchTex0(), 
			m_pFrameBuffer->DepthBuffer(), 
			&i_Camera, p);
		aoPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += aoPass.Render(i_fSimTime);

		// now, draw icons and manipulators!
		// big assumption that everything after layer 0 can be drawn now?
		i_pWindow->MakeCurrent();
		m_pFrameBuffer->DepthBuffer()->MakeDepthCurrent();
		
		g3dBlendStateMgr::SetBlendState(st_NoBlend);
		// remaining scene layers will be overlays
		// note this is a 1-pass default renderer
		// with no special lighting passes.
		for (int i = 1; i < layers.size(); i++)
		{
			g3dRenderLayer rLayer;
			rLayer.Set(layers[i], &i_Camera, i_pWindow);
			rLayer.PerFrameInit(i_fSimTime);
			/*l_nNumTrianglesRendered += */ rLayer.Render(i_fSimTime);
		}
		// Set the camera and projection transform
		g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

		g3dBlendStateMgr::SetBlendState(st_Restore);
	}
////////////////
	// prepare zbuffer for viewer layers to follow.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPrePass.Render(i_fSimTime);

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
void shdwAOVolumesRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}
/*
void shdwAOVolumesRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique(matShaderEffect::e_Default);	

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}
*/
//--------------------------------------------------------------------
//--------------------------------------------------------------------

/*
//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
void shdwAOVolumesRendererDX11::DrawHairNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwAOVolumesRendererDX11::DrawHairNode" );
	// set the minimal state necessary to draw depth.

	// resolve material/effect
	const matMaterial* pMaterial = m_pHairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	ID3DXEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetTechnique("Tangents");

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
}
*/
//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwAOVolumesRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}


g2dPFD::PixelFormat shdwAOVolumesRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

void shdwAOVolumesRendererDX11::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Restore = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwAOVolumesRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Restore );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
}