/****************************************************************************\
**	shdwGIPreviewRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_GIPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGIPreviewRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/eff/effOcclusionData.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/eff/effTexturedData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
//#include "GraphicsVk/g2d/g2dRenderTargetVk.hpp"
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
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
//#include "GraphicsVk/shdw/shdwAORendererVk.hpp"
#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_SHDW_PASSENVIRONMENT_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvironment.hpp"
#endif
#ifndef GVK_SHDW_PASSLPVGI_HPP
#include "GraphicsVulkan/shdw/shdwPassLPVGI.hpp"
#endif
#ifndef GVK_SHDW_PASSLIT_HPP
#include "GraphicsVulkan/shdw/shdwPassLit.hpp"
#endif
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif
#ifndef GVK_SHDW_PASSSSAO_HPP
#include "GraphicsVulkan/shdw/shdwPassSSAO.hpp"
#endif
#ifndef GVK_SHDW_PASSSSGI_HPP
#include "GraphicsVulkan/shdw/shdwPassSSGI.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef GVK_SHDW_PASSHAIR_HPP
#include "GraphicsVulkan/shdw/shdwPassHair.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

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
shdwGIPreviewRendererVk::shdwGIPreviewRendererVk()
: m_Width(0), m_Height(0)
{
	m_OverscanSize = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwGIPreviewRendererVk::~shdwGIPreviewRendererVk()
{
	ReleaseResources();
}

void shdwGIPreviewRendererVk::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwGIPreviewRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("GIPreviewRenderer");

	m_pWindow = i_pWindow;

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
	g3dFogVk::EnableFog(false);

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

	g3dVkUtil::release_textures();

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
	g3dVkUtil::AllowAdditiveChanges(true);
	// turn off alpha writes, since the zfill pass did it.
	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
	{
		// set blend mode
		g3dVkUtil::AllowAdditiveChanges(false);

		shdwPassEnvironment envPass;
		envPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTriangles += envPass.Render(i_fSimTime);

		g3dVkUtil::AllowAdditiveChanges(true);
		g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);
	}

	if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
	{
		// set blend mode
		g3dVkUtil::AllowAdditiveChanges(false);

		shdwPassLit litPass;
		litPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTriangles += litPass.Render(i_fSimTime);

		g3dVkUtil::AllowAdditiveChanges(true);
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
	return l_nNumTriangles;
}

//int shdwGIPreviewRendererVk::DrawNode(const g3dSceneNode* i_pNode)
//{
//	// resolve material/effect
//	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
//	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
//	pEffect->SetTechnique(matShaderEffect::e_Default);
//
//	// set shader globals
//	g3dVkUtil::SetupShaderGlobals(pEffect);
//
//	// geometry data
//	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);
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
void shdwGIPreviewRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, m_OverscanSize);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

g2dPFD::PixelFormat shdwGIPreviewRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_Color;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwGIPreviewRendererVk::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);


	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dVkUtil::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

void shdwGIPreviewRendererVk::InitStates()
{
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwGIPreviewRendererVk::CleanupStates()
{
	delete st_BlendNoAlpha;
	st_BlendNoAlpha = NULL;
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete st_MulNoBlend;
	st_MulNoBlend = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
