/****************************************************************************\
**	shdwAOPreviewRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_AOPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwAOPreviewRendererVk.hpp"
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
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
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
#ifndef GVK_SHDW_PASSAOVOLUMES_HPP
#include "GraphicsVulkan/shdw/shdwPassAOVolumes.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif
#ifndef GVK_SHDW_PASSSSAO_HPP
#include "GraphicsVulkan/shdw/shdwPassSSAO.hpp"
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
	int l_nNumTriangles = 0;

	// the hdr pixel format we use.
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 16*4);

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Restore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOPreviewRendererVk::shdwAOPreviewRendererVk()
: m_Width(0), m_Height(0)
{
	m_OverscanSize = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwAOPreviewRendererVk::~shdwAOPreviewRendererVk()
{
	ReleaseResources();
}

void shdwAOPreviewRendererVk::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwAOPreviewRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("AOPreviewRenderer");

	ssaoParams p;
	i_Scene.GetSSAOSettings(p);
	m_OverscanSize = p.m_OverscanPixels;	//acquire border overscan for depth buffers before surface creation

	CreateSurfaces(i_pWindow);
	i_pWindow->MakeCurrent();

//	HRESULT op_result;

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in pick buffers
	g3dFogVk::EnableFog(false);

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
//	g3dVkUtil::SetOverrideMaterial(this->m_pAOMat);

	// Render the layers
	for (int i=0; i<layers.size(); i++)
	{
//		this->render_layer(*layers[i], i_Camera);
	}

	// Restore override material
//	g3dVkUtil::SetOverrideMaterial(NULL);

	g3dVkUtil::release_textures();

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
	return l_nNumTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwAOPreviewRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, m_OverscanSize);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

g2dPFD::PixelFormat shdwAOPreviewRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_Color;
}

void shdwAOPreviewRendererVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_Restore = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwAOPreviewRendererVk::CleanupStates()
{
	delete st_NoBlend;
	st_NoBlend = NULL;
	delete st_Restore;
	st_Restore = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
