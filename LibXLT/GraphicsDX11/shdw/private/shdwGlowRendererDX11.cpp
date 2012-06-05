/****************************************************************************\
**	shdwGlowRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwGlowRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/Eff/effSolidData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
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
#include "GraphicsDX11/shdw/shdwPassAlphaFill.hpp"
#include "GraphicsDX11/shdw/shdwPassGlow.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/G2d/g2dWindowDX11.hpp"


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
	g2dPFD l_materialsPFD(g2dPFD::e_RGBA16f, 16*4);

	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwGlowRendererDX11::shdwGlowRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	m_LastNumGenerated = 0;
	m_TotalColors = 0;

	m_pMaterialsMat = new matMaterial("Solid.fx");
	m_pMaterialsData = dynamic_cast<effSolidData*>(m_pMaterialsMat->GetEffectData());
	DBG_ASSERT(m_pMaterialsData, "Could not load shader needed for Materials rendering");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwGlowRendererDX11::~shdwGlowRendererDX11()
{
	ReleaseResources();

	delete m_pMaterialsMat;

	//delete m_ColorMap;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGlowRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGlowRendererDX11::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);
	
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDX11Util::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwGlowRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, 
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwGlowRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);
	// init and clear main render surface
	// clear the zbuffer for the main window's backbuffer
	i_pWindow->ClearDepthStencil();

	// set up the ms surface then stretchrect to m_hdrrendertargettex when done drawing.
#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentGlowTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else

	g2dRenderTarget* currentGlowTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentGlowTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentGlowTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif

	//clear the target color and depth
	currentGlowTarget->Clear(maFloatRGBA(0,0,0,0),true);

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera, false);	

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

#ifdef NEW_AA_CODE
	if( !currentGlowTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
	{
		DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
	}

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

	// reset d3d state
	g3dDX11Util::release_textures();

#else
	if (m_pFrameBuffer->HDRAATarget())
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
	}
	
	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

#endif

	// glow will not contribute to matte pass
	if (g3dPrefs::CurrentPrefs().m_bEnableGlow)
	{	
		shdwPassGlow glowPass(layers[0], &i_Camera, 
			m_pFrameBuffer->HDRRenderTargetTex(), 
			m_pFrameBuffer->HDRScratchTex0());
		glowPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += glowPass.Render(i_fSimTime);
	}
	// reset d3d state
	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

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


	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGlowRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwGlowRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}

g2dPFD::PixelFormat shdwGlowRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

void shdwGlowRendererDX11::InitStates()
{
	 st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	 st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwGlowRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_MulNoBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
}

