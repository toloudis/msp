/****************************************************************************\
**	shdwUVRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_UVRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwUVRendererVk.hpp"
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
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PRIMITIVEFRAGMENTUTIL_HPP
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
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
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
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
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{



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
shdwUVRendererVk::shdwUVRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;


	// lazy init so that this material can be reused across instantiations.
	if (!l_UVMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(std::string("Special/Solid.fx"), matShaderMgr::GetSpecialEffect("Solid.fx"));
		l_UVMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwUVRendererVk::~shdwUVRendererVk()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwUVRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwUVRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("UVRenderer");

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

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

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
		g3dDrawStyleUtilVk::SetDrawStyle(g3dSceneNode::e_Wireframe);
		DrawNode(layers[0]->GetRootNode());
		g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	}


	// restore state (could go in DrawNode)
	g3dSingleLightRendering::SetDoBaking(false);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

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
void shdwUVRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// test conditions for recreating surfaces:
	if ((w != m_Width) ||
		(h != m_Height))
	{
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

void shdwUVRendererVk::DrawNode(const g3dSceneNode* i_pNode)
{
	if (i_pNode == NULL)
		return;
	if (i_pNode->GetFragment() == NULL)
		return;

	// resolve material/effect
	const matMaterial* pMaterial = &l_UVMat;//g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

void shdwUVRendererVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Write_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwUVRendererVk::CleanupStates()
{
	delete st_NoBlend;
	st_NoBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Write_NS;
	ds_Write_NS = NULL;
}

}
