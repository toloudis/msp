/****************************************************************************\
**	shdwDepthRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwVelocityMapRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"
//#include "Graphics/cam/camCamera.hpp"
#include "Graphics/Eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
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
#include "GraphicsDX11/shdw/shdwPassVelocity.hpp"

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
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwVelocityMapRendererDX11::shdwVelocityMapRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	m_pVelocityStateManager = new VelocityStateManager();

	m_lastTime = 0;

	m_pVelocityMat = new matMaterial("VelocityRender.fx");
	m_pVelocityData = dynamic_cast<effTexturedData*>(m_pVelocityMat->GetEffectData());
	DBG_ASSERT(m_pVelocityData, "Could not load shader needed for Velocity rendering");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwVelocityMapRendererDX11::~shdwVelocityMapRendererDX11()
{
	ReleaseResources();

	delete m_pVelocityMat;

	delete m_pVelocityStateManager;
	m_pVelocityStateManager = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwVelocityMapRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwVelocityMapRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwVelocityMapRendererDX11::Render" );
	
	// Assign shader
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("VelocityRender.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;
	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

	// Velocity map pass
	shdwPassVelocity velocityPass(m_pFrameBuffer->VelocityBuffer(), m_pVelocityStateManager, &i_Camera, &i_Scene );
	velocityPass.SetSceneInfo(&m_SceneDatabase);
	velocityPass.SetOldTime(m_lastTime);
	l_nNumTrianglesRendered += velocityPass.Render(i_fSimTime);
	
	// Read in velocity map now stored in m_pVelocityBuffer, and display it!
	i_pWindow->MakeCurrent();
	int w,h;
	i_pWindow->GetDimensions(w,h);

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("DisplayVelocity");

	//	pEffect->SetTexture("velocityBuffer", m_pFrameBuffer->VelocityBuffer()->GetSurface());
	pEffect->GetVariableByName("velocityBuffer")->AsShaderResource()->SetResource( m_pFrameBuffer->VelocityBuffer()->GetSurface() );

	// Render it on a viewport via full-screen quad
	
	// Motion blur are disabled, these new blend states have never been tested.
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0, g2dDX11Global::g_pDeviceContext);
	g3dDX11Util::DrawFullScreenQuad( w,h );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

// Motion blur are disabled, these new blend states have never been tested.
	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// reset d3d state
	g3dDX11Util::release_textures();
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	D3DPERF_EndEvent();

	m_lastTime = i_fSimTime;

	m_pVelocityStateManager->UpdatePrevTransforms();
	velocityPass.ClearObjects();

	return l_nNumTrianglesRendered;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwVelocityMapRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

void shdwVelocityMapRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	// Set draw style
	g3dDrawStyleUtilDX11::SetDrawStyle(i_pNode->GetDrawStyle());

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);
	// DrawNode is never used in this class, if it's used in the future the blend state need to be properly set here
	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwVelocityMapRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->VelocityBuffer();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dPFD::PixelFormat shdwVelocityMapRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

void shdwVelocityMapRendererDX11::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwVelocityMapRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Blend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}