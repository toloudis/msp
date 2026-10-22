/****************************************************************************\
**	g3dSceneRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif

#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
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
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

//#include "profile.h"



namespace
{
	// Stats
	int l_nNumTriangles = 0;
	SceneNodeVector l_NonWorldSpaceNodes;
	g3dTransparencySortVk l_transparencySort;

	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;

	//------------------------------------------------------------------------
	//	DrawNode
	//------------------------------------------------------------------------
	int DrawNode( g3dSceneNode* i_pNode )
	{
		// Check if node is renderable
		if( !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer())
		{
			return 0;
		}

		// resolve material/effect
		const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// is this the right technique choice here?
		pEffect->SetTechnique(matShaderEffect::e_Default);

		// set shader globals
		g3dVkUtil::SetupShaderGlobals(pEffect);

		// geometry data
		g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

		// shading data
		pEffect->SetupMaterial(pMaterial);
		//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);

		pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());

		// I think we actually always want to set up the ambient pass,
		// even if that just means we set it up with somethinng with
		// NULL pointers and booleans set to false
		static const g3dAmbientEnvState l_NoAmbientEnvState;
		pEffect->SetupAmbientPass(l_NoAmbientEnvState);
		
		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
		int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
		
		return nTriangles;
	}

	//------------------------------------------------------------------------
	//	world_space_render
	//------------------------------------------------------------------------
	void world_space_render( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos, 
							 g3dRenderStateTraverser& io_StateTraverser,
							 g3dSceneNode::DrawStyle i_DrawStyle,
							 bool i_bRenderLowRes )
	{
		// Check if node is renderable
		if( !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer())
		{
			return;
		}

		// See if we can cull from resolution
		if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
			return;

		// Check if node is culled
		ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
		if( clip_result == e_Reject )
		{
			return;
		}

		// Add the render state to the stack
		io_StateTraverser.AddRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment() );

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
		if( pFrag )
		{
			matMaterial* pMatOverride = i_pNode->GetMaterial();

			if (draw_style != g3dSceneNode::e_Solid)
			{
				// Force non-transparent if draw style is not solid
				g3dFogVk::EnableFog( false );
				g3dDrawStyleUtilVk::SetDrawStyle(draw_style);
				l_nNumTriangles += DrawNode( i_pNode );
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
				l_nNumTriangles += DrawNode( i_pNode );
			}
		}

		// Render the children
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		int nkids = children.size();
		for (int i=0; i<nkids; i++)
		{
			world_space_render(children[i], 
							   i_CameraPos, 
							   io_StateTraverser, 
							   draw_style,
							   i_bRenderLowRes);
		}

		// Remove the render state
		io_StateTraverser.SubtractRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void set_additive_mode()
	{
		//if( !l_AdditiveMode )
		{
// This chunk of blend state code haven't been tested yet
			g3dBlendStateMgr::SetBlendState(st_AddBlend);
			//l_AdditiveMode = true;
		}
	}

	void set_multiplicative_mode()
	{
		//if( l_AdditiveMode )
		{
// This chunk of blend state code haven't been tested yet
			g3dBlendStateMgr::SetBlendState(st_MulBlend);
			//l_AdditiveMode = false;
		}
	}

	// set up attributes of a layer
	void setup_layer(g3dLayer& i_Layer)
	{
		//	We don't need alpha blending in the static models.
		//	It will be turned on later if we have alpha stuff
		//	in the dynamic models.

		g3dDepthStencilStateMgr::SetDepthStencilState( (i_Layer.GetSortMethod() == g3dLayer::e_ZSort) ? ds_Disable_NS : ds_Test_Write_LessE_NS );
		// enable or disable fog
		g3dFogVk::EnableFog(i_Layer.GetFogEnabled());

		// set blending method
		//if( i_SetBlend )
		{
			if( i_Layer.GetBlendMethod() == g3dLayer::e_Multiplicative )
				set_multiplicative_mode();
			else if( i_Layer.GetBlendMethod() == g3dLayer::e_Additive )
				set_additive_mode();
		}
	}

	// render a layer by setting up its attributes and then rendering
	// its scene graph
	void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera)
	{
		setup_layer(i_Layer);

		g3dSceneRenderUtil::enable_lights();

		/*if (i_Layer.GetShadowRender())
			shadow_render_graph(i_Layer.GetRootNode());
		else */
		if (i_Layer.GetModelSpace() == g3dLayer::e_World)
		{
			//g3dSceneRenderUtil::update_world_data( i_Layer.GetRootNode() );

			g3dRenderStateTraverser state_traverser;
			g3dSceneNode::DrawStyle draw_style = g3dPrefs::CurrentPrefs().m_DrawStyle; //g3dSceneNode::e_Solid;
			bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
			world_space_render(i_Layer.GetRootNode(), 
							   i_Camera.GetPosition(), 
							   state_traverser,
							   draw_style,
							   bLowRes);
			g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

			// Render the transparent nodes, in the order they were found (as
			// DX11's, which never sorts them); then forget them, as DX11 does
			l_transparencySort.RenderTransparentNodes(state_traverser );
			l_transparencySort.ClearTransparentNodes();
		}
		else
		{
			// sorted render
			// (should this always sort?
			//   maybe there need to be variations on i_Layer.GetSortMethod())
			g3dSceneRenderUtil::gather_fragment_nodes( i_Layer.GetRootNode(), l_NonWorldSpaceNodes );
			std::sort( l_NonWorldSpaceNodes.begin(), l_NonWorldSpaceNodes.end(), g3dSceneRenderUtil::screen_space_sort );
			envSTLHelpers::ForAll( l_NonWorldSpaceNodes, g3dSceneRenderUtil::nonworld_space_render );
			l_NonWorldSpaceNodes.clear();
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dSceneRendererVk::g3dSceneRendererVk()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dSceneRendererVk::~g3dSceneRendererVk()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dSceneRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("SceneRenderer");

	// setup fog settings
	fogParams fog_params;
	i_Scene.GetFogSettings(fog_params);
	g3dFogVk::SetFog(fog_params.m_nMode, fog_params.m_Color, fog_params.m_fStart, fog_params.m_fEnd, fog_params.m_fDensity);

	// Check variable each frame so that it can be switched by the app
	//l_bDoShadows = g3dSingleLightRendering::GetDoSingleLightRendering();

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// Initialize the Z buffer
	i_pWindow->Clear( maFloatRGBA() );
/*
	HRESULT op_result;
	if( g2dVkGlobal::HasStencil() )
	{
//		op_result = g2dVkGlobal::g_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
		// temp: does it work to clear here?
	}
	else
	{
		op_result = g2dVkGlobal::g_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	}
	CHECK_D3D_ERROR(op_result, "Couldn't clear zbuffer");
*/

	// Initialize the triangle count
	l_nNumTriangles = 0;

	// Begin Scene
//	op_result = g2dVkGlobal::g_pDevice->BeginScene();
//	CHECK_D3D_ERROR(op_result, "Couldn't begin scene");

	// Render the layers
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	for (int i=0; i<layers.size(); i++)
	{
		render_layer(*layers[i], i_Camera);
	}
	// Render additional layers from viewer
	for (int i=0; i<i_ViewerLayers.size(); i++)
	{
		render_layer(*i_ViewerLayers[i], i_Camera);
	}

	// End Scene (Flip)
//	op_result = g2dVkGlobal::g_pDevice->EndScene();
//	if( op_result != D3D_OK )
//	{
//		g2dVkGlobal::PrintDXError(op_result);
//	}

	// reset d3d state
	g3dVkUtil::release_textures();

	//Vk commented out
//	g2dVkGlobal::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dVkGlobal::g_pDevice->SetPixelShader( NULL );

{//PROFILE("debug");
	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	g2dScreen::SetDebugInfo(2, num);
}
	return l_nNumTriangles;
}

void g3dSceneRendererVk::InitStates()
{
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState ( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dSceneRendererVk::CleanupStates()
{
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
