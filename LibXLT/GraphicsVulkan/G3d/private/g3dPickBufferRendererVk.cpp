/****************************************************************************\
**	g3dPickBufferRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_PICKBUFFERRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dPickBufferRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PICKINFO_HPP
#include "Graphics/g3d/g3dPickInfo.hpp"
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
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_G2DIMAGEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageVk.hpp"
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
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <cstring>
#include <vector>

namespace gvk
{
namespace
{
	// Stats
	int l_nNumTriangles = 0;

	// The last pixel GetPickInfo read (RGBA32F): DX11's readback surface,
	// which keeps the previous pick when the target isn't a render-target texture
	float l_PickPixel[4] = { 0, 0, 0, 0 };

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;

	//------------------------------------------------------------------------
	// Make sure all children underneath this node
	// have their pick codes flushed to a "not picked" state.
	//------------------------------------------------------------------------
	void flush_pickcodes(g3dSceneNode* i_pNode, uint32_t i_PickCode)
	{
		i_pNode->SetLowPickCode( i_PickCode );
		i_pNode->SetHighPickCode( i_PickCode );

		// flush kids
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		int nkids = (int)children.size();
		for (int i=0; i<nkids; i++)
			flush_pickcodes(children[i], i_PickCode);
	}

}	// end if namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPickBufferRendererVk::g3dPickBufferRendererVk()
: m_FragmentCount(0), m_nOldViewports(0)
{
	m_pColorCodeMat = new matMaterial("MaskAlpha.fx");
	m_pMaskAlphaData = dynamic_cast<effMaskAlphaData*>(m_pColorCodeMat->GetEffectData());
	DBG_ASSERT(m_pMaskAlphaData, "Could not load shader needed for PickBuffer rendering");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPickBufferRendererVk::~g3dPickBufferRendererVk()
{
	delete m_pColorCodeMat;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dPickBufferRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{
	G3D_PROFILE_SCOPE("PickBufferRenderer");

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in pick buffers
	g3dFogVk::EnableFog(false);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// Initialize the Z buffer
	i_pWindow->ClearDepthStencil();

	// Initialize the triangle count
	l_nNumTriangles = 0;

	// Initialize the variable that counts the fragments
	m_FragmentCount = 1;

	// Force our material for all renderers
	g3dVkUtil::SetOverrideMaterial(this->m_pColorCodeMat);

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
	g3dVkUtil::SetOverrideMaterial(NULL);

	g3dVkUtil::release_textures();

	return l_nNumTriangles;
}


//------------------------------------------------------------------------
//	world_space_render
//------------------------------------------------------------------------
void g3dPickBufferRendererVk::world_space_render( g3dSceneNode* i_pNode,
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
	int nkids = (int)children.size();
	for (int i=0; i<nkids; i++)
		world_space_render(children[i], i_CameraPos, i_bRenderLowRes);


	// Reset the high range for this node to signify which
	// fragment codes were used in this node and its children
	i_pNode->SetHighPickCode( m_FragmentCount );
}

//------------------------------------------------------------------------
// set up attributes of a layer
//------------------------------------------------------------------------
void g3dPickBufferRendererVk::setup_layer(g3dLayer& i_Layer)
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
void g3dPickBufferRendererVk::render_layer(g3dLayer& i_Layer, const camCamera& i_Camera)
{
	this->setup_layer(i_Layer);

	// no lighting needed in pick buffer
	g3dLightMgrVk::Implementation()->DisableAllLights();

	if (i_Layer.GetModelSpace() == g3dLayer::e_World)
	{
		bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
		this->world_space_render(i_Layer.GetRootNode(), i_Camera.GetPosition(), bLowRes);

		// No need to render sorted transparent nodes in pick buffer?
	}
	else
	{
		// non-world space doesn't appear in pick buffers?
	}
}

int g3dPickBufferRendererVk::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	pEffect->SetTechnique(matShaderEffect::e_Environment);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);
	// GET IT FROM FRAGMENT OR MATERIAL?
	bool bDither = false;
	float fDitherBias = 0.0;
	const g3dFragment* pFrag = i_pNode->GetFragment();
	pFrag->GetShadowDithering(bDither, fDitherBias);
	g3dVkUtil::SetupDitheredShadows(bDither, fDitherBias);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	return nTriangles;
}

//--------------------------------------------------------------------
//	set a 1x1 viewport before clearing.
//--------------------------------------------------------------------
void g3dPickBufferRendererVk::SetPickViewport(g2dRenderTarget* i_pTarget)
{
	vkdContext& context = vkdGlobal::Context();
	m_nOldViewports = context.GetViewports( m_OldViewports );
	vkdViewport new_viewport;
	new_viewport.m_Width = 1;
	new_viewport.m_Height = 1;
	new_viewport.m_X = 0;
	new_viewport.m_Y = 0;
	new_viewport.m_MinDepth = 0.0f;
	new_viewport.m_MaxDepth = 1.0f;
	context.SetViewports( 1, &new_viewport );
}

//--------------------------------------------------------------------
//	restore viewport (D3D11's RSSetViewports(1, ...) of the first one)
//--------------------------------------------------------------------
void g3dPickBufferRendererVk::RestoreViewport()
{
	vkdGlobal::Context().SetViewports( m_nOldViewports > 0 ? 1 : 0, m_OldViewports );
}

//--------------------------------------------------------------------
//	return info about the picked object.
//--------------------------------------------------------------------
void g3dPickBufferRendererVk::GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo)
{
	// Get out the pixel color

	// copy from gpu (submitting what was drawn and waiting for it)
	matRenderTargetTexture* pTarget = dynamic_cast<matRenderTargetTexture*>( i_pWindow );
	if (pTarget && pTarget->GetImage())
	{
		vkdImage* pImage = pTarget->GetImage();
		DBG_ASSERT(pImage->Format() == VK_FORMAT_R32G32B32A32_SFLOAT, "pick format mismatch");
		DBG_ASSERT(pImage->Height() == 1, "pick height mismatch");
		DBG_ASSERT(pImage->Width() == 1, "pick width mismatch");
		vkdGlobal::Context().Flush();
		const std::vector<uint8_t> pixels = pImage->ReadBack(0, 0);
		if (pixels.size() >= sizeof(l_PickPixel))
			memcpy(l_PickPixel, pixels.data(), sizeof(l_PickPixel));
	}

	g2dImageVk image_wrapper;
	image_wrapper.MakeFromDxgiFormat(1, 1, 2);		// DXGI_FORMAT_R32G32B32A32_FLOAT
	memcpy(image_wrapper.Lock(), l_PickPixel, sizeof(l_PickPixel));
	image_wrapper.Release();

	// Get color at (0,0) pixel location (the single pixel viewport set above)
	// red is object ID, (green,blue) is UV, and alpha is depth.
	// see MaskAlpha.fx for details.
	float red = 0, green = 0, blue = 0, alpha = 0;
	image_wrapper.GetPixelColor(0, 0, red, green, blue, alpha);
	uint32_t objectCode = (uint32_t)red;

	o_PickInfo.m_ObjectID = objectCode;
	o_PickInfo.m_Depth = alpha;
	o_PickInfo.m_U = green;
	o_PickInfo.m_V = blue;
}

void g3dPickBufferRendererVk::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dPickBufferRendererVk::CleanupStates()
{
	delete st_NoBlend; st_NoBlend = NULL;
	delete st_Blend; st_Blend = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS; ds_Test_Write_LessE_NS = NULL;
	memset(l_PickPixel, 0, sizeof(l_PickPixel));
}

}
