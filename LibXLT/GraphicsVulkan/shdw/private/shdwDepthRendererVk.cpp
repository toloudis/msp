/****************************************************************************\
**	shdwDepthRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_DEPTHRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwDepthRendererVk.hpp"
#endif

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
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
#include "Graphics/G3d/g3dPrefs.hpp"
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
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
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

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulNoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwDepthRendererVk::shdwDepthRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	m_pAlphaMaskMat = new matMaterial("DepthRender.fx");
	m_pMaskAlphaData = dynamic_cast<effMaskAlphaData*>(m_pAlphaMaskMat->GetEffectData());
	DBG_ASSERT(m_pMaskAlphaData, "Could not load shader needed for Depth rendering");

	m_pHairMat = new matMaterial("HairDefault.fx");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwDepthRendererVk::~shdwDepthRendererVk()
{
	ReleaseResources();

	delete m_pHairMat;
	delete m_pAlphaMaskMat;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwDepthRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwDepthRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("DepthRenderer");

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
//	m_HDRRenderTargetTex->MakeCurrent();
//	m_HDRRenderTargetTex->Clear(maFloatRGBA(0,0,0,0));
	i_pWindow->ClearDepthStencil();

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	//g3dSceneRenderUtil::update_world_data( layers[0]->GetRootNode() );
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	g3dRenderStateTraverser* stateTraverser = m_SceneDatabase.GetRenderStateCache();

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	const g3dSceneNode* pNode = NULL;
	g3dSceneNode::DrawStyle draw_style;

	int i,n;

	// blend for ambient pass
	g3dBlendStateMgr::SetBlendState(st_MulNoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// simple stupid draw loop

	// 1. render eye-space depths into buffer.
	m_pFrameBuffer->DepthBuffer()->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
	shdwPassDepth depthPass(m_pFrameBuffer->DepthBuffer(), &i_Camera);
	depthPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPass.Render(i_fSimTime);

	// 2. COPY IT INTO THE BACKBUFFER, map to grayscale
	i_pWindow->MakeCurrent();
//	CopyToBackBuf(m_pDepthBuffer, i_Camera);

	// Force our material for all renderers
	g3dVkUtil::SetOverrideMaterial(m_pAlphaMaskMat);

	// Render the layers

	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = nonShadowNodes[i].m_pNode;
		draw_style = nonShadowNodes[i].m_drawStyle;

		g3dDrawStyleUtilVk::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = shadowNodes[i].m_pNode;
		draw_style = shadowNodes[i].m_drawStyle;

		g3dDrawStyleUtilVk::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		pNode = nonSolidNodes[i].m_pNode;
		draw_style = nonSolidNodes[i].m_drawStyle;

		g3dDrawStyleUtilVk::SetDrawStyle(draw_style);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		DrawNode(pNode);
	}
	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_SceneDatabase.GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			DrawHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	if( g3dPrefs::CurrentPrefs().m_bEnableTransparent )
	{
		g3dTransparencySortVk* pTranspNodes = m_SceneDatabase.GetTransparentNodes();
		const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();
		for (it = tNodes.begin(); it != end; ++it)
		{
			DrawNode(it->m_pSceneNode);
		}
	}

	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();
/*
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		// draw transparent
		shdwPassTransparent transparentPass(layers[0], &i_Camera, NULL,
			NULL, m_HDRRenderTargetTex);
		transparentPass.SetSceneInfo(&m_SceneDatabase);
		l_nNumTrianglesRendered += transparentPass.Render(i_fSimTime);
	}
*/

	// Restore override material
	g3dVkUtil::SetOverrideMaterial(NULL);

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

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

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
void shdwDepthRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

void shdwDepthRendererVk::DrawNode(const g3dSceneNode* i_pNode)
{

	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	const matMaterial* pOrigMaterial = i_pNode->GetFragment()->GetMaterial();
	g3dVkUtil::SetupOpacity(i_pEffect, *pOrigMaterial, "hasTransparencyMap", "TransparencyMap", "g_Transparency");

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
void shdwDepthRendererVk::DrawHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	// resolve material/effect
	const matMaterial* pMaterial = m_pHairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetupMaterial( pMaterial );

#ifdef USE_NORMALIZED_DEPTHS
	pEffect->SetTechnique("ViewSpaceDepthN");
#else
	pEffect->SetTechnique("ViewSpaceDepth");
#endif

	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

}


//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwDepthRendererVk::GetRawBuffer()
{
	return m_pFrameBuffer->DepthBuffer();
}

void shdwDepthRendererVk::GetTightestNearFar(const camCamera& i_Camera, float& o_CamNear, float& o_CamFar)
{
	o_CamNear = i_Camera.GetNearClip();
	o_CamFar = i_Camera.GetFarClip();

	// get scene bounds.
	maAxisBox b;
	b.Union(m_SceneDatabase.m_NonShadowNodesBounds);
	b.Union(m_SceneDatabase.m_NonSolidNodesBounds);
	b.Union(m_SceneDatabase.m_ShadowNodesBounds);

	// sort bounding box pts by dist to cam.
	// assumes cam pos and dir are in same space as bounds.

	maPoint3d camPos = i_Camera.GetPosition();
	maVector3d camDir = i_Camera.GetDirection();
	maPlane camPlane(camDir, camPos);

	maPoint3d bounds[8];
	b.GetBoxPoints(bounds);
	float distances[8];
	// negative distances are behind camera.
	bool any_neg = false;
	float minDist = o_CamFar, maxDist = o_CamNear;
	for (int i = 0; i < 8; i ++)
	{
		distances[i] = camPlane.TestPoint(bounds[i]);
		if (distances[i] < minDist)
			minDist = distances[i];
		if (distances[i] > maxDist)
			maxDist = distances[i];
	}

	if (minDist > o_CamNear)
		o_CamNear = minDist;
	if (maxDist < o_CamFar)
		o_CamFar = maxDist;
}

g2dPFD::PixelFormat shdwDepthRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_Float32;
}

void shdwDepthRendererVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulNoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwDepthRendererVk::CleanupStates()
{
	delete st_NoBlend;
	st_NoBlend = NULL;
	delete st_MulNoBlend;
	st_MulNoBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
