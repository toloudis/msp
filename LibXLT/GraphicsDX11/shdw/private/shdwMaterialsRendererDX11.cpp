/****************************************************************************\
**	shdwMaterialsRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwMaterialsRendererDX11.hpp"

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
#include "GraphicsDX11/shdw/shdwPassMaterials.hpp"
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
shdwMaterialsRendererDX11::shdwMaterialsRendererDX11()
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
shdwMaterialsRendererDX11::~shdwMaterialsRendererDX11()
{
	ReleaseResources();

	delete m_pMaterialsMat;

	//delete m_ColorMap;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwMaterialsRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwMaterialsRendererDX11::CopyToBackBuf(matRenderTargetTexture* pTex)
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
int shdwMaterialsRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, 
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwMaterialsRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;
	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentMaterialsTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentMaterialsTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentMaterialsTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentMaterialsTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();

	if ( g3dPrefs::CurrentPrefs().m_bLoadingScene ) {
		g3dPrefs::CurrentPrefs().m_bLoadingScene = false;
		m_SceneDatabase.ClearMaterialArray();
	}

	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera, false);	

	m_SceneMaterials = m_SceneDatabase.GetMaterialArray();

#ifndef USE_HASH_FUNC

	m_LastNumGenerated = 0;
	m_ColorMap.clear();
	for ( int i = 0 ; i < m_SceneMaterials->size() ; i++ )
	{
#ifdef USE_ALTERNATING_GEN
		m_LastNumGenerated = GenerateNextNumAlternating( 0 , 1, m_LastNumGenerated );
#else
		m_LastNumGenerated = GenerateNextNumSequential( 0 , 1, m_LastNumGenerated );
#endif
		m_ColorMap.insert(std::pair<std::string, float>(m_SceneMaterials->at(i),m_LastNumGenerated));
	}

#endif

	// 1. render eye-space materials into buffer.
	shdwPassMaterials materialsPass(currentMaterialsTarget, &i_Camera, &m_ColorMap, &m_LastNumGenerated, &m_TotalColors);
	materialsPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += materialsPass.Render(i_fSimTime);	

	//currentMaterialsTarget->ClearDepthStencil();
	shdwPassAlphaFill alphaFillPass(currentMaterialsTarget, true); 
	alphaFillPass.SetSceneInfo(&m_SceneDatabase);
	alphaFillPass.SetTransparentPassInfo(layers[0], &i_Camera, 
		m_pFrameBuffer->TransDepthBuffer1(), 
		m_pFrameBuffer->HDRScratchTex0(),
		m_pFrameBuffer->HDRScratchTex1(),
		m_pFrameBuffer->HDRScratchTex2(),
		m_pFrameBuffer->HDRAAScratchTarget(), 
		m_pFrameBuffer->IsAntiAliased());
	l_nNumTrianglesRendered += alphaFillPass.Render(i_fSimTime);

#ifdef NEW_AA_CODE
	if( !currentMaterialsTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
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

	// reset d3d state
	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	// prepare zbuffer for viewer layers to follow.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPrePass.Render(i_fSimTime);
#endif

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
void shdwMaterialsRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwMaterialsRendererDX11::GenerateNextNumSequential(float i_min, float i_max, float i_LastNum)
{
	bool foundLastNum = false;
	float numerator = i_max - i_min;
	float denominator = numerator * 2;

	while (true)
	{
		float currNum = numerator/denominator;
		
		if ( foundLastNum || i_LastNum == 0 ) 
		{
			return currNum;
		}

		if ( numerator == denominator - 1 )
		{
			numerator = 1;
			denominator *= 2;
		}
		else
		{
			numerator += 2;
		}

		if ( currNum == i_LastNum )
		{
			foundLastNum = true;
		}
		
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwMaterialsRendererDX11::GenerateNextNumAlternating(float i_min, float i_max, float i_LastNum)
{
	bool foundLastNum = false;
	float numerator = i_max - i_min;
	float denominator = numerator * 2;
	float lastNumerator = 0;

	while (true)
	{
		float currNum = numerator/denominator;

		if ( foundLastNum || i_LastNum == 0 ) 
		{
			return currNum;
		}
		
		if ( numerator == ((denominator/2) + 1) || currNum == 0.5 )
		{
			numerator = 1;
			denominator *= 2;
		}
		else
		{
			if ( currNum > 0.5 )
			{
				numerator = lastNumerator + 2;
			}
			else
			{
				lastNumerator = numerator;
				numerator = denominator - numerator;
			}
		}

		if ( currNum == i_LastNum )
		{
			foundLastNum = true;
		}

	}
}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwMaterialsRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}

g2dPFD::PixelFormat shdwMaterialsRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

void shdwMaterialsRendererDX11::InitStates()
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

void shdwMaterialsRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_MulNoBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
}