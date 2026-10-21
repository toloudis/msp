/****************************************************************************\
**	shdwReflectionsOnlyRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_REFLECTIONSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwReflectionsOnlyRendererVk.hpp"
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
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PASSBUFFERS_HPP
#include "Graphics/g3d/g3dPassBuffers.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
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
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
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
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
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
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 64);

	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_CopyRestore = NULL;
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwReflectionsOnlyRendererVk::shdwReflectionsOnlyRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwReflectionsOnlyRendererVk::~shdwReflectionsOnlyRendererVk()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwReflectionsOnlyRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwReflectionsOnlyRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("ReflectionsOnlyRenderer");


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
	matRenderTargetTexture* currentTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
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
	//g3dSceneRenderUtil::update_world_data( layers[0]->GetRootNode() );
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	/*const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	g3dRenderStateTraverser* stateTraverser = m_SceneDatabase.GetRenderStateCache();*/

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );


	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	//const g3dSceneNode* pNode = NULL;
	//g3dSceneNode::DrawStyle draw_style;
	//int i,n;

	// blend for ambient pass

	g3dBlendStateMgr::SetBlendState(st_Initial);

	// simple stupid draw loop
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(true);
	g3dVkUtil::AllowAdditiveChanges(false);

	int i,n;

	currentTarget->MakeCurrent();
	currentTarget->Clear(maFloatRGBA(0,0,0,0), true);

	g3dSingleLightRendering::SetDoIsolateReflections(true);

	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	// transparent nodes
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortVk* pTransNodes = m_SceneDatabase.GetTransparentNodes();
		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();
		for (it = tNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;

			bool doRender = true;

			g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
			if ( frag )
			{
				matMaterial * mat = frag->GetMaterial();
				if ( mat )
				{
					doRender = !mat->GetBelongsToLightShaft();
				}
			}

			if ( doRender )
			{
				l_nNumTrianglesRendered += RenderNode(NS, m_SceneDatabase.GetRenderStateCache());
			}
		}
	}

	g3dSingleLightRendering::SetDoIsolateReflections(false);

	// restore some state.
	g3dVkUtil::AllowAdditiveChanges(true);
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(false);
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	/*shdwPassAlphaFill alphaFillPass(currentTarget, (m_pFrameBuffer->HDRAATarget() != NULL), true);
	alphaFillPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += alphaFillPass.Render(i_fSimTime);*/

	camPassBuffersData passBuffersData;
	i_Camera.GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	i_Camera.GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_ReflBuffer && g3dPrefs::CurrentPrefs().m_bEnableReflection && g3dPassBuffers::GetDoingFileRefl() )
	{
		g3dVkUtil::BlendBuffers(currentTarget,passBuffersData.m_ReflBuffer,passBuffersData.m_ReflBlendOp, passBuffersData.m_ReflIntensity,
								  top, bottom, left, right);
	}

#ifdef NEW_AA_CODE
	if( !currentTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
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
void shdwReflectionsOnlyRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
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
void shdwReflectionsOnlyRendererVk::CopyToBackBuf(matRenderTargetTexture* pTex, bool bDoBlend, bool isRGB)
{
	g3dBlendStateMgr::SetBlendState(st_Initial);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	int technique = pEffect->FindTechnique(isRGB?"SimpleCopyLDR":"SimpleCopyLumLDR");

	m_pWindow->MakeCurrent();
	int w,h;
	m_pWindow->GetDimensions(w,h);

	vkdImage* aRes = pTex->GetImage();

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	unsigned int nTechPasses = (unsigned int)pEffect->GetPassCount(technique);
	for( unsigned int uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, &aRes);
		g3dVkUtil::DrawFullScreenQuad( w,h );
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_CopyRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwReflectionsOnlyRendererVk::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}


g2dPFD::PixelFormat shdwReflectionsOnlyRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

int shdwReflectionsOnlyRendererVk::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache)
{
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache,
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	// rendering with a null environment will isolate the reflections only, since
	// the environment pass includes reflections.
	g3dAmbientEnvState nullEnv;
	nTriangles += g3dSceneRenderUtil::DrawNodeEnvironment(i_Node.m_pNode, nullEnv);

	return nTriangles;
}

void shdwReflectionsOnlyRendererVk::InitStates()
{
	st_Initial = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_CopyRestore = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwReflectionsOnlyRendererVk::CleanupStates()
{
	delete st_Initial;
	st_Initial = NULL;
	delete st_CopyRestore;
	st_CopyRestore = NULL;
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
}

}
