/****************************************************************************\
**	g3dRSMRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_RSMRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dRSMRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
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
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/Eff/effStrandHairData.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
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
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
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
#ifndef GVK_MATREFLECTIVESHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matReflectiveShadowMap.hpp"
#endif
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
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
	// Stats
	int l_nNumTriangles = 0;

	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dRSMRendererVk::g3dRSMRendererVk()
{
	m_pAlphaMaskMat = new matMaterial("ReflectiveShadowMap.fx");
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
g3dRSMRendererVk::~g3dRSMRendererVk()
{
	delete m_pAlphaMaskMat;
}

void g3dRSMRendererVk::ReleaseResources()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dRSMRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
							  const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							  float i_fSimTime )
{
	G3D_PROFILE_SCOPE("RSMRenderer");

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// no need for fog in depth maps
	g3dFogVk::EnableFog(false);

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// Initialize the Z buffer
	i_pWindow->ClearDepthStencil();

	// Initialize the triangle count
	l_nNumTriangles = 0;

	// Force our material for all renderers
	g3dVkUtil::SetOverrideMaterial(this->m_pAlphaMaskMat);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dVkUtil::AllowAdditiveChanges(false);

	vkdContext& context = vkdGlobal::Context();
	vkdViewport vpOld[vkdContext::c_MaxViewports];
	const uint32_t nViewPorts = context.GetViewports( vpOld );

	// Setup the viewport to match the backbuffer
	int w, h;
	i_pWindow->GetDimensions(w, h);
	vkdViewport vp;
	vp.m_Width = (float)w;
	vp.m_Height = (float)h;
	vp.m_MinDepth = 0.0f;
	vp.m_MaxDepth = 1.0f;
	vp.m_X = 0;
	vp.m_Y = 0;
	context.SetViewports( 1, &vp );

	matReflectiveShadowMap* pRSM = dynamic_cast<matReflectiveShadowMap*>(i_pWindow);

	pRSM->Clear(maFloatRGBA(0,0,0,0));
	pRSM->MakeCurrent();
	for (int i=0; i<layers.size(); i++)
	{
		this->render_layer(*layers[i], i_Camera);
	}

	pRSM->MakeCurrent();
	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		this->render_layer(*i_ViewerLayers[i], i_Camera);
	}

	// Restore the Old viewport
	context.SetViewports( nViewPorts, vpOld );

	// Restore override material
	g3dVkUtil::SetOverrideMaterial(NULL);

	g3dVkUtil::AllowAdditiveChanges(true);
// Restoring blend state, could be dropped later
	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dVkUtil::release_textures();

	return l_nNumTriangles;
}

//------------------------------------------------------------------------
//	world_space_render
//------------------------------------------------------------------------
void g3dRSMRendererVk::world_space_render( g3dSceneNode* i_pNode,
						 const maPoint3d &i_CameraPos,
						 bool i_bRenderLowRes,
						 bool i_bDoClip)
{
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

	const g3dFragment* pFrag = i_pNode->GetFragment();

	// Check if it has geometry
	if( pFrag )
	{
		// Use only the "Cast Shadows" flag here
		if (pFrag->GetCastsShadow())
		{
#ifdef HAIR_SUPPORTED
			const hairModelFragVk* pHF = dynamic_cast<const hairModelFragVk*>(pFrag);
			if( pHF )	//test is this is a hair fragment (render special)
			{
				if (g3dPrefs::CurrentPrefs().m_bEnableHair)
				{
					//l_nNumTriangles += DrawHairNode( i_pNode );
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
				l_nNumTriangles += DrawNode(i_pNode);
			}
#ifdef HAIR_SUPPORTED
		}
#endif
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = (int)children.size();
	for (int i=0; i<nkids; i++)
		world_space_render(children[i], i_CameraPos, i_bRenderLowRes, i_bDoClip);
}

//------------------------------------------------------------------------
// set up attributes of a layer
//------------------------------------------------------------------------
void g3dRSMRendererVk::setup_layer(g3dLayer& i_Layer)
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
void g3dRSMRendererVk::render_layer(g3dLayer& i_Layer, const camCamera& i_Camera)
{
	setup_layer(i_Layer);

	// no lighting needed in depth map
	g3dLightMgrVk::Implementation()->DisableAllLights();

	if (i_Layer.GetModelSpace() == g3dLayer::e_World)
	{
		bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
		bool bDoClip = true;
		world_space_render(i_Layer.GetRootNode(), i_Camera.GetPosition(), bLowRes, bDoClip);

		// No need to render sorted transparent nodes in depth map
	}
	else
	{
		// non-world space doesn't appear in depth maps?
	}
}


int g3dRSMRendererVk::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pEffectDX = dynamic_cast<effShaderBaseVk*>(pEffect)->GetFxEffect();

	const matMaterial* pMaterialOveride = i_pNode->GetMaterial();
	const g3dFragment* pFrag = i_pNode->GetFragment();
	const matMaterial* pOriMaterial = ( pMaterialOveride ) ? pMaterialOveride : pFrag->GetMaterial();

	maFloatRGBA diffuseColor(0,0,0,0);
	maFloatRGBA emissiveColor(0,0,0,0);
	matTexture* diffuseMap = NULL;
	float emissiveIntensity = 1.0f;

	if ( pOriMaterial )
	{
		if (pOriMaterial->GetShaderParams() != NULL)
		{

			effParamColor* c_emissive = pOriMaterial->GetShaderParams()->FindColorParam("g_emissive");
			if (c_emissive)
			{
				emissiveColor = c_emissive->GetProperty().GetValue();
			}

			effParamFloat* c_emissiveIntensity = pOriMaterial->GetShaderParams()->FindFloatParam("g_emissiveIntensity");
			if (c_emissiveIntensity)
			{
				emissiveIntensity = c_emissiveIntensity->GetProperty().GetValue();
			}

			if (pOriMaterial->GetShaderParams()->m_pDiffuseColor)
				diffuseColor = pOriMaterial->GetShaderParams()->m_pDiffuseColor->GetProperty().GetValue();

			if (pOriMaterial->GetShaderParams()->m_pDiffuseMap)
				diffuseMap = pOriMaterial->GetShaderParams()->m_pDiffuseMap->GetTexture();

		}else if(pOriMaterial->GetEffectData() != NULL)
		{
			diffuseColor += pOriMaterial->GetEffectData()->GetEmissive();
			diffuseColor += pOriMaterial->GetEffectData()->GetDiffuse();
		}
	}
	if (pEffectDX)
	{
		pEffectDX->GetVariableByName("g_emissiveColor")->AsVector()->SetFloatVector(emissiveColor.Ptr());
		pEffectDX->GetVariableByName("g_emissiveIntensity")->AsScalar()->SetFloat(emissiveIntensity);
		pEffectDX->GetVariableByName("g_diffuseColor")->AsVector()->SetFloatVector(diffuseColor.Ptr());
		pEffectDX->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool((diffuseMap) ? true : false);
		pEffectDX->GetVariableByName("g_diffuseMap")->AsShaderResource()->SetResource(
			g3dVkTextureUtil::GetImage(diffuseMap));
	}
	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

	return nTriangles;
}

int g3dRSMRendererVk::DrawHairNode(const g3dSceneNode* i_pNode)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	effStrandHairData& HairData = pMaterial->StrandHairData();

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	pEffect->SetTechnique("ViewSpaceDepthN");

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

	return nTriangles;
}

void g3dRSMRendererVk::InitStates()
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

void g3dRSMRendererVk::CleanupStates()
{
	delete st_NoBlend; st_NoBlend = NULL;
	delete st_Blend; st_Blend = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS; ds_Test_Write_LessE_NS = NULL;
}

}
