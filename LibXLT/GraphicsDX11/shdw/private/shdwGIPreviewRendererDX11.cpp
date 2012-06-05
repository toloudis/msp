/****************************************************************************\
**	shdwGIPreviewRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/shdw/shdwGIPreviewRendererDX11.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
//#include "GraphicsDX11/g2d/g2dRenderTargetDX11.hpp"
#include "GraphicsDX11/G2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
//#include "GraphicsDX11/shdw/shdwAORendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/shdw/shdwPassEnvironment.hpp"
#include "GraphicsDX11/shdw/shdwPassGIVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassLPVGI.hpp"
#include "GraphicsDX11/shdw/shdwPassLit.hpp"
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"
#include "GraphicsDX11/shdw/shdwPassSSAO.hpp"
#include "GraphicsDX11/shdw/shdwPassSSGI.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/shdw/shdwPassHair.hpp"
#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

namespace
{
	// Stats
	int l_nNumTriangles = 0;

	// the hdr pixel format we use.
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 16*4);

	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwGIPreviewRendererDX11::shdwGIPreviewRendererDX11()
: m_Width(0), m_Height(0)
{
	m_OverscanSize = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwGIPreviewRendererDX11::~shdwGIPreviewRendererDX11()
{
	ReleaseResources();
}

void shdwGIPreviewRendererDX11::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwGIPreviewRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwGIPreviewRendererDX11::Render" );

	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

	ssaoParams p;
	i_Scene.GetSSAOSettings(p);
	//m_OverscanSize = p.m_OverscanPixels;

	ssgiParams ssgi_p;
	i_Scene.GetSSGISettings(ssgi_p);
	m_OverscanSize = ssgi_p.m_OverscanPixels;	//acquire border overscan for depth buffers before surface creation

	CreateSurfaces(i_pWindow);

#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentRenderTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentRenderTarget = i_pWindow;
	i_pWindow->Clear(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));

#ifndef ENABLE_GI_VOLUME
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentRenderTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentRenderTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif
	i_pWindow->MakeCurrent();
#endif
	currentRenderTarget->MakeCurrent();

//	HRESULT op_result;

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in pick buffers
	g3dFogDX11::EnableFog(false);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// clear to white and initialize the Z buffer
	currentRenderTarget->Clear( maFloatRGBA(0,0,0,1), true );
	// Initialize the triangle count
	l_nNumTriangles = 0;

	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	//set initial render state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

#ifndef ENABLE_GI_VOLUME
	// fill Z with opaques. 
	// also fill alpha = 1 with opaques
	// this allows for all passes to do correct blending.
	shdwPassZFill depthPrePassFirst(true);
	depthPrePassFirst.SetSceneInfo(&m_SceneDatabase);
	l_nNumTriangles += depthPrePassFirst.Render(i_fSimTime);

	// blend for ambient pass
	g3dDX11Util::AllowAdditiveChanges(true);
	// turn off alpha writes, since the zfill pass did it.
	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
	{
		// set blend mode
		g3dDX11Util::AllowAdditiveChanges(false);

		shdwPassEnvironment envPass;
		envPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTriangles += envPass.Render(i_fSimTime);

		g3dDX11Util::AllowAdditiveChanges(true);
		g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);
	}

	if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
	{
		// set blend mode
		g3dDX11Util::AllowAdditiveChanges(false);

		shdwPassLit litPass;
		litPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTriangles += litPass.Render(i_fSimTime);

		g3dDX11Util::AllowAdditiveChanges(true);
		g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);
	}
#ifdef HAIR_SUPPORTED

	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		shdwPassHair hairPass( &i_Camera, m_pFrameBuffer->OSM().m_pOSM, currentRenderTarget, i_pWindow, 
			m_pFrameBuffer->DepthBuffer(), 
			m_pFrameBuffer->TransDepthBuffer1(), m_pFrameBuffer->TransDepthBuffer2(), 
			m_pFrameBuffer->TransDepthAux(), 
			m_pFrameBuffer->HDRScratchTex0(), 
			m_pFrameBuffer->HDRScratchTex1());
		hairPass.SetSceneInfo(&m_SceneDatabase);
		if( g3dPrefs::CurrentPrefs().m_HairTransparencyMode < 2)
		{	// draw object sorted transparent
			l_nNumTriangles += hairPass.Render(i_fSimTime);
		}
		else if( g3dPrefs::CurrentPrefs().m_HairTransparencyMode == 2 )
		{	//draw depth peeled transparent
			l_nNumTriangles += hairPass.RenderDepthPeeled(i_fSimTime );
		}
		else
		{
			l_nNumTriangles += hairPass.Render(i_fSimTime);
		}
	}
#endif

	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		shdwPassTransparent transparentPass(layers[0], &i_Camera, 
			m_pFrameBuffer->TransDepthBuffer1(), 
			m_pFrameBuffer->TransDepthBuffer2(), 
			m_pFrameBuffer->TransDepthAux(), 
			m_pFrameBuffer->HDRScratchTex0(), 
			m_pFrameBuffer->HDRScratchTex1(),
			currentRenderTarget);
		transparentPass.SetSceneInfo(&m_SceneDatabase);
		if( 0 == g3dPrefs::CurrentPrefs().m_TransparencyMode )
		{	// draw object sorted transparent
			l_nNumTriangles += transparentPass.Render(i_fSimTime);
		}
		else
		{
			l_nNumTriangles += transparentPass.RenderDepthPeeled(i_fSimTime);
		}
	}

#endif

	// blend in the AO stuff
	m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
	if (g3dPrefs::CurrentPrefs().m_bEnableSSGI)
	{
#ifdef ENABLE_GI_VOLUME
		/*shdwPassGIVolumes giPass(i_pWindow, m_pFrameBuffer->HDRScratchTex2(), m_pFrameBuffer->HDRScratchTex1(), 
			m_pFrameBuffer->HDRScratchTex0(), 
			NULL,//m_pFrameBuffer->DepthBuffer(), 
			&i_Camera, ssgi_p);
		giPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTriangles += giPass.Render(i_fSimTime);*/

		shdwPassLPVGI lpvGIPass(ssgi_p,
			m_pFrameBuffer->HDRScratchTex0(),
			m_pFrameBuffer->DepthBuffer(),
			i_pWindow,
			&i_Camera,
			&i_Scene);
		lpvGIPass.SetSceneInfo(&m_SceneDatabase);
		lpvGIPass.Render(i_fSimTime);
#else

#ifdef NEW_AA_CODE
		// Copy color buffer to tex
		if( !currentRenderTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
		{
			DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
		}
#else 
		// Copy color buffer to tex
		if (m_pFrameBuffer->HDRAATarget() != NULL)
		{
			m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
		}
#endif		
		if (g3dPrefs::CurrentPrefs().m_bGIInvalid)
		{
			shdwPassLPVGI lpvGIPass(ssgi_p,
				m_pFrameBuffer->HDRScratchTex0(),
				m_pFrameBuffer->DepthBuffer(),
				i_pWindow,
				&i_Camera,
				&i_Scene);
			lpvGIPass.SetSceneInfo(&m_SceneDatabase);
			lpvGIPass.Render(i_fSimTime);
		}
		else
		{
			m_pFrameBuffer->DepthBuffer2()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
			m_pFrameBuffer->MultiDepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
			m_pFrameBuffer->MultiDepthBuffer()->MakeCurrent();
			m_pFrameBuffer->HDRScratchTex2()->GetRenderTargetAPI()->Clear(maFloatRGBA(0,0,0,0));
			m_pFrameBuffer->HDRScratchTex2()->MakeCurrent();
			shdwPassSSGI ssgiPass(m_pFrameBuffer->MultiDepthBuffer(), 
				m_pFrameBuffer->DepthBuffer(), 
				m_pFrameBuffer->DepthBuffer2(),
				m_pFrameBuffer->HDRScratchTex2()->GetRenderTargetAPI(), 
				m_pFrameBuffer->HDRScratchTex0(), 
				m_pFrameBuffer->HDRScratchTex1(), 
				ssgi_p, 
				m_pFrameBuffer->HDRRenderTargetTex());
			ssgiPass.SetSceneInfo(&m_SceneDatabase);
			ssgiPass.SetCamera(&i_Camera);
			l_nNumTriangles += ssgiPass.Render(i_fSimTime);

			CopyToBackBuf(m_pFrameBuffer->HDRScratchTex2());
		}
#endif
	}

#ifndef NEW_AA_CODE
	// prepare zbuffer for viewer layers to follow.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTriangles += depthPrePass.Render(i_fSimTime);
#endif

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTriangles += rLayer.Render(i_fSimTime);
	}


{//PROFILE("debug");
	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	g2dScreen::SetDebugInfo(2, num);
}
	D3DPERF_EndEvent();
	return l_nNumTriangles;
}

//int shdwGIPreviewRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
//{
//	// resolve material/effect
//	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
//	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
//	pEffect->SetTechnique(matShaderEffect::e_Default);
//
//	// set shader globals
//	g3dDX11Util::SetupShaderGlobals(pEffect);
//
//	// geometry data
//	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);
//
//	// shading data
//	pEffect->SetupMaterial(pMaterial);
//	//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);
//
//	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
//	return nTriangles;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGIPreviewRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, m_OverscanSize);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

g2dPFD::PixelFormat shdwGIPreviewRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_Color;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGIPreviewRendererDX11::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);


	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dDX11Util::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
}

void shdwGIPreviewRendererDX11::InitStates()
{
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwGIPreviewRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_BlendNoAlpha );
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( st_MulNoBlend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}