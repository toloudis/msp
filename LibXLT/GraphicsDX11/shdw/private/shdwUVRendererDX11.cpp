/****************************************************************************\
**	shdwUVRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwUVRendererDX11.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

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

	matMaterial l_UVMat;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Write_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwUVRendererDX11::shdwUVRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;


	// lazy init so that this material can be reused across instantiations.
	if (!l_UVMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/Solid.fx"), matShaderMgr::GetSpecialEffect("Solid.fx"));
		l_UVMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwUVRendererDX11::~shdwUVRendererDX11()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwUVRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwUVRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwUVRendererDX11::Render" );

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

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Write_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// blend for ambient pass
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	// Render the layers
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera, layers[0]->GetModelSpace());

	// set wireframe and uv vertex transform state (this could go in DrawNode)
	g3dSingleLightRendering::SetDoBaking(true);

	if ((layers[0]->GetRootNode() != NULL) && (layers[0]->GetRootNode()->GetFragment() != NULL))
	{
		// this is nuts to tessellate and allocate/delete this fragment here...
		g3dFragment* frag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(1, 1, 2, 2);
		g3dSceneNode node(frag);

		// steal the material!
		frag->SetMaterial(layers[0]->GetRootNode()->GetFragment()->GetMaterial());

		// simple directional headlight
		g3dLightMgr::EnableHeadlight(true);
		g3dLightMgr::SetHeadlightDirection( -i_Camera.GetDirection() );

		g3dSceneRenderUtil::DrawNodeAmbient(&node, g3dAmbientEnvState());

		// restore prior state.
		g3dLightMgr::EnableHeadlight(g3dPrefs::CurrentPrefs().m_bHeadlightOn);

		// this is nuts to tessellate and allocate/delete this fragment here...
		delete frag;

		// now put the node down in wireframe on top!
		g3dDrawStyleUtilDX11::SetDrawStyle(g3dSceneNode::e_Wireframe);
		DrawNode(layers[0]->GetRootNode());
		g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	}


	// restore state (could go in DrawNode)
	g3dSingleLightRendering::SetDoBaking(false);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// reset d3d state
	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

/*
// 1. GET THE DEPTH BUFFER
	IDirect3DSurface9* pDepthBuffer = NULL;
	g2dDX11Global::g_pDevice->GetDepthStencilSurface(&pDepthBuffer);

	g2dD3D11SurfacePtr renderTargetSurface = NULL;
	m_HDRRenderTargetTex->GetTextureSurface()->GetSurfaceLevel(0, &renderTargetSurface );

	// copy surface to surface
	op_result = ::D3DXLoadSurfaceFromSurface(	renderTargetSurface,
												NULL,
												NULL,
												pDepthBuffer,
												NULL,
												NULL,
												D3DX_DEFAULT,
												0);
	CHECK_D3D_ERROR(op_result, "Failed to copy depth buffer");
	renderTargetSurface->Release();
	pDepthBuffer->Release();
	// 2. COPY IT INTO THE BACKBUFFER
	CopyToBackBuf(m_HDRRenderTargetTex);
*/

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwUVRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// test conditions for recreating surfaces:
	if ((w != m_Width) || 
		(h != m_Height))
	{
		// UNCOMMENT THIS IF RESOURCES WILL BE REALLOC'D HERE
//		g2dDX11Global::EvictManagedResources();

		// Crop the scene texture so width and height are evenly divisible by 8.
		// This cropped version of the scene will be used for post processing effects,
		// and keeping everything evenly divisible allows precise control over
		// sampling points within the shaders.
		m_Width = w;
		m_Height = h;

		// special knowledge that this render target is a window...
		g2dPFD targetPFD;
		g2dWindow* pWnd = (g2dWindow*)dynamic_cast<g2dWindow*>(i_pWindow);
		if (pWnd != NULL)
			targetPFD = pWnd->GetBackBufferPixelFormat();
		else
			targetPFD = i_pWindow->GetPixelFormat();
	}
}

void shdwUVRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	if (i_pNode == NULL)
		return;
	if (i_pNode->GetFragment() == NULL)
		return;

	// resolve material/effect
	const matMaterial* pMaterial = &l_UVMat;//g3dDX11Util::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique(matShaderEffect::e_Default);	

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

void shdwUVRendererDX11::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Write_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwUVRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Write_NS );
}