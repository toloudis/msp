/****************************************************************************\
**	shdwNormalRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_NORMALRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwNormalRendererVk.hpp"
#endif

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
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
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
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
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;
	g2dPFD l_normalPFD(g2dPFD::e_RGBA32f, 128);

	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwNormalRendererVk::shdwNormalRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwNormalRendererVk::~shdwNormalRendererVk()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwNormalRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwNormalRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("NormalRenderer");

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.

	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentNormalTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentNormalTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentNormalTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentNormalTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}

	// init and clear main render surface
	i_pWindow->MakeCurrent();
	i_pWindow->ClearDepthStencil();
#endif

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	// simple stupid draw loop

	// 1. render eye-space normals into buffer.
	shdwPassNormals normalPass(currentNormalTarget, &i_Camera);
	normalPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += normalPass.Render(i_fSimTime);

	shdwPassAlphaFill alphaFillPass(currentNormalTarget, true);
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
	if( !currentNormalTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
	{
		DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
	}
	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());
#else
	if (m_pFrameBuffer->HDRAATarget())
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
	}

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());
#endif

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	// reset d3d state
	g3dVkUtil::release_textures();

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	return l_nNumTrianglesRendered;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwNormalRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
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
g2dRenderTarget* shdwNormalRendererVk::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}


g2dPFD::PixelFormat shdwNormalRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwNormalRendererVk::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dVkUtil::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

void shdwNormalRendererVk::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwNormalRendererVk::CleanupStates()
{
	delete st_MulNoBlend;
	st_MulNoBlend = NULL;
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
}

}
