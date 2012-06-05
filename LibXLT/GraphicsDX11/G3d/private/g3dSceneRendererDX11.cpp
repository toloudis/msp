/****************************************************************************\
**	g3dSceneRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "Graphics/G2d/g2dRenderTarget.hpp"

//#include "profile.h"


//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

namespace
{
	// Stats
	int l_nNumTriangles = 0;
	SceneNodeVector l_NonWorldSpaceNodes;
	g3dTransparencySortDX11 l_transparencySort;

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
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// is this the right technique choice here?
		pEffect->SetTechnique(matShaderEffect::e_Default);

		// set shader globals
		g3dDX11Util::SetupShaderGlobals(pEffect);

		// geometry data
		g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

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
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
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
				g3dFogDX11::EnableFog( false );
				g3dDrawStyleUtilDX11::SetDrawStyle(draw_style);
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
				g3dFogDX11::EnableFog( i_pNode->GetFogged() );
				g3dDrawStyleUtilDX11::SetDrawStyle(draw_style);
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
		g3dFogDX11::EnableFog(i_Layer.GetFogEnabled());

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
			g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

			// Render the transparent nodes
			l_transparencySort.RenderTransparentNodes(state_traverser );
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
g3dSceneRendererDX11::g3dSceneRendererDX11()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dSceneRendererDX11::~g3dSceneRendererDX11()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dSceneRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dSceneRendererDX11::Render" );

	// setup fog settings
	fogParams fog_params;
	i_Scene.GetFogSettings(fog_params);
	g3dFogDX11::SetFog(fog_params.m_nMode, fog_params.m_Color, fog_params.m_fStart, fog_params.m_fEnd, fog_params.m_fDensity);

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
	if( g2dDX11Global::g_bHasStencil )
	{
//		op_result = g2dDX11Global::g_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
		// temp: does it work to clear here?
	}
	else
	{
		op_result = g2dDX11Global::g_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	}
	CHECK_D3D_ERROR(op_result, "Couldn't clear zbuffer");
*/

	// Initialize the triangle count
	l_nNumTriangles = 0;

	// Begin Scene
//	op_result = g2dDX11Global::g_pDevice->BeginScene();
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
//	op_result = g2dDX11Global::g_pDevice->EndScene();
//	if( op_result != D3D_OK )
//	{
//		g2dDX11Global::PrintDXError(op_result);
//	}

	// reset d3d state
	g3dDX11Util::release_textures();

	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

{//PROFILE("debug");
	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	g2dScreen::SetDebugInfo(2, num);
}
	D3DPERF_EndEvent();
	return l_nNumTriangles;
}

void g3dSceneRendererDX11::InitStates()
{
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState ( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dSceneRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}