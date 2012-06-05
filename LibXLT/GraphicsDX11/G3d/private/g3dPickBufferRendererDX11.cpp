/****************************************************************************\
**	g3dPickBufferRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g3d/g3dPickBufferRendererDX11.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPickInfo.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"
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

	// For color to index conversion:
	const int c_Mask = 0xFF;
	const float c_Div = 255.0f; // (float) mask ?
	const int c_Shift = 8;
	/* For testing with much more visible color changes
	const int c_Mask = 0x3;
	const float c_Div = 3.0f;
	const int c_Shift = 2;*/

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;

	//------------------------------------------------------------------------
	// Make sure all children underneath this node
	// have their pick codes flushed to a "not picked" state.
	//------------------------------------------------------------------------
	void flush_pickcodes(g3dSceneNode* i_pNode, envType::UInt32 i_PickCode)
	{
		i_pNode->SetLowPickCode( i_PickCode );
		i_pNode->SetHighPickCode( i_PickCode );

		// flush kids
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		int nkids = children.size();
		for (int i=0; i<nkids; i++)
			flush_pickcodes(children[i], i_PickCode);
	}

}	// end if namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maFloatRGBA g3dPickBufferRendererDX11::ConvertIndexToColor(envType::UInt32 i_FragmentIndex)
{
	float r = (i_FragmentIndex & c_Mask) / c_Div;
	i_FragmentIndex >>= c_Shift;
	float g = (i_FragmentIndex & c_Mask) / c_Div;
	i_FragmentIndex >>= c_Shift;
	float b = (i_FragmentIndex & c_Mask) / c_Div;
	
	DBG_ASSERT(i_FragmentIndex <= c_Mask, "Too many fragments to encode into color");

	return maFloatRGBA(r,g,b, 1.0f);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envType::UInt32 g3dPickBufferRendererDX11::ConvertColorToIndex(envType::UInt8 i_Red, 
	envType::UInt8 i_Green,
	envType::UInt8 i_Blue)
{
	return ( (i_Blue << (c_Shift*2)) + (i_Green << c_Shift) + i_Red);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPickBufferRendererDX11::g3dPickBufferRendererDX11()
: m_FragmentCount(0)
{
	m_pColorCodeMat = new matMaterial("MaskAlpha.fx");
	m_pMaskAlphaData = dynamic_cast<effMaskAlphaData*>(m_pColorCodeMat->GetEffectData());
	DBG_ASSERT(m_pMaskAlphaData, "Could not load shader needed for PickBuffer rendering");

	m_pReadbackSurface = NULL;

	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, 
		1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ);
	// optimize: pass data in here in the 2nd arg.
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &m_pReadbackSurface);
	if ( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(false, "error creating image surface");
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPickBufferRendererDX11::~g3dPickBufferRendererDX11()
{
	delete m_pColorCodeMat;

	m_pReadbackSurface->Release();
	m_pReadbackSurface = NULL;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dPickBufferRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dPickBufferRendererDX11::Render" );

//	HRESULT op_result;

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in pick buffers
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

	// Initialize the variable that counts the fragments
	m_FragmentCount = 1;

	// Force our material for all renderers
	g3dDX11Util::SetOverrideMaterial(this->m_pColorCodeMat);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	// Render the layers
	for (int i=0; i<layers.size(); i++)
	{
		this->render_layer(*layers[i], i_Camera);
	}
	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		this->render_layer(*i_ViewerLayers[i], i_Camera);
	}

	g3dBlendStateMgr::SetBlendState(st_Blend);

	// Restore override material
	g3dDX11Util::SetOverrideMaterial(NULL);

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


//------------------------------------------------------------------------
//	world_space_render
//------------------------------------------------------------------------
void g3dPickBufferRendererDX11::world_space_render( g3dSceneNode* i_pNode, 
						 const maPoint3d &i_CameraPos,
						 bool i_bRenderLowRes )
{
	// Check if node is not renderable or if it isn't pickable
	if ( (!i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer()) && !i_pNode->GetPickHull())
		return;
	if (!i_pNode->GetGPUPickable())
		return;

	// Set the low and high range for this node
	i_pNode->SetLowPickCode( m_FragmentCount );
	i_pNode->SetHighPickCode( m_FragmentCount );

	// See if we can cull from resolution
	if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
		return;
	
	// If both pick masks are non-zero, use the pick mask to filter the results
	if ((GetPickMask() > 0) && (i_pNode->GetPickMask() > 0))
	{
		// Bit-wise compare, if zero then do not pick
		if ((GetPickMask() & i_pNode->GetPickMask()) == 0)
			return;
	}

	// Check if node is culled
	ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
	if( clip_result == e_Reject )
	{
		// Make sure all children underneath this node
		// have their pick codes flushed to a "not picked" state.
		flush_pickcodes(i_pNode, m_FragmentCount);
		return;
	}

	// Add the render state to the stack
	//add_render_state( i_pNode->GetRenderState() );

	const g3dFragment* pFrag = i_pNode->GetFragment();

	// Check if it has geometry
	if( pFrag )
	{
		// Set color to encode the object ID
		m_pMaskAlphaData->m_Color = maFloatRGBA((float)m_FragmentCount, (float)m_FragmentCount, (float)m_FragmentCount, (float)m_FragmentCount );

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

		// Render the fragment with our masking shader
		l_nNumTriangles += DrawNode( i_pNode );

		m_FragmentCount++;
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = children.size();
	for (int i=0; i<nkids; i++)
		world_space_render(children[i], i_CameraPos, i_bRenderLowRes);

	
	// Reset the high range for this node to signify which
	// fragment codes were used in this node and its children
	i_pNode->SetHighPickCode( m_FragmentCount );

	// Remove the render state
	//subtract_render_state( i_pNode->GetRenderState() );
}

//------------------------------------------------------------------------
// set up attributes of a layer
//------------------------------------------------------------------------
void g3dPickBufferRendererDX11::setup_layer(g3dLayer& i_Layer)
{
	// TODO: [bga] - we really only want to render if Z-write is true, 
	// because that's why we are here, right?
	g3dDepthStencilStateMgr::SetDepthStencilState( i_Layer.GetSortMethod() == g3dLayer::e_ZSort ? ds_Disable_NS : ds_Test_Write_LessE_NS );

	// - fog already disabled
	// - lights already disabled
	// - no blending method in pick buffers
}

//------------------------------------------------------------------------
// render a layer by setting up its attributes and then rendering
// its scene graph
//------------------------------------------------------------------------
void g3dPickBufferRendererDX11::render_layer(g3dLayer& i_Layer, const camCamera& i_Camera)
{
	this->setup_layer(i_Layer);

	// no lighting needed in pick buffer
	g3dLightMgrDX11::Implementation()->DisableAllLights();

	/*if (i_Layer.GetShadowRender())
		shadow_render_graph(i_Layer.GetRootNode());
	else */
	if (i_Layer.GetModelSpace() == g3dLayer::e_World)
	{
		//g3dSceneRenderUtil::update_world_data( i_Layer.GetRootNode() );
		bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
		this->world_space_render(i_Layer.GetRootNode(), i_Camera.GetPosition(), bLowRes);

		// No need to render sorted transparent nodes in pick buffer?
		//g3dTransparencySortDX11::RenderTransparentNodes();
	}
	else
	{
		// non-world space doesn't appear in pick buffers?
	}
}

int g3dPickBufferRendererDX11::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	pEffect->SetTechnique(matShaderEffect::e_Environment);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);
	// GET IT FROM FRAGMENT OR MATERIAL?
	bool bDither = false;
	float fDitherBias = 0.0;
	const g3dFragment* pFrag = i_pNode->GetFragment();
	pFrag->GetShadowDithering(bDither, fDitherBias);
	g3dDX11Util::SetupDitheredShadows(bDither, fDitherBias);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	return nTriangles;
}

//--------------------------------------------------------------------
//	set a 1x1 viewport before clearing.
//--------------------------------------------------------------------
void g3dPickBufferRendererDX11::SetPickViewport(g2dRenderTarget* i_pTarget)
{
	UINT numViews = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &numViews, &m_OldViewport );
	D3D11_VIEWPORT new_viewport;
	new_viewport.Width = 1;
	new_viewport.Height = 1;
	new_viewport.TopLeftX = 0;
	new_viewport.TopLeftY = 0;
	new_viewport.MinDepth = 0.0f;
	new_viewport.MaxDepth = 1.0f;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &new_viewport );
}

//--------------------------------------------------------------------
//	restore viewport
//--------------------------------------------------------------------
void g3dPickBufferRendererDX11::RestoreViewport()
{
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &m_OldViewport );
}

//--------------------------------------------------------------------
//	return info about the picked object.
//--------------------------------------------------------------------
void g3dPickBufferRendererDX11::GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo)
{
	// Get out the pixel color

	// copy from gpu to readback surface.
	matRenderTargetTexture* pTarget = dynamic_cast<matRenderTargetTexture*>( i_pWindow );
	if (pTarget)
	{
		g2dD3D11TexturePtr pTex = pTarget->GetTextureSurface();

		//	test code
#ifdef _DEBUG
		D3D11_TEXTURE2D_DESC pickDesc;
		pTex->GetDesc(&pickDesc);
		D3D11_TEXTURE2D_DESC readbackDesc;
		m_pReadbackSurface->GetDesc(&readbackDesc);
		DBG_ASSERT(pickDesc.Format == readbackDesc.Format, "pick format mismatch");
		DBG_ASSERT(pickDesc.Height == readbackDesc.Height, "pick height mismatch");
		DBG_ASSERT(pickDesc.Width == readbackDesc.Width, "pick width mismatch");
#endif
		//
		g2dDX11Global::g_pDeviceContext->CopyResource(m_pReadbackSurface, pTex);
	}

	// Addref because g2dImage will release the surface but we want to keep it 
	// around for the next pick.
	m_pReadbackSurface->AddRef();
	g2dImageDX11 image_wrapper(m_pReadbackSurface); 

	// Get color at (0,0) pixel location (the single pixel viewport set above)
	// red is object ID, (green,blue) is UV, and alpha is depth.
	// see MaskAlpha.fx for details.
	envType::Float32 red = 0, green = 0, blue = 0, alpha = 0;
	// note RGBA-BGRA swap here. this undoes a channel swap that happens internally:
	image_wrapper.GetPixelColor(0, 0, red, green, blue, alpha);
	envType::UInt32 objectCode = (envType::UInt32)red;

	o_PickInfo.m_ObjectID = objectCode;
	o_PickInfo.m_Depth = alpha;
	o_PickInfo.m_U = green;
	o_PickInfo.m_V = blue;
}

void g3dPickBufferRendererDX11::InitStates()
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

void g3dPickBufferRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Blend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}