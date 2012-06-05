/****************************************************************************\
**	shdwAOPreviewRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/shdw/shdwAOPreviewRendererDX11.hpp"

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
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
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
#include "GraphicsDX11/shdw/shdwPassAOVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"
#include "GraphicsDX11/shdw/shdwPassSSAO.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"

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

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Restore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOPreviewRendererDX11::shdwAOPreviewRendererDX11()
: m_Width(0), m_Height(0)
{
	m_OverscanSize = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOPreviewRendererDX11::~shdwAOPreviewRendererDX11()
{
	ReleaseResources();
}

void shdwAOPreviewRendererDX11::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwAOPreviewRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwAOPreviewRendererDX11::Render" );

	ssaoParams p;
	i_Scene.GetSSAOSettings(p);
	m_OverscanSize = p.m_OverscanPixels;	//acquire border overscan for depth buffers before surface creation

	CreateSurfaces(i_pWindow);
	i_pWindow->MakeCurrent();

//	HRESULT op_result;

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in pick buffers
	g3dFogDX11::EnableFog(false);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// clear to white and initialize the Z buffer
	i_pWindow->Clear( maFloatRGBA(1,1,1,1), true );

	// Initialize the triangle count
	l_nNumTriangles = 0;

	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	// Force our material for all renderers
//	g3dDX11Util::SetOverrideMaterial(this->m_pAOMat);

	// Render the layers
	for (int i=0; i<layers.size(); i++)
	{
//		this->render_layer(*layers[i], i_Camera);
	}

	// Restore override material
//	g3dDX11Util::SetOverrideMaterial(NULL);

	g3dDX11Util::release_textures();

	//set initial render state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// blend in the AO stuff
	if (g3dPrefs::CurrentPrefs().m_bEnableSSAO)
	{
		if (g3dPrefs::CurrentPrefs().m_bAOInvalid)
		{
			shdwPassAOVolumes aoPass(i_pWindow, m_pFrameBuffer->HDRScratchTex2(), m_pFrameBuffer->HDRScratchTex1(), 
				m_pFrameBuffer->HDRScratchTex0(), 
				NULL,//m_pFrameBuffer->DepthBuffer(), 
				&i_Camera, p);
			aoPass.SetSceneInfo(&m_SceneDatabase);
			l_nNumTriangles += aoPass.Render(i_fSimTime);
		}
		else
		{
			m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
			m_pFrameBuffer->DepthBuffer2()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
			m_pFrameBuffer->MultiDepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
			shdwPassSSAO ssaoPass(m_pFrameBuffer->MultiDepthBuffer(), 
				m_pFrameBuffer->DepthBuffer(), 
				m_pFrameBuffer->DepthBuffer2(),
				i_pWindow, 
				m_pFrameBuffer->HDRScratchTex0(), 
				m_pFrameBuffer->HDRScratchTex1(), p);
			ssaoPass.SetSceneInfo(&m_SceneDatabase);
			ssaoPass.SetCamera(&i_Camera);
			l_nNumTriangles += ssaoPass.Render(i_fSimTime);
		}

		// now, draw icons and manipulators!
		// big assumption that everything after layer 0 can be drawn now?
		i_pWindow->MakeCurrent();
		
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

	//reset depth/stencil state, layer render alters it.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// prepare zbuffer for viewer layers to follow.
	shdwPassZFill depthPrePass( false, true );
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTriangles += depthPrePass.Render(i_fSimTime);

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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwAOPreviewRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, m_OverscanSize);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

g2dPFD::PixelFormat shdwAOPreviewRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_Color;
}

void shdwAOPreviewRendererDX11::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Restore = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwAOPreviewRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Restore );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}