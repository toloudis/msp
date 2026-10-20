/****************************************************************************\
**	shdwHDRRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_HDRRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwHDRRendererVk.hpp"
#endif

#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_BLURDATA_HPP
#include "Graphics/eff/effBlurData.hpp"
#endif
#ifndef EFF_DOFDATA_HPP
#include "Graphics/eff/effDOFData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_POSTPROCESSING_HPP
#include "Graphics/g3d/g3dPostProcessing.hpp"
#endif
#ifndef G3D_PASSBUFFERS_HPP
#include "Graphics/g3d/g3dPassBuffers.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_SHDW_PASSALPHAFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassAlphaFill.hpp"
#endif
#ifndef GVK_SHDW_PASSAMBIENT_HPP
#include "GraphicsVulkan/shdw/shdwPassAmbient.hpp"
#endif
#ifndef GVK_SHDW_PASSDOF_HPP
#include "GraphicsVulkan/shdw/shdwPassDOF.hpp"
#endif
#ifndef GVK_SHDW_PASSFOG_HPP
#include "GraphicsVulkan/shdw/shdwPassFog.hpp"
#endif
#ifndef GVK_SHDW_PASSSTUBSVK_HPP
#include "GraphicsVulkan/stub/shdwPassStubsVk.hpp"
#endif
#ifndef GVK_SHDW_PASSSSAO_HPP
#include "GraphicsVulkan/shdw/shdwPassSSAO.hpp"
#endif
#ifndef GVK_SHDW_PASSGLOW_HPP
#include "GraphicsVulkan/shdw/shdwPassGlow.hpp"
#endif
#ifndef GVK_SHDW_PASSOUTLINE_HPP
#include "GraphicsVulkan/shdw/shdwPassOutline.hpp"
#endif
#ifndef GVK_SHDW_PASSPOSTSHADER_HPP
#include "GraphicsVulkan/shdw/shdwPassPostShader.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_SHDW_PASSENVIRONMENT_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvironment.hpp"
#endif
#ifndef GVK_SHDW_PASSENVBACKGROUND_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvBackground.hpp"
#endif
#ifndef GVK_SHDW_PASSLIT_HPP
#include "GraphicsVulkan/shdw/shdwPassLit.hpp"
#endif
//#include "GraphicsDX11/shdw/shdwPassRSMGI.hpp"
#ifndef GVK_SHDW_PASSTONEMAP_HPP
#include "GraphicsVulkan/shdw/shdwPassToneMap.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
//#include "GraphicsDX11/shdw/shdwPassMotionBlur.hpp"
#ifndef GVK_SHDW_PASSHAIR_HPP
#include "GraphicsVulkan/shdw/shdwPassHair.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

//============================================================================
//============================================================================
unsigned int halfToFloat (unsigned short y);
unsigned short bitflip(unsigned short x);

//============================================================================
//============================================================================
namespace
{

	// Stats
	int l_nNumTrianglesRendered = 0;

	inline uint32_t F2DW( float f ) { return *((uint32_t*)&f); }

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;

	g3dBlendStateMgr::BlendState* st_IncludeAlpha1 = NULL;
	g3dBlendStateMgr::BlendState* st_IncludeAlpha2 = NULL;

	g3dBlendStateMgr::BlendState* st_CopyToBackBuf1 = NULL;
	g3dBlendStateMgr::BlendState* st_CopyToBackBuf2 = NULL;

	g3dBlendStateMgr::BlendState* st_DOF1 = NULL;
	g3dBlendStateMgr::BlendState* st_DOF2 = NULL;
	g3dBlendStateMgr::BlendState* st_DOFRestore = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Equal_NS = NULL;

//	(DX11's CopyCurrentTargetIntoTexture, a CopyResource of the current
//	target, is left out: both its calls are commented out)

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CopyBackBufInto(matRenderTargetTexture* pTex)
{

    g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf1);

    g3dDepthStencilStateMgr::SetDepthStencilState(ds_Disable_NS);

    // Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

    matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

    pEffBase->SetTechnique("SimpleCopy");

    // source is whatever g_curRenderTarget is (its image is read
    // directly: no view to make)
    vkdImage* pSrc = g2dVkGlobal::GetColorTarget();

    vkdImage* inputTextures[1] = {
        pSrc
    };

    g3dRasterizerStateMgr::SetRasterizerState(VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle());

    // save current render target and depth buf to pop them back on when we are done?
    vkdImage* pDepth = g2dVkGlobal::GetDepthTarget();
    pTex->MakeCurrent();
    int w, h;
    pTex->GetDimensions(w, h);

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

        vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad(w, h);

        pEffBase->EndPass();
    }
    pEffBase->End();

    g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf2);

    g3dDepthStencilStateMgr::SetDepthStencilState(ds_Test_Write_LessE_NS);

    g3dRasterizerStateMgr::SetRasterizerState(g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle());

    vkdImage* nullTex[1] = { NULL };
    vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

    g2dVkGlobal::SetRenderTargets(pSrc, pDepth);

}


}

void shdwHDRRendererVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	st_IncludeAlpha1 = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_A_BIT);
	st_IncludeAlpha2 = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	st_CopyToBackBuf1 = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_CopyToBackBuf2 = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	st_DOF1 = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_A_BIT);
	st_DOF2 = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_DOFRestore = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwHDRRendererVk::CleanupStates()
{
	delete st_NoBlend;
	delete st_Blend;

	delete st_IncludeAlpha1;
	delete st_IncludeAlpha2;

	delete st_CopyToBackBuf1;
	delete st_CopyToBackBuf2;

	delete st_DOF1;
	delete st_DOF2;
	delete st_DOFRestore;

	delete ds_Test_Write_LessE_NS;
	delete ds_Test_Less_NS;
	delete ds_Test_LessE_NS;
	delete ds_Disable_NS;
	delete ds_Test_Equal_NS;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwHDRRendererVk::shdwHDRRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_OverscanSize = 0;
	m_nHairShadowMapRes = -1;	//force initial update
	m_nHairShadowMapType = -1;

	m_pWindow = NULL;

	m_pToneMapper = new shdwPassToneMap;

	velocityStateManager = new VelocityStateManager();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwHDRRendererVk::~shdwHDRRendererVk()
{
	ReleaseResources();
	delete velocityStateManager;
	velocityStateManager = NULL;

	delete m_pToneMapper;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;
	m_nHairShadowMapRes = -1;	//force initial update
	m_nHairShadowMapType = -1;

	m_pFrameBuffer->ReleaseSurfaces();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwHDRRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("HDRRenderer");

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;
//	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
//	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.

	ssaoParams p;
	i_Scene.GetSSAOSettings(p);
	if (g3dPrefs::CurrentPrefs().m_bAOInvalid)
		m_OverscanSize = 0;
	else
		m_OverscanSize = p.m_OverscanPixels;	//acquire border overscan for depth buffers before surface creation


	ssgiParams ssgi_p;
	i_Scene.GetSSGISettings(ssgi_p);
	ssgi_p.m_OverscanPixels = p.m_OverscanPixels;
	//if (ssgi_p.m_OverscanPixels > p.m_OverscanPixels)
	//	m_OverscanSize = ssgi_p.m_OverscanPixels; //acquire border overscan for depth buffers before surface creation

	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);
	// init and clear main render surface
	// clear the zbuffer for the main window's backbuffer
	i_pWindow->ClearDepthStencil();

	// set up the ms surface then stretchrect to m_hdrrendertargettex when done drawing.
#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentHDRTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else

	g2dRenderTarget* currentHDRTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentHDRTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentHDRTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif

	//clear the target color and depth
	currentHDRTarget->Clear(maFloatRGBA(0,0,0,0),true);

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	g3dSceneGlobal::g_TargetRes = maVector4d((float)m_Width, (float)m_Height, 1.0f/m_Width, 1.0f/m_Height);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// Fog
	g3dFogVk::EnableFog(layers[0]->GetFogEnabled());

	// render eye-space depths into buffer.
//	shdwPassDepth depthPass(m_pDepthBuffer, &i_Camera);
//	depthPass.SetSceneInfo(&m_SceneDatabase);
//	depthPass.SetOverscanSize( m_OverscanSize );
//	depthPass.Render(i_fSimTime);

#ifndef NEW_AA_CODE
	i_pWindow->MakeCurrent();
#endif
	currentHDRTarget->MakeCurrent();

	// simple stupid draw loop

	g3dAmbientEnvState globalAmbient;
	i_Scene.GetGlobalAmbient(globalAmbient);
	if (globalAmbient.m_bEnableSwlEnv && g3dPrefs::CurrentPrefs().m_bEnableEnvironment &&
		globalAmbient.m_bEnableSwlEnvBG)
	{
		shdwPassEnvBackground EnvPrePass(currentHDRTarget, &i_Camera, globalAmbient);
		EnvPrePass.Render(i_fSimTime);
	}

	// fill Z with opaques.
	// also fill alpha = 1 with opaques
	// this allows for all passes to do correct blending.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	shdwPassZFill depthPrePass(true);
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPrePass.Render(i_fSimTime);

	// blend for ambient pass
	g3dVkUtil::AllowAdditiveChanges(true);

	if (!g3dSingleLightRendering::GetDoSingleLightRendering())
	{
		// set blend mode
		g3dVkUtil::AllowAdditiveChanges(false);

		shdwPassAmbient ambientPass;
		ambientPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += ambientPass.Render(i_fSimTime);

		if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
		{
			shdwPassTransparent transparentPass(layers[0], &i_Camera,
				m_pFrameBuffer->TransDepthBuffer1(),
				m_pFrameBuffer->TransDepthBuffer2(),
				m_pFrameBuffer->TransDepthAux(),
				m_pFrameBuffer->HDRScratchTex0(),
				m_pFrameBuffer->HDRScratchTex1(),
				currentHDRTarget);
			transparentPass.SetSceneInfo(&m_SceneDatabase);
			if( 0 == g3dPrefs::CurrentPrefs().m_TransparencyMode )
			{	// draw object sorted transparent
				l_nNumTrianglesRendered += transparentPass.Render(i_fSimTime);
			}
			else
			{
				l_nNumTrianglesRendered += transparentPass.RenderDepthPeeled(i_fSimTime);
			}
		}

		g3dVkUtil::AllowAdditiveChanges(true);
	}
	else
	{
		if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
		{
			// set blend mode
			g3dVkUtil::AllowAdditiveChanges(false);

			shdwPassEnvironment envPass;
			envPass.SetSceneInfo(&m_SceneDatabase);
			l_nNumTrianglesRendered += envPass.Render(i_fSimTime);

			g3dVkUtil::AllowAdditiveChanges(true);
		}

		if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
		{
			// set blend mode
			g3dVkUtil::AllowAdditiveChanges(false);

			shdwPassLit litPass;
			litPass.SetSceneInfo(&m_SceneDatabase);
			l_nNumTrianglesRendered += litPass.Render(i_fSimTime);

			g3dVkUtil::AllowAdditiveChanges(true);
		}

#ifdef HAIR_SUPPORTED
		if (g3dPrefs::CurrentPrefs().m_bEnableHair)
		{
			shdwPassHair hairPass( &i_Camera, m_pFrameBuffer->OSM().m_pOSM, currentHDRTarget, i_pWindow,
				m_pFrameBuffer->DepthBuffer(),
				m_pFrameBuffer->TransDepthBuffer1(), m_pFrameBuffer->TransDepthBuffer2(),
				m_pFrameBuffer->TransDepthAux(),
				m_pFrameBuffer->HDRScratchTex0(),
				m_pFrameBuffer->HDRScratchTex1());
			hairPass.SetSceneInfo(&m_SceneDatabase);
			if( g3dPrefs::CurrentPrefs().m_HairTransparencyMode < 2)
			{	// draw object sorted transparent
				l_nNumTrianglesRendered += hairPass.Render(i_fSimTime);
			}
			else if( g3dPrefs::CurrentPrefs().m_HairTransparencyMode == 2 )
			{	//draw depth peeled transparent
				l_nNumTrianglesRendered += hairPass.RenderDepthPeeled(i_fSimTime );
			}
			else
			{
				l_nNumTrianglesRendered += hairPass.Render(i_fSimTime);
			}
		}
#endif

		m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);	//clear before AO for restored depth later
		if (g3dPrefs::CurrentPrefs().m_bEnableSSAO)
		{
			if (g3dPrefs::CurrentPrefs().m_bAOInvalid)
			{
				shdwPassAOVolumes aoPass(currentHDRTarget, m_pFrameBuffer->HDRScratchTex2(), m_pFrameBuffer->HDRScratchTex1(),
					m_pFrameBuffer->HDRScratchTex0(),
					m_pFrameBuffer->DepthBuffer(),
					&i_Camera, p);
				aoPass.SetSceneInfo(&m_SceneDatabase);
				l_nNumTrianglesRendered += aoPass.Render(i_fSimTime);
			}
			else
			{
				m_pFrameBuffer->DepthBuffer2()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
				m_pFrameBuffer->MultiDepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
				shdwPassSSAO ssaoPass(m_pFrameBuffer->MultiDepthBuffer(),
					m_pFrameBuffer->DepthBuffer(),
					m_pFrameBuffer->DepthBuffer2(),
					currentHDRTarget,
					m_pFrameBuffer->HDRScratchTex0(),
					m_pFrameBuffer->HDRScratchTex1(), p);
				ssaoPass.SetSceneInfo(&m_SceneDatabase);
				ssaoPass.SetCamera(&i_Camera);
				l_nNumTrianglesRendered += ssaoPass.Render(i_fSimTime);
			}
		}

		camPassBuffersData passBuffersData;
		i_Camera.GetPassBuffersParams(passBuffersData);
		float top, bottom, left, right;
		i_Camera.GetSubViewport(top, bottom, left, right);
		if ( passBuffersData.m_ReflBuffer && g3dPrefs::CurrentPrefs().m_bEnableReflection && g3dPassBuffers::GetDoingFileRefl() )
		{
			g3dVkUtil::BlendBuffers(currentHDRTarget,passBuffersData.m_ReflBuffer,passBuffersData.m_ReflBlendOp, passBuffersData.m_ReflIntensity,
									  top, bottom, left, right);
		}

		m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);	//clear before GI for restored depth later
		if (g3dPrefs::CurrentPrefs().m_bEnableSSGI)
		{
#ifdef NEW_AA_CODE
			currentHDRTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex() );
#else
			if (m_pFrameBuffer->HDRAATarget() != NULL)
			{
				m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
			}
#endif

#ifdef ENABLE_GI_VOLUME
			// bReuseBuff decide if we reuse the normal/position buffer
			// Also to control multipling AO result to GI
			// TODO: the mutiplication only works when AO volume is enabled. Also it has
			// the problem that AO/GI texture might have differen size!
			bool bReuseBuff = (g3dPrefs::CurrentPrefs().m_bEnableSSAO && g3dPrefs::CurrentPrefs().m_bAOInvalid);
			shdwPassGIVolumes giPass(currentHDRTarget, m_pFrameBuffer->HDRBufferBackup(), m_pFrameBuffer->HDRScratchTex1(),
					m_pFrameBuffer->HDRScratchTex0(),
					m_pFrameBuffer->DepthBuffer(),
					&i_Camera, ssgi_p, bReuseBuff,
					m_pFrameBuffer->HDRScratchTex2());
				giPass.SetSceneInfo(&m_SceneDatabase);
				l_nNumTrianglesRendered += giPass.Render(i_fSimTime);

#else
			if (g3dPrefs::CurrentPrefs().m_bGIInvalid)
			{
				shdwPassLPVGI lpvGIPass(ssgi_p,
					m_pFrameBuffer->HDRScratchTex0(),
					m_pFrameBuffer->DepthBuffer(),
					currentHDRTarget,
					&i_Camera,
					&i_Scene);
				lpvGIPass.SetSceneInfo(&m_SceneDatabase);
				lpvGIPass.Render(i_fSimTime);
			}
			else
			{
				m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
				m_pFrameBuffer->DepthBuffer2()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
				m_pFrameBuffer->MultiDepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
				m_pFrameBuffer->MultiDepthBuffer()->MakeCurrent();
				shdwPassSSGI ssgiPass(m_pFrameBuffer->MultiDepthBuffer(),
					m_pFrameBuffer->DepthBuffer(),
					m_pFrameBuffer->DepthBuffer2(),
					currentHDRTarget,
					m_pFrameBuffer->HDRScratchTex0(),
					m_pFrameBuffer->HDRScratchTex1(),
					ssgi_p,
					m_pFrameBuffer->HDRRenderTargetTex());
				ssgiPass.SetSceneInfo(&m_SceneDatabase);
				ssgiPass.SetCamera(&i_Camera);
				l_nNumTrianglesRendered += ssgiPass.Render(i_fSimTime);
			}
#endif
		}
	}

	// blend for ambient pass
#ifndef NEW_AA_CODE
	i_pWindow->MakeCurrent();			//restore depth buffer after SSAO
#endif
	currentHDRTarget->MakeCurrent();

	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		shdwPassTransparent transparentPass(layers[0], &i_Camera,
			m_pFrameBuffer->TransDepthBuffer1(),
			m_pFrameBuffer->TransDepthBuffer2(),
			m_pFrameBuffer->TransDepthAux(),
			m_pFrameBuffer->HDRScratchTex0(),
			m_pFrameBuffer->HDRScratchTex1(),
			currentHDRTarget);
		transparentPass.SetSceneInfo(&m_SceneDatabase);
		if( 0 == g3dPrefs::CurrentPrefs().m_TransparencyMode )
		{	// draw object sorted transparent
			l_nNumTrianglesRendered += transparentPass.Render(i_fSimTime);
		}
		else
		{
			l_nNumTrianglesRendered += transparentPass.RenderDepthPeeled(i_fSimTime);
		}
	}

#ifndef NEW_AA_CODE
	i_pWindow->MakeCurrent();			//restore depth buffer after SSAO
#endif
	currentHDRTarget->MakeCurrent();

	// alpha fill pass (this is responsible for the final alpha mask)
	// Fill alpha with additive alpha blending mode
	shdwPassAlphaFill alphaFillPass(currentHDRTarget);
	alphaFillPass.SetSceneInfo(&m_SceneDatabase);
	// must set the transparent pass info if rendering actual alpha
	alphaFillPass.SetTransparentPassInfo(layers[0], &i_Camera,
		m_pFrameBuffer->TransDepthBuffer1(), // not anti-aliased!
		m_pFrameBuffer->HDRScratchTex0(), // not anti-aliased!
		m_pFrameBuffer->HDRScratchTex1(), // not anti-aliased!
		m_pFrameBuffer->HDRScratchTex2(), // not anti-aliased!
		m_pFrameBuffer->HDRAAScratchTarget(),
		m_pFrameBuffer->IsAntiAliased());
	l_nNumTrianglesRendered += alphaFillPass.Render(i_fSimTime);

	camPassBuffersData passBuffersData;
	i_Camera.GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	i_Camera.GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_BeautyBuffer )
	{
		g3dVkUtil::BlendBuffers(currentHDRTarget,passBuffersData.m_BeautyBuffer, passBuffersData.m_BeautyBlendOp, passBuffersData.m_BeautyIntensity,
								  top, bottom, left, right);
	}

	// the point of this is to freeze the alphas before glow and bloom happen.
	// it will later be used in IncludeAlpha
#ifdef NEW_AA_CODE
	if( !currentHDRTarget->CopyWithResolve( m_pFrameBuffer->HDRBufferBackup() ))
	{
		DBG_TRACE( "HDR backup buffer Copy/Resolve failed!" );
	}
#else
	if (m_pFrameBuffer->HDRAATarget() != NULL)
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRBufferBackup());
	}
	else
	{
		g3dVkUtil::CopyBackBufferToRenderTargetTex(m_pFrameBuffer->HDRBufferBackup());
	}
#endif

	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent && g3dPrefs::CurrentPrefs().m_bEnableDeferredTransparency)
	{
		shdwPassTransparent transparentPass(layers[0], &i_Camera,
			m_pFrameBuffer->TransDepthBuffer1(),
			m_pFrameBuffer->TransDepthBuffer2(),
			m_pFrameBuffer->TransDepthAux(),
			m_pFrameBuffer->HDRScratchTex0(),
			m_pFrameBuffer->HDRScratchTex1(),
			currentHDRTarget);
		transparentPass.SetSceneInfo(&m_SceneDatabase);

		// draw deferred transparent after glow happens, for correct z buffering.
		l_nNumTrianglesRendered += transparentPass.RenderDeferred(i_fSimTime);
	}

	// Done drawing: resolve hdr multisamples to the texture.
	// This should be done AFTER tone mapping but it is impossible
	// in DX11 to tone map the subpixel msaa samples.

	// Restore depth for non AA target
#ifndef NEW_AA_CODE
	i_pWindow->MakeCurrent();
#endif

	//------------------------------------------------------------------------
	// No more writing to current target after this point
	// only HDRRenderTargetTex which is non MSAA and readable
	//------------------------------------------------------------------------
#ifdef NEW_AA_CODE
	if( !currentHDRTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
	{
		DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
	}
#else
	if (m_pFrameBuffer->HDRAATarget() != NULL)
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());

		//also resolve the HDR depth buffer to the targets depth.
		//Since the HDRAA target has it's own depth buffer the primary window target has no info.
		if( i_pWindow->GetHasDepthBuffer() && m_pFrameBuffer->HDRAATarget()->GetHasDepthBuffer() )
		{
			g3dVkUtil::CopyDepth( m_pFrameBuffer->HDRAATarget(), i_pWindow );
		}
	}
#endif

	// glow will not contribute to matte pass
	if (g3dSingleLightRendering::GetDoSingleLightRendering() &&
		!g3dSingleLightRendering::GetDoDOFPrepPass() &&
		g3dPrefs::CurrentPrefs().m_bEnableGlow &&
		!g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		shdwPassGlow glowPass(layers[0], &i_Camera,
			m_pFrameBuffer->HDRRenderTargetTex(),
			m_pFrameBuffer->HDRScratchTex0());
		glowPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += glowPass.Render(i_fSimTime);
	}

	// outline will not contribute to matte pass
	if (g3dSingleLightRendering::GetDoSingleLightRendering() &&
		!g3dSingleLightRendering::GetDoDOFPrepPass() &&
		g3dPrefs::CurrentPrefs().m_bEnableOutline &&
		!g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		shdwPassOutline outlinePass(m_pFrameBuffer->HDRScratchTex0(),
			m_pFrameBuffer->HDRScratchTex1(), m_pFrameBuffer->HDRRenderTargetTex(), &m_SceneDatabase );
		l_nNumTrianglesRendered += outlinePass.Render(i_fSimTime);
	}

	// should this happen after tone map?  HDR colors will be mapped toward the fog color which is not HDR.
	// only do fog if not computing a reflection map.
	if (
		!g3dSingleLightRendering::GetDoReflectionGen() &&
		i_Scene.IsFogEnabled())
	{
		m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
		shdwPassFog fogPass(m_pFrameBuffer->HDRRenderTargetTex(), m_pFrameBuffer->DepthBuffer(),
			m_pFrameBuffer->HDRScratchTex0(), &i_Scene, &i_Camera);
		fogPass.SetSceneInfo(&m_SceneDatabase);
		fogPass.Render(i_fSimTime);
	}

#ifdef ENABLE_MOTIONBLUR
	//motion blur after full scene render and overlay passes
	if ( g3dPrefs::CurrentPrefs().m_bMotionBlurEnable &&
		!g3dSingleLightRendering::GetDoDOFPrepPass() )
	{
		//// SAMPLE VELOCITY PASS CALLS
		shdwPassVelocity velocityPass(m_pVelocityBuffer, velocityStateManager, &i_Camera, &i_Scene );
		velocityPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += velocityPass.Render(i_fSimTime);

		//post process the scene with motion blur
		MotionBlurParams mparms;
		i_Scene.GetMotionBlurSettings(mparms);
		shdwPassMotionBlur MBPass( m_HDRRenderTargetTex, m_HDRScratchTex0, m_pVelocityBuffer, mparms );
		MBPass.Render(i_fSimTime);
	}
#endif

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	// reset d3d state
	g3dVkUtil::release_textures();

	// bypass all tone mapping for cube map reflection render!
	if (g3dSingleLightRendering::GetDoReflectionGen())
	{
		g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

		//covert HDR to LDR and copy to frame buffer
		g3dVkUtil::CopyTexToTarget(m_pFrameBuffer->HDRRenderTargetTex(), i_pWindow);

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
		return l_nNumTrianglesRendered;
	}

	//------------------------------------------------------------------------
	// Convert HDR to LDR and copy to frame buffer
	// all post processing effects should write to the framebuffer and read
	// from previous non MSAA HDR textures.
	// Bring along the depths for depth effects

//------------------------------------------------------------------------
// No more writing to HDR targets after this point
//------------------------------------------------------------------------

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

	PostProcessing( i_pWindow, i_Camera, i_Scene, i_fSimTime );

	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}
	IncludeAlphas(i_fSimTime);

	//clear out texture for next pass
	g3dVkUtil::release_textures();

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);

	return l_nNumTrianglesRendered;
}

void shdwHDRRendererVk::PostProcessing( g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene &i_Scene, float i_fSimTime)
{
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	//////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////

	m_pToneMapper->Setup(i_Scene, i_Camera);

	// Create a scaled copy of the scene
	m_pToneMapper->Scene_To_SceneScaled(m_pFrameBuffer->HDRScaledTex(), m_pFrameBuffer->HDRRenderTargetTex());

	// Now that luminance information has been gathered, the scene can be bright-pass filtered
	// to remove everything except bright lights and reflections.
	m_pToneMapper->SceneScaled_To_BrightPass(m_pFrameBuffer->HDRScaledTex(), m_pFrameBuffer->TexBrightPass());

	// Blur the bright-pass filtered image to create the source texture for the star effect
	m_pToneMapper->BrightPass_To_StarSource(m_pFrameBuffer->TexBrightPass(), m_pFrameBuffer->TexStarSource());

	// Scale-down the source texture for the star effect to create the source texture
	// for the bloom effect
	m_pToneMapper->StarSource_To_BloomSource(m_pFrameBuffer->TexStarSource(), m_pFrameBuffer->TexBloomSource());

	// Render post-process lighting effects
	m_pToneMapper->RenderBloom(m_pFrameBuffer->TexBloomSource(), m_pFrameBuffer->TexBloom().m_apTexBloom);
	m_pToneMapper->RenderStar(m_pFrameBuffer->TexStarSource(), m_pFrameBuffer->TexStar().m_apTexStar);


	switch( g3dPrefs::CurrentPrefs().m_HDRDebugMode )
	{
	case 0:
		{

			//////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////
			if (g3dPrefs::CurrentPrefs().m_bRenderMatte)
			{

				g3dBlendStateMgr::SetBlendState(st_NoBlend);
				g3dRenderAlpha mattePass( m_pFrameBuffer->HDRRenderTargetTex(), i_pWindow);

				l_nNumTrianglesRendered += mattePass.Render(i_fSimTime);

				// render star + bloom on top.
				g3dBlendStateMgr::SetBlendState(st_Blend);

				g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

				LuminanceToGrayscale(m_pFrameBuffer->TexBloom().m_apTexBloom[0], "MIN_MAG_LINEAR_MIP_POINT");
				LuminanceToGrayscale(m_pFrameBuffer->TexStar().m_apTexStar[0], "MIN_MAG_LINEAR_MIP_POINT");
				g3dBlendStateMgr::SetBlendState(st_NoBlend);
			}
			else
			{
				// main render just does tonemap and be done with it
				m_pToneMapper->ToneMap(m_pWindow, m_pFrameBuffer->HDRRenderTargetTex(),
					m_pFrameBuffer->TexBloom().m_apTexBloom[0],
					m_pFrameBuffer->TexStar().m_apTexStar[0],
					m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]);
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////
			// DOF

			camDOFData dof;
			i_Camera.GetDOFParams(dof);
			if (g3dPrefs::CurrentPrefs().m_bEnableDOF && (dof.m_MaxFarBlur != -1))
			{
				// set into global 3d rendering state
				g3dSceneGlobal::g_DOFParams.m_FarBlurDist = dof.m_FarBlurDist;
				g3dSceneGlobal::g_DOFParams.m_FarFocalDist = dof.m_FarFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearFocalDist = dof.m_NearFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearBlurDist = dof.m_NearBlurDist;
				g3dSceneGlobal::g_DOFParams.m_MaxFarBlur = dof.m_MaxFarBlur;
				g3dSceneGlobal::g_DOFParams.m_MaxCoC = dof.m_MaxCoC;
				RenderDOF(i_fSimTime, false);
			}

			// overlay layers need the current z buffer.
			// in non-msaa, i have the window's zbuf.
			// in msaa mode, the zbuf is msaa'd so i should use the depth buffer z buffer!

			// note this will not antialias the layers.
			// to antialias, clear the msaa buffer, make current,
			// render to it, then resolve to hdrrendertarget and blend onto i_pWindow
			i_pWindow->MakeCurrent();

			// overscan causes this buffer to be a mismatch with the color buffer.
			//m_pFrameBuffer->DepthBuffer()->MakeDepthCurrent();
			//			i_pWindow->ClearDepthStencil();
			//			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

			//CopyCurrentTargetIntoTexture(m_pFrameBuffer->HDRScratchTex1());

            CopyBackBufInto(m_pFrameBuffer->HDRScratchTex1());

            if (g3dPostProcessing::GetActive())
			{
				shdwPassPostShader postPass(m_pFrameBuffer->HDRScratchTex0(), m_pFrameBuffer->HDRScratchTex1());
				l_nNumTrianglesRendered += postPass.Render(i_fSimTime);
			}
			CopyToBackBuf(m_pFrameBuffer->HDRScratchTex1());


			//			shdwPassZFill depthPrePass;
			//			depthPrePass.SetSceneInfo(&m_SceneDatabase);
			//			l_nNumTrianglesRendered += depthPrePass.Render(i_fSimTime);

			// remaining scene layers will be overlays
			// note this is a 1-pass default renderer
			// with no special lighting passes.
			for (int i = 1; i < layers.size(); i++)
			{
				g3dRenderLayer rLayer;
				rLayer.Set(layers[i], &i_Camera, i_pWindow);
				rLayer.PerFrameInit(i_fSimTime);
				l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
			}
			// Also add in the extra layers that are specific to this viewer
			//			for (int i = 0; i < i_ViewerLayers.size(); i++)
			//			{
			//				g3dRenderLayer rLayer;
			//				rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
			//				rLayer.PerFrameInit(i_fSimTime);
			//				l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
			//			}
		}
		break;
	case 1:
		CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());
		break;
	case 2:
		CopyToBackBuf(m_pFrameBuffer->HDRScaledTex());
		break;
	case 3:
		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		LuminanceToGrayscale(m_pFrameBuffer->HDRRenderTargetTex());
		g3dBlendStateMgr::SetBlendState(st_Blend);

		break;
	case 4:
		{
			// just put something in the backbuf
			CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

			// DOF
			camDOFData dof;
			i_Camera.GetDOFParams(dof);
			if (g3dPrefs::CurrentPrefs().m_bEnableDOF && (dof.m_MaxFarBlur != -1))
			{
				// set into global 3d rendering state
				g3dSceneGlobal::g_DOFParams.m_FarBlurDist = dof.m_FarBlurDist;
				g3dSceneGlobal::g_DOFParams.m_FarFocalDist = dof.m_FarFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearFocalDist = dof.m_NearFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearBlurDist = dof.m_NearBlurDist;
				g3dSceneGlobal::g_DOFParams.m_MaxFarBlur = dof.m_MaxFarBlur;
				g3dSceneGlobal::g_DOFParams.m_MaxCoC = dof.m_MaxCoC;
				RenderDOF(i_fSimTime, true);
			}
		}
		break;
	case 5:
		CopyToBackBuf(m_pFrameBuffer->TexToneMap().m_apTexToneMap[NUM_TONEMAP_TEXTURES-1], false);
		break;
	case 6:
		CopyToBackBuf(m_pFrameBuffer->TexBrightPass());
		break;
	case 7:
		CopyToBackBuf(m_pFrameBuffer->TexBloomSource());
		break;
	case 8:
		CopyToBackBuf(m_pFrameBuffer->TexBloom().m_apTexBloom[0]);
		break;
	case 9:
		CopyToBackBuf(m_pFrameBuffer->TexStar().m_apTexStar[0]);
		break;
	case 10:
		if (g3dSingleLightRendering::GetDoSingleLightRendering() &&
			g3dPrefs::CurrentPrefs().m_bEnableSSAO)
		{
			CopyToBackBuf(m_pFrameBuffer->MultiDepthBuffer());
		}
		break;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::Slideshow()
{
	static int nSlides = 9;
	static int iView = 0;
	static int nFramesToHold = 24;
	switch( ((iView++)/nFramesToHold) % nSlides)
	{
	case 0:
		m_pToneMapper->ToneMap(m_pWindow, m_pFrameBuffer->HDRRenderTargetTex(),
				m_pFrameBuffer->TexBloom().m_apTexBloom[0],
				m_pFrameBuffer->TexStar().m_apTexStar[0],
				m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]);
		break;
	case 1:
		CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());
		break;
	case 2:
		CopyToBackBuf(m_pFrameBuffer->HDRScaledTex());
		break;
	case 3:
		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		LuminanceToGrayscale(m_pFrameBuffer->HDRRenderTargetTex());
		g3dBlendStateMgr::SetBlendState(st_Blend);

		break;
	case 4:
		CopyToBackBuf(m_pFrameBuffer->TexToneMap().m_apTexToneMap[NUM_TONEMAP_TEXTURES-1], false);
		break;
	case 5:
		CopyToBackBuf(m_pFrameBuffer->TexBrightPass());
		break;
	case 6:
		CopyToBackBuf(m_pFrameBuffer->TexBloomSource());
		break;
	case 7:
		CopyToBackBuf(m_pFrameBuffer->TexBloom().m_apTexBloom[0]);
		break;
	case 8:
		CopyToBackBuf(m_pFrameBuffer->TexStar().m_apTexStar[0]);
		break;
	}
}

void shdwHDRRendererVk::IncludeAlphas(float i_fSimTime)
{
//	g3dRenderAlpha mattePass(m_pHDRBufferBackup, m_pMatteTarget);
//	mattePass.Render(i_fSimTime);


	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique("SimpleCopyLDR");

	// depth not needed for this step.
	g2dVkGlobal::SetDepthTarget(NULL);

	// first, put alphas into hdr target.
	m_pFrameBuffer->HDRRenderTargetTex()->MakeCurrent();
	int w,h;
	m_pFrameBuffer->HDRRenderTargetTex()->GetDimensions(w,h);

	g3dBlendStateMgr::SetBlendState(st_IncludeAlpha1);

//    g2dDX11Global::g_pDevice->SetTexture( 0, m_pMatteTarget->GetImage() );
	vkdImage* inputTextures[1] = {
		m_pFrameBuffer->HDRBufferBackup()->GetImage()
	};
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uint32_t uiPassCount, uiPass;
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );
        pEffBase->EndPass();
    }
    pEffBase->End();

	// then, put alphas into main window target
	m_pWindow->MakeCurrent();
	m_pWindow->GetDimensions(w,h);

	uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );
        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_IncludeAlpha2);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	vkdImage* nullTex[1] = {NULL};
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::CopyToBackBuf(matRenderTargetTexture* pTex, bool bDoBlend, bool isRGB)
{

	g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf1);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

//	g3dRenderFullScreenQuad fsq(m_HDRRenderTargetTex, m_pWindow, true);
//	fsq.Render(0);


// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique(isRGB?"SimpleCopyLDR":"SimpleCopyLumLDR");

	m_pWindow->MakeCurrent();
	int w,h;
	m_pWindow->GetDimensions(w,h);
    //g2dDX11Global::g_pDevice->SetRenderTarget(0, m_pWindow);

	vkdImage* inputTextures[1] = {
		pTex->GetImage()
	};
//	g2dDX11Global::g_pDevice->SetTexture( 0, pTex->GetImage() );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT  );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf2);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	vkdImage* nullTex[1] = {NULL};
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::LuminanceToGrayscale(matRenderTargetTexture* pTex, const char* i_pTexFilter /* = "MIN_MAG_MIP_POINT" */)
{

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique("DrawLuminance");

	m_pWindow->MakeCurrent();
	int w,h;
	m_pWindow->GetDimensions(w,h);
    //g2dDX11Global::g_pDevice->SetRenderTarget(0, m_pWindow);

	vkdImage* inputTextures[1] = {
		pTex->GetImage()
	};
//    g2dDX11Global::g_pDevice->SetTexture( 0, pTex->GetImage() );

	// using POINT sampling by default now. Ignoring the passed in parameter.
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, i_D3DTexFilter );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, i_D3DTexFilter );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::RenderDOF(float i_fSimTime, bool i_debug)
{
//	HRESULT op_result;

	m_pWindow->MakeCurrent();
//	m_pWindow->ClearDepthStencil();

	// replace alpha in HDR render target with blurriness factor

	// draw this to a AA buffer if AA enabled.
	// then resolve to one of our scratch buffers.
	// replace alpha in render target with blurriness factor
#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentHDRTarget = m_pFrameBuffer->HDRRenderTarget();
#else
	g2dRenderTarget* currentHDRTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentHDRTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentHDRTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif

	currentHDRTarget->MakeCurrent();
//	currentHDRTarget->ClearDepthStencil();
	shdwPassDOF dofPass;
	dofPass.SetSceneInfo(&m_SceneDatabase);
	dofPass.Render(i_fSimTime);

#ifdef NEW_AA_CODE
	currentHDRTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex() );
#else
	if (currentHDRTarget == m_pFrameBuffer->HDRAATarget())
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
	}
#endif

	// Now, prepare blurred version of color buffer:

	m_pWindow->MakeCurrent();

	// Copy backbuffer to DOF Reserve Target.
	// This will be the source texture with color data.
//	CopyCurrentTargetIntoTexture(m_pFrameBuffer->DOFReserveTarget());
    CopyBackBufInto(m_pFrameBuffer->DOFReserveTarget());

	// now put alphas in m_pDOFReserveTarget.
	m_pFrameBuffer->DOFReserveTarget()->MakeCurrent();
	int w,h;
	m_pFrameBuffer->DOFReserveTarget()->GetDimensions(w,h);
	g3dBlendStateMgr::SetBlendState(st_DOF1);

//	g2dDX11Global::g_pDevice->SetVertexShader(NULL);
//	g2dDX11Global::g_pDevice->SetPixelShader(NULL);

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	pEffBase->SetTechnique("SimpleCopyLDR");
	vkdImage* inputTextures[1] = {
		m_pFrameBuffer->HDRRenderTargetTex()->GetImage()
	};
//	g2dDX11Global::g_pDevice->SetTexture( 0, m_pFrameBuffer->HDRRenderTargetTex()->GetSurface() );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT  );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uint32_t uiPassCount, uiPass;
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );
        pEffBase->EndPass();
    }
    pEffBase->End();

//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_DOF2);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	m_pWindow->MakeCurrent();
	if (i_debug)
		DrawAlpha(m_pFrameBuffer->DOFReserveTarget(), m_pWindow);
	else
	{
	// run the Blur effect on this data
		matShaderEffect* effBlur = matShaderMgr::GetSpecialEffect("Blur");
		effBlurData blurData;
		blurData.m_pSceneTexture = m_pFrameBuffer->DOFReserveTarget();
		blurData.m_pDownsampledTexture = m_pFrameBuffer->DOFBlurTarget();
		blurData.m_pHorizontalBlurTexture = m_pFrameBuffer->DOFHBlurTarget();
		effBlur->SetupParams(&blurData);

		vkdImage* nullTex[1] = {NULL};

		g3dBlendStateMgr::SetBlendState(st_DOF2);

		effBlur->SetTechnique(matShaderEffect::e_Default);
		int nPasses = effBlur->Begin();
		// DBG_ASSERT(nPasses == 3, "Wrong number of passes in blur shader");

		m_pFrameBuffer->DOFBlurTarget()->MakeCurrent();
		effBlur->BeginPass(0);
		g3dRenderFullScreenQuad fsq0(m_pFrameBuffer->DOFReserveTarget(), m_pFrameBuffer->DOFBlurTarget());
		l_nNumTrianglesRendered += fsq0.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		m_pFrameBuffer->DOFHBlurTarget()->MakeCurrent();
		effBlur->BeginPass(1);
		g3dRenderFullScreenQuad fsq1(m_pFrameBuffer->DOFBlurTarget(), m_pFrameBuffer->DOFHBlurTarget());
		l_nNumTrianglesRendered += fsq1.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		m_pFrameBuffer->DOFBlurTarget()->MakeCurrent();
		effBlur->BeginPass(2);
		g3dRenderFullScreenQuad fsq2(m_pFrameBuffer->DOFHBlurTarget(), m_pFrameBuffer->DOFBlurTarget());
		l_nNumTrianglesRendered += fsq2.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		effBlur->End();

		g3dBlendStateMgr::SetBlendState(st_DOFRestore);
		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////

		// post process full screen quad
		g3dRenderDOF rpass_DOF(m_pFrameBuffer->DOFReserveTarget(), m_pFrameBuffer->DOFBlurTarget(), m_pWindow);
		l_nNumTrianglesRendered += rpass_DOF.Render(i_fSimTime);
	}
}

//	(DX11's CheckForNaN, ReadFirstFloat and WriteFirstFloat, D3D9 code that is
//	compiled out or commented out, are left out)
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, m_OverscanSize);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
	m_dwCropWidth = (w - w % 8);
	m_dwCropHeight = (h - h % 8);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
unsigned short bitflip(unsigned short x)
{
	unsigned short retval=0;
	int i;
	int bit1;
	for (i = 0; i < 16; i++)
	{
		// pull the last bit off x
		bit1 = x & 1;
		// stuff it in the correct position in retval
		retval = retval | (bit1<<(15-i));
		// discard the used bit in x and advance the current bit to the last position.
		x = x >> 1;
	}
	return retval;
}
//---------------------------------------------------
// Interpret an unsigned short bit pattern as a half,
// and convert that half to the corresponding float's
// bit pattern.
//---------------------------------------------------
unsigned int halfToFloat (unsigned short y)
{
	int s = (y >> 15) & 0x00000001;
	int e = (y >> 10) & 0x0000001f;
	int m = y & 0x000003ff;
	if (e == 0)
	{
		if (m == 0)
		{
			//
			// Plus or minus zero
			//
			return s << 31;
		}
		else
		{
			//
			// Denormalized number -- renormalize it
			//
			while (!(m & 0x00000400))
			{
				m <<= 1;
				e -= 1;
			}
			e += 1;
			m &= ~0x00000400;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			//
			// Positive or negative infinity
			//
			return (s << 31) | 0x7f800000;
		}
		else
		{
			//
			// Nan -- preserve sign and significand bits
			//
			return (s << 31) | 0x7f800000 | (m << 13);
		}
	}
	//
	// Normalized number
	//
	e = e + (127 - 15);
	m = m << 13;
	//
	// Assemble s, e and m.
	//
	return (s << 31) | (e << 23) | m;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwHDRRendererVk::GetLastAvgLuminance()
{
	return 1;//ReadFirstFloat(m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::SetAvgLuminance(float i_Luminance)
{
//	PDIRECT3DSURFACE9 pSurfToneMap = NULL;
//	HRESULT hr = m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfToneMap );
//	if ( !FAILED(hr) )
//		WriteFirstFloat(pSurfToneMap, i_Luminance);
//	SAFE_RELEASE(pSurfToneMap);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwHDRRendererVk::DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest)
{
    uint32_t uiPassCount, uiPass;


	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique("DrawAlpha");

	i_dest->MakeCurrent();
	int w,h;
	i_dest->GetDimensions(w,h);

	vkdImage* inputTextures[1] = {
		i_src->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_src->GetImage() );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT  );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
        vkdGlobal::Context().SetShaderResources(0,1,inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

}

g2dRenderTarget* shdwHDRRendererVk::GetRawBuffer()
{
	//return NULL;
	return m_pFrameBuffer->HDRRenderTargetTex();
}

g2dPFD::PixelFormat shdwHDRRendererVk::GetRawBufferPFD()
{
	//return g2dPFD::e_Color;
	return g2dPFD::e_RGBA16f;
}

}
