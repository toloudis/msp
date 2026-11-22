/****************************************************************************\
**	shdwShadowLayerRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_SHADOWLAYERRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowLayerRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSAMBIENT_HPP
#include "GraphicsVulkan/shdw/shdwPassAmbient.hpp"
#endif
#ifndef GVK_SHDW_PASSENVIRONMENT_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvironment.hpp"
#endif
#ifndef GVK_SHDW_PASSGLOW_HPP
#include "GraphicsVulkan/shdw/shdwPassGlow.hpp"
#endif
#ifndef GVK_SHDW_PASSLIT_HPP
#include "GraphicsVulkan/shdw/shdwPassLit.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef GVK_SHDW_PASSOUTLINE_HPP
#include "GraphicsVulkan/shdw/shdwPassOutline.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
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
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	// Flag for trivial rejection in shadow code
	bool l_bHaveShadowCastingLights = false;

	g3dTransparencySortVk l_transparencySort;

	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;

	//------------------------------------------------------------------------
	//	world_space_render
	//------------------------------------------------------------------------
	int world_space_render( g3dSceneNode* i_pNode,
							 const maPoint3d &i_CameraPos,
							 g3dRenderStateTraverser &io_StateTraverser,
							 g3dSceneNode::DrawStyle i_DrawStyle,
							 bool i_bRenderLowRes,
							 bool i_bDoClip)
	{
		int nTriangles = 0;
		// Check if node is renderable
		if( !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer())
		{
			return 0;
		}

		// See if we can cull from resolution
		if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
			return 0;

		if (i_bDoClip)
		{
			// Check if node is culled
			ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
			if( clip_result == e_Reject )
			{
				return 0;
			}
			else if (clip_result == e_NoClip)
			{
				// Box is completely in view, no need to check children
				// because they will be in view also.
				i_bDoClip = false;
			}
		}

		// Add the render state to the stack:
		// This allows us to remember lighting state for nodes that need to be
		// rendered in subsequent passes.
		io_StateTraverser.AddRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
			g3dSingleLightRendering::GetDoSingleLightRendering() );

		// Combine draw style
		g3dSceneNode::DrawStyle draw_style = i_DrawStyle;
		if (i_pNode->GetDrawStyle() != g3dSceneNode::e_Inherit)
		{
			draw_style = i_pNode->GetDrawStyle();
		}
		if( g3dPrefs::CurrentPrefs().m_bRenderWireframe )
		{
			draw_style = g3dSceneNode::e_LitWireframe;
		}

		const g3dFragment* pFrag = i_pNode->GetFragment();

		// Check if it has geometry
		if( pFrag && !pFrag->IsShadowHull())
		{
			matMaterial* pMatOverride = i_pNode->GetMaterial();

			if (draw_style != g3dSceneNode::e_Solid)
			{
				// Force non-transparent if draw style is not solid
				g3dFogVk::EnableFog( false );
				g3dDrawStyleUtilVk::SetDrawStyle(draw_style);
				nTriangles += g3dSceneRenderUtil::nonworld_space_render( i_pNode );
			}
			// If it is transparent then render it after the opaque
			else if( ( pMatOverride && pMatOverride->GetHasTransparency() ) ||
				  pFrag->GetMaterial()->GetHasTransparency() )
			{
				l_transparencySort.AddTransparentNode( i_pNode,
															i_CameraPos,
															io_StateTraverser.GetCurrentStateCache() );
			}
			// Else render the node
			else
			{
				g3dFogVk::EnableFog( i_pNode->GetFogged() );
				g3dDrawStyleUtilVk::SetDrawStyle(draw_style);

				nTriangles += g3dSceneRenderUtil::nonworld_space_render( i_pNode );
			}
		}

		// Render the children
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		int nkids = children.size();
		for (int i=0; i<nkids; i++)
			nTriangles += world_space_render(children[i],
							   i_CameraPos,
							   io_StateTraverser,
							   draw_style,
							   i_bRenderLowRes,
							   i_bDoClip);

		// Remove the render state
		io_StateTraverser.SubtractRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
			g3dSingleLightRendering::GetDoSingleLightRendering() );

		return nTriangles;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwShadowLayerRendererVk::shdwShadowLayerRendererVk()
{
	m_width = 0;
	m_height = 0;
	m_renderTargetTex = NULL;
	m_blurredTex = NULL;
	m_glowTex = NULL;
	m_intermediateTex = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwShadowLayerRendererVk::~shdwShadowLayerRendererVk()
{
	ReleaseResources();
}

void shdwShadowLayerRendererVk::ReleaseResources()
{
	matTextureMgr::ReleaseTexture(m_renderTargetTex);
	m_renderTargetTex = NULL;
	matTextureMgr::ReleaseTexture(m_blurredTex);
	m_blurredTex = NULL;
	matTextureMgr::ReleaseTexture(m_intermediateTex);
	m_intermediateTex = NULL;
	matTextureMgr::ReleaseTexture(m_glowTex);
	m_glowTex = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwShadowLayerRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("ShadowLayerRenderer");

	// Initialize the triangle count
	int nTriangles = 0;

	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);
	i_pWindow->MakeCurrent();

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.

	// draw full scene to render target
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	// setup fog settings
	fogParams fog_params;
	i_Scene.GetFogSettings(fog_params);
	g3dFogVk::SetFog(fog_params.m_nMode, fog_params.m_Color, fog_params.m_fStart, fog_params.m_fEnd, fog_params.m_fDensity);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// Initialize the Z buffer
	i_pWindow->ClearDepthStencil();

	// Render the layers
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	m_rWorldLayer.Set(layers[0], &i_Camera, i_pWindow);
	m_rWorldLayer.PerFrameInit(i_fSimTime);

//	if (layers.size()>3)
//		m_rSkyLayer.Set(layers[3], &i_Camera, i_pWindow);
//	else
		m_rSkyLayer.Set(NULL, &i_Camera, i_pWindow);
	m_rSkyLayer.PerFrameInit(i_fSimTime);

	i_pWindow->MakeCurrent();

	nTriangles += m_rWorldLayer.Render(i_fSimTime);
	nTriangles += m_rSkyLayer.Render(i_fSimTime);

	// reset d3d state
	g3dVkUtil::release_textures();
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	if (g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		g3dVkUtil::CopyBackBufferToRenderTargetTex(m_renderTargetTex);
		g3dRenderAlpha mattePass(m_renderTargetTex, i_pWindow);
		nTriangles += mattePass.Render(i_fSimTime);
	}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	camDOFData dof;
	i_Camera.GetDOFParams(dof);
	// set into global 3d rendering state
	g3dSceneGlobal::g_DOFParams.m_FarBlurDist = dof.m_FarBlurDist;
	g3dSceneGlobal::g_DOFParams.m_FarFocalDist = dof.m_FarFocalDist;
	g3dSceneGlobal::g_DOFParams.m_NearFocalDist = dof.m_NearFocalDist;
	g3dSceneGlobal::g_DOFParams.m_NearBlurDist = dof.m_NearBlurDist;
	g3dSceneGlobal::g_DOFParams.m_MaxFarBlur = dof.m_MaxFarBlur;
	g3dSceneGlobal::g_DOFParams.m_MaxCoC = dof.m_MaxCoC;

//	(DX11's DOF branch, commented out there as unreachable - "There is no
//	circumstance that this chunk of code could be executed for now" - is
//	left out)

	// deposit overlay layers on top
	if( !g3dSingleLightRendering::GetDoReflectionGen() )	//disable for reflections
	{
		for (int i = 1; i < layers.size(); i++)
		{
			if (layers[i] && layers[i]->GetModelSpace() == g3dLayer::e_Camera)
			{
				m_rCameraLayer.Set(layers[i], &i_Camera, i_pWindow);
				m_rCameraLayer.PerFrameInit(i_fSimTime);
				nTriangles += m_rCameraLayer.Render(i_fSimTime);
			}
			else if (layers[i] && layers[i]->GetModelSpace() == g3dLayer::e_Screen)
			{
				m_rScreenLayer.Set(layers[i], &i_Camera, i_pWindow);
				m_rScreenLayer.PerFrameInit(i_fSimTime);
				nTriangles += m_rScreenLayer.Render(i_fSimTime);
			}
		}
		// Also add in the extra layers that are specific to this viewer
		for (int i = 0; i < i_ViewerLayers.size(); i++)
		{
			if (i_ViewerLayers[i] && i_ViewerLayers[i]->GetModelSpace() == g3dLayer::e_Camera)
			{
				m_rCameraLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
				m_rCameraLayer.PerFrameInit(i_fSimTime);
				nTriangles += m_rCameraLayer.Render(i_fSimTime);
			}
			else if (i_ViewerLayers[i] && i_ViewerLayers[i]->GetModelSpace() == g3dLayer::e_Screen)
			{
				m_rScreenLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
				m_rScreenLayer.PerFrameInit(i_fSimTime);
				nTriangles += m_rScreenLayer.Render(i_fSimTime);
			}
		}
	}

	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////

	m_rWorldLayer.PerFrameCleanup();
	m_rSkyLayer.PerFrameCleanup();
	m_rCameraLayer.PerFrameCleanup();
	m_rScreenLayer.PerFrameCleanup();
    m_rCameraLayer.Set(NULL, &i_Camera, i_pWindow);
	m_rScreenLayer.Set(NULL, &i_Camera, i_pWindow);


	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", nTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	return nTriangles;
}




g3dRenderWorldSpaceObjects::g3dRenderWorldSpaceObjects()
:	g3dRenderLayer(),
	m_pGlowTarget(NULL),
	m_pTempTarget(NULL)
{
	m_sceneInfo = new shdwPassTraversal();
}
g3dRenderWorldSpaceObjects::~g3dRenderWorldSpaceObjects()
{
	delete m_sceneInfo;
}
void g3dRenderWorldSpaceObjects::PerFrameInit(float i_time)
{
	if (m_pLayer == NULL)
		return;
	DBG_ASSERT(m_pLayer->GetModelSpace() == g3dLayer::e_World, "Wrong coordinate space for camera space layer" );

	g3dSceneRenderUtil::enable_lights();
	//g3dSceneRenderUtil::update_world_data( m_pLayer->GetRootNode() );
	m_sceneInfo->TraverseLayer(i_time, m_pLayer, m_pCamera);
}
void g3dRenderWorldSpaceObjects::PerFrameCleanup()
{
	m_sceneInfo->Clear();
}
int g3dRenderWorldSpaceObjects::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderWorldSpaceObjects");
	m_stats.Reset();

	if (m_pLayer == NULL)
		return 0;

	// Z Buffering

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// Fog
	g3dFogVk::EnableFog(m_pLayer->GetFogEnabled());

	// view / projection transformation
	DBG_ASSERT(m_pLayer->GetModelSpace() == g3dLayer::e_World, "Wrong coordinate space for world space layer" );

	// fill Z with opaques.
	// this allows for all passes to do correct blending.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(m_sceneInfo);
	m_stats.m_nTriangles  += depthPrePass.Render(i_time);

	// blend for ambient pass
	g3dVkUtil::AllowAdditiveChanges(true);
	g3dBlendStateMgr::SetBlendState(st_Blend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );
//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	if (!g3dSingleLightRendering::GetDoSingleLightRendering())
	{
		// set blend mode
		g3dVkUtil::AllowAdditiveChanges(false);

		shdwPassAmbient ambientPass;
		ambientPass.SetSceneInfo(m_sceneInfo);
		m_stats.m_nTriangles += ambientPass.Render(i_time);

		g3dVkUtil::AllowAdditiveChanges(true);
	}
	else
	{
		if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
		{
			// set blend mode
			g3dVkUtil::AllowAdditiveChanges(false);

			shdwPassEnvironment envPass;
			envPass.SetSceneInfo(m_sceneInfo);
			m_stats.m_nTriangles += envPass.Render(i_time);

			g3dVkUtil::AllowAdditiveChanges(true);
		}

		if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
		{
			// set blend mode
			g3dVkUtil::AllowAdditiveChanges(false);

			shdwPassLit litPass;
			litPass.SetSceneInfo(m_sceneInfo);
			m_stats.m_nTriangles += litPass.Render(i_time);

			g3dVkUtil::AllowAdditiveChanges(true);
		}
	}

	//NULL safe for depthaux since only render() is called
	shdwPassTransparent transparentPass(m_pLayer, m_pCamera, m_pTempTarget, m_pTempScratch0, NULL, m_pTempScratch1, m_pGlowTarget, m_pRenderTarget);
	transparentPass.SetSceneInfo(m_sceneInfo);
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		m_stats.m_nTriangles += transparentPass.Render(i_time);
	}
	// glow will not contribute to matte pass
	if (!g3dSingleLightRendering::GetDoDOFPrepPass() &&
		g3dPrefs::CurrentPrefs().m_bEnableGlow &&
		!g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		// Assuming m_pRenderTarget is always a non MSAA target
//	FIXME TODO how to handle glows in dynamic reflction maps.
		//DBG_TRACE( "Need to implement!");
//		shdwPassGlow glowPass(m_pLayer, m_pCamera, m_pRenderTarget, m_pGlowTarget );
//		glowPass.SetSceneInfo(m_sceneInfo);
//		m_stats.m_nTriangles += glowPass.Render(i_time);
	}

	// outline will not contribute to matte pass
	if (!g3dSingleLightRendering::GetDoDOFPrepPass() &&
		g3dPrefs::CurrentPrefs().m_bEnableOutline &&
		!g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		//reuse scratch buffers from glow
		shdwPassOutline outlinePass(m_pTempScratch0, m_pTempScratch1, m_pRenderTarget, m_sceneInfo );
		m_stats.m_nTriangles += outlinePass.Render(i_time);
	}
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent && g3dPrefs::CurrentPrefs().m_bEnableDeferredTransparency)
	{
		// draw deferred transparent after glow happens, for correct z buffering.
		m_stats.m_nTriangles += transparentPass.RenderDeferred(i_time);
	}
	return m_stats.m_nTriangles;
}
g3dRenderSkyBox::g3dRenderSkyBox()
:	g3dRenderLayer()
{
}
g3dRenderSkyBox::~g3dRenderSkyBox()
{
}
int g3dRenderSkyBox::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderSkyBox");
	// This chunk of code is never executed
	m_stats.Reset();

	if (m_pLayer == NULL)
		return 0;

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// Fog
	g3dFogVk::EnableFog(m_pLayer->GetFogEnabled());

	// Blending
	DBG_ASSERT(m_pLayer->GetBlendMethod() == g3dLayer::e_Additive, "Wrong blend mode for camera space layer" );

	// This chunk of blendstate has never been tested
	g3dBlendStateMgr::SetBlendState(st_AddBlend);

	// view / projection transformation
	DBG_ASSERT(m_pLayer->GetModelSpace() == g3dLayer::e_World, "Wrong coordinate space for camera space layer" );

	g3dSceneRenderUtil::enable_lights();
	// main world layer
	// other world space layer
	g3dRenderStateTraverser state_traverser;
	//g3dSceneRenderUtil::update_world_data( m_pLayer->GetRootNode() );
	g3dSceneNode::DrawStyle draw_style = g3dPrefs::CurrentPrefs().m_DrawStyle; //g3dSceneNode::e_Solid;
	bool bRenderLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
	bool bDoClip = true;
	m_stats.m_nTriangles += world_space_render(m_pLayer->GetRootNode(),
						m_pCamera->GetPosition(),
						state_traverser,
						draw_style,
						bRenderLowRes,
						bDoClip);
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();
	// Render the transparent nodes
	m_stats.m_nTriangles += l_transparencySort.RenderTransparentNodes(state_traverser );
	return m_stats.m_nTriangles;
}

void shdwShadowLayerRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// test conditions for recreating surfaces:
	if ((w != m_width) ||
		(h != m_height) ||
		(m_renderTargetTex == NULL) ||
		(m_intermediateTex == NULL) ||
		(m_blurredTex == NULL) ||
		(m_glowTex == NULL))
	{
		// special knowledge that this render target is a window...
		g2dPFD pfd;
		g2dWindow* pWnd = (g2dWindow*)dynamic_cast<g2dWindow*>(i_pWindow);
		if (pWnd != NULL)
			pfd = pWnd->GetBackBufferPixelFormat();
		else
			pfd = i_pWindow->GetPixelFormat();

		matTextureMgr::ReleaseTexture(m_renderTargetTex);
		m_renderTargetTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(w, h,
			false, &pfd, false, true, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_intermediateTex);
		m_intermediateTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(w, h,
			false, &pfd, false, true, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_blurredTex);
		m_blurredTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(w, h,
			false, &pfd, false, true, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_glowTex);
		m_glowTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(w, h,
			false, &pfd, false, true, matTextureMgr::e_Framebuffer));

		m_rWorldLayer.SetGlowTargets(m_glowTex, m_renderTargetTex, m_intermediateTex, m_blurredTex);

		m_width = w;
		m_height = h;
	}
}
void shdwShadowLayerRendererVk::DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest)
{
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
	int technique = pEffect->FindTechnique("DrawAlpha");

	i_dest->MakeCurrent();
	int w,h;
	i_dest->GetDimensions(w,h);

	vkdImage* aRes[1] = { i_src->GetImage() };
	vkdGlobal::Context().SetShaderResources(0, 1, aRes);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for( uint32_t uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);

		g3dVkUtil::DrawFullScreenQuad( w,h );
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

void shdwShadowLayerRendererVk::InitStates()
{
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwShadowLayerRendererVk::CleanupStates()
{
	delete st_Blend;
	st_Blend = NULL;
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_LessE_NS;
	ds_Test_LessE_NS = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
}

}
