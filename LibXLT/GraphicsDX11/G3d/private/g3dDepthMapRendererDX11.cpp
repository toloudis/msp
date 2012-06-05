/****************************************************************************\
**	g3dDepthMapRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDepthMapRendererDX11.hpp"

//#include "Core/fs/fsLocator.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/Eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/hair/hairModelFrag.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"
//#include "Graphics/Eff/effStrandHairData.hpp"

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

	// Constants used for Depth Bias 
	const float c_fSlopeScaleDepthBias	= 1.0f;
	const float c_fDepthBias			= 0.0005f;
	const float c_fDefaultDepthBias		= 0.0f;
	inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dDepthMapRendererDX11::g3dDepthMapRendererDX11()
{
	m_pAlphaMaskMat = new matMaterial("DepthMap.fx");
	m_pMaskAlphaData = dynamic_cast<effMaskAlphaData*>(m_pAlphaMaskMat->GetEffectData());
	DBG_ASSERT(m_pMaskAlphaData, "Could not load shader needed for DepthMap rendering");

	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dDepthMapRendererDX11::~g3dDepthMapRendererDX11()
{
	delete m_pAlphaMaskMat;
}

void g3dDepthMapRendererDX11::ReleaseResources()
{
}

void WriteToFile(g2dRenderTarget* i_pWindow, fsLocator loc)
{
	matRenderTargetTexture* pMap = dynamic_cast<matRenderTargetTexture*>(i_pWindow);
	matTextureMgr::SaveTextureToFile(pMap, loc);
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dDepthMapRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDepthMapRendererDX11::Render" );

//	HRESULT op_result;

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in depth maps
	g3dFogDX11::EnableFog(false);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// Initialize the Z buffer
	i_pWindow->ClearDepthStencil();
//	op_result = g2dDX11Global::g_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
//	CHECK_D3D_ERROR(op_result, "Couldn't clear zbuffer");

	// Initialize the triangle count
	l_nNumTriangles = 0;

	// Force our material for all renderers
	g3dDX11Util::SetOverrideMaterial(this->m_pAlphaMaskMat);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dDX11Util::AllowAdditiveChanges(false);

	// Begin Scene
//	op_result = g2dDX11Global::g_pDevice->BeginScene();
//	CHECK_D3D_ERROR(op_result, "Couldn't begin scene");

	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	int w, h;
	i_pWindow->GetDimensions(w, h);
	D3D11_VIEWPORT vp;
	vp.Width = (float)w;
	vp.Height = (float)h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

	for (int i=0; i<layers.size(); i++)
	{
		this->render_layer(*layers[i], i_Camera);
	}
	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		this->render_layer(*i_ViewerLayers[i], i_Camera);
	}

	// Restore the Old viewport
	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );

	// End Scene (Flip)
//	op_result = g2dDX11Global::g_pDevice->EndScene();
//	if( op_result != D3D_OK )
//	{
//		g2dDX11Global::PrintDXError(op_result);
//	}

	// Restore override material
	g3dDX11Util::SetOverrideMaterial(NULL);

	g3dDX11Util::AllowAdditiveChanges(true);
// Restoring blend state, could be dropped later	
	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dDX11Util::release_textures();

	//dx11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	{	// PROFILE("debug");
		// Display debug information for number of triangles
		// char num[64];
		// sprintf(num, "tris: %d", l_nNumTriangles);
		// g2dScreen::SetDebugInfo(2, num);
	}

	D3DPERF_EndEvent();
	//WriteToFile( i_pWindow , fsLocator( itString("C:\\Projects\\ShadowMap.dds") ) );

	return l_nNumTriangles;
}
#if 0
	//--------------------------------------------------------------------
	// collect relevant occlusion nodes into lists
	//--------------------------------------------------------------------
	class ShadowNodeProcessor : public g3dSceneNodeProcessor
	{
	public:
		ShadowNodeProcessor(){m_bDoClip.push(true);}
		~ShadowNodeProcessor(){Clear();}

		// NOTE THESE ARE REFERENCES TO THE OWNER'S DATA PASSED TO CONSTRUCTOR
		nodeList m_shadowNodes;
		std::stack<bool> m_bDoClip;
		bool m_bDoClip;

		virtual bool ProcessNode(g3dSceneNode* i_pNode) 
		{
			// Check if node is renderable
			if( !i_pNode->GetRenderable() || !i_pNode->GetCastsShadow() || !i_pNode->GetActiveInRenderLayer())
			{
				return false;
			}
			// See if we can cull from resolution
			if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
			{
				return false;
			}

			bool bClipChildren = true;
			if (m_bDoClip.top())
			{
				// Check if node is culled
				ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
				if( clip_result == e_Reject )
				{
					return false;
				}
				else if (clip_result == e_NoClip)
				{
					// Box is completely in view, no need to check children
					// because they will be in view also.
					bClipChildren = false;
				}
			}

			const g3dFragment* pFrag = i_pNode->GetFragment();
			// Check if it has geometry
			if( pFrag )
			{
				// Use only the "Cast Shadows" flag here
				if (pFrag->GetCastsShadow())
				{
					m_shadowNodes.push_back(i_pNode);
				}
			}
			m_bDoClip.push(bClipChildren);
			return true;
		}
		virtual void PostProcessNode(g3dSceneNode* i_pNode) 
		{
			m_bDoClip.pop();
		}

		void Clear()
		{
			m_shadowNodes.clear();
		}
	};
#endif
//------------------------------------------------------------------------
//	world_space_render
//------------------------------------------------------------------------
void g3dDepthMapRendererDX11::world_space_render( g3dSceneNode* i_pNode, 
						 const maPoint3d &i_CameraPos,
						 bool i_bRenderLowRes,
						 bool i_bDoClip)
{
	// Check if node is not renderable or if it doesn't cast a shadow
	/*if( !i_pNode->GetRenderable() || !i_pNode->GetCastsShadow())
	{
			return;
	}*/
	// if object doesn't cast shadow, return
	if (!i_pNode->GetCastsShadow() || !i_pNode->GetActiveInSceneMgr())
		return;
	
	// Special case: when enable invisibility objects cast shadows
	// nodes inactive in render layer + renderable will cast shadows
	if (!i_pNode->GetRenderable() && !g3dPrefs::CurrentPrefs().m_bEnableInvisibleCastShadows)
		return;

	if (!i_pNode->GetActiveInRenderLayer() && 
		!g3dPrefs::CurrentPrefs().m_bEnableInvisibleCastShadows)
		return;

	// See if we can cull from resolution
	if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
		return;


	if (i_bDoClip)
	{
		// Check if node is culled
		ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
		if( clip_result == e_Reject )
		{
			return;
		}
		else if (clip_result == e_NoClip)
		{
			// Box is completely in view, no need to check children
			// because they will be in view also.
			i_bDoClip = false;
		}
	}

	// Add the render state to the stack
	//add_render_state( i_pNode->GetRenderState() );

	const g3dFragment* pFrag = i_pNode->GetFragment();

	// Check if it has geometry
	if( pFrag )
	{
		// Use only the "Cast Shadows" flag here
		if (pFrag->GetCastsShadow())
		{
#ifdef HAIR_SUPPORTED
			const hairModelFrag* pHF = dynamic_cast<const hairModelFrag*>(pFrag);
			if( pHF )	//test is this is a hair fragment (render special)
			{
				if (g3dPrefs::CurrentPrefs().m_bEnableHair)
				{
					l_nNumTriangles += DrawHairNode( i_pNode );
				}
			}
			else
			{
#endif//HAIR_SUPPORTED

				// Get material for fragment
				const matMaterial *pMaterial = pFrag->GetMaterial();
				if (i_pNode->GetMaterial())
					pMaterial = i_pNode->GetMaterial();

				m_pMaskAlphaData->m_TextureTransparencyMap = NULL;
				m_pMaskAlphaData->m_Transparency = 1.0f;
				if (pMaterial )
				{
					// first check shader params
					if (pMaterial->GetShaderParams() != NULL)
					{
						if (pMaterial->GetShaderParams()->m_pTransparencyMap)
							m_pMaskAlphaData->m_TextureTransparencyMap = pMaterial->GetShaderParams()->m_pTransparencyMap->GetTexture();
						if (pMaterial->GetShaderParams()->m_pTransparency)
							m_pMaskAlphaData->m_Transparency = pMaterial->GetShaderParams()->m_pTransparency->GetProperty().GetValue();
					}
					// then check effectdata
					else if (pMaterial->GetEffectData() != NULL)
					{
						m_pMaskAlphaData->m_TextureTransparencyMap = pMaterial->GetEffectData()->GetTransparencyTexture();
						m_pMaskAlphaData->m_Transparency = pMaterial->GetEffectData()->GetTransparencyValue();
					}
				}

				bool bDither = false;
				float fDitherBias = 0.0;
				const g3dFragment* pFrag = i_pNode->GetFragment();
				pFrag->GetShadowDithering(bDither, fDitherBias);
				m_pMaskAlphaData->m_bDitherTranslucent = bDither;
				m_pMaskAlphaData->m_DitherAlphaBias = fDitherBias;

				// Render the fragment with our masking shader
	//			l_nNumTriangles += g3dRendererMgr::Render( i_pNode );

				l_nNumTriangles += DrawNode(i_pNode);

			}
#ifdef HAIR_SUPPORTED
		}
#endif
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = children.size();
	for (int i=0; i<nkids; i++)
		world_space_render(children[i], i_CameraPos, i_bRenderLowRes, i_bDoClip);

	// Remove the render state
	//subtract_render_state( i_pNode->GetRenderState() );
}

//------------------------------------------------------------------------
// set up attributes of a layer
//------------------------------------------------------------------------
void g3dDepthMapRendererDX11::setup_layer(g3dLayer& i_Layer)
{
	// TODO: [bga] - we really only want to render if Z-write is true, 
	// because that's why we are here, right?
	g3dDepthStencilStateMgr::SetDepthStencilState( (i_Layer.GetSortMethod() == g3dLayer::e_ZSort) ? ds_Disable_NS : ds_Test_Write_LessE_NS );

	// - fog already disabled
	// - lights already disabled
	// - no blending method in depth maps
}

//------------------------------------------------------------------------
// render a layer by setting up its attributes and then rendering
// its scene graph
//------------------------------------------------------------------------
void g3dDepthMapRendererDX11::render_layer(g3dLayer& i_Layer, const camCamera& i_Camera)
{
	setup_layer(i_Layer);

	// no lighting needed in depth map
	g3dLightMgrDX11::Implementation()->DisableAllLights();

	/*if (i_Layer.GetShadowRender())
		shadow_render_graph(i_Layer.GetRootNode());
	else */
	if (i_Layer.GetModelSpace() == g3dLayer::e_World)
	{
		//g3dSceneRenderUtil::update_world_data( i_Layer.GetRootNode() );
		bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
		bool bDoClip = true;
		world_space_render(i_Layer.GetRootNode(), i_Camera.GetPosition(), bLowRes, bDoClip);

		// No need to render sorted transparent nodes in depth map
		//g3dTransparencySortDX11::RenderTransparentNodes();
	}
	else
	{
		// non-world space doesn't appear in depth maps?
	}
}


int g3dDepthMapRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDepthMapRendererDX11::DrawNode" );
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	g3dFragment* pF = (g3dFragment*)i_pNode->GetFragment();
//	bool ds = pF->GetDoubleSided();
//	pF->SetDoubleSided(false);

	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

//	pF->SetDoubleSided(ds);

	D3DPERF_EndEvent();
	return nTriangles;
}

int g3dDepthMapRendererDX11::DrawHairNode(const g3dSceneNode* i_pNode)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDepthMapRendererDX11::DrawHairNode" );
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	//	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	//	ID3DXEffect* pD3DEffect = i_pEffect->GetD3DXEffect();

//	effStrandHairData HairData;
	effStrandHairData& HairData = pMaterial->StrandHairData();

//	int W, H;
//	m_pRenderTarget->GetDimensions( W, H );
//	HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}



	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

//	pEffect->SetTechnique("Default");
	pEffect->SetTechnique("ViewSpaceDepthN");
/*
#ifdef USE_NORMALIZED_DEPTHS
	pEffect->SetTechnique("ViewSpaceDepthN");
#else
	pEffect->SetTechnique("ViewSpaceDepth");
#endif
*/
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

void g3dDepthMapRendererDX11::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dDepthMapRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Blend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}