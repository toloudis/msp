/****************************************************************************\
**	shdwPassGlow.cpp
**
**	Render glowing objects into a buffer and then smear that buffer over
**	the scene.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSGLOW_HPP
#include "GraphicsVulkan/shdw/shdwPassGlow.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
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
#ifndef GVK_G2DRENDERTARGETVK_HPP
#include "GraphicsVulkan/G2d/g2dRenderTargetVk.hpp"
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
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassGlow::shdwPassGlow(
	g3dLayer*				i_pLayer,
	const camCamera*		i_pCamera,
	matRenderTargetTexture* i_SceneTarget,
	matRenderTargetTexture* i_glowTarget )
:	m_pLayer(i_pLayer),
	m_pCamera(i_pCamera),
	m_sceneInfo(NULL),
	m_glowTarget(i_glowTarget),
	m_SceneTarget(i_SceneTarget)
{
	SetRenderTarget(i_SceneTarget);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassGlow::~shdwPassGlow()
{
}

//----------------------------------------------------------------------------------------
// Here's the strategy:
// Render each object that glows, to a glow surface.
// This surface is smaller so the obj is downsampled.
// Render only the specular parts of the object, and use
// a mask texture to block out parts that should not glow.
// Then apply the glow params to the glow shader. This will
// apply a blur filter to the glow surface
// contents, and blend it onto the scene.
// This requires each object to be done one by one, since each ob may have its own
// glow params (spread, intensity, etc) for the blur pass.
// Also requires a specular-only technique in each shader.
//----------------------------------------------------------------------------------------
int shdwPassGlow::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassGlow");
	m_stats.Reset();

	if (!g3dSingleLightRendering::GetDoSingleLightRendering())
		return 0;
	if (g3dSingleLightRendering::GetDoDOFPrepPass())
		return 0;

	nodeCacheList glowNodes = m_sceneInfo->GetGlowNodes();
	if (glowNodes.empty())
		return 0;

	// lights
	const std::vector<g3dLight*>& lights = g3dLightMgrVk::Implementation()->GetLights( );

	int num_lights = lights.size();
	if (num_lights == 0)
		return 0;

	// Disable all lights and then turn them on one at a time
	g3dLightMgrVk::Implementation()->DisableAllLights();

	//share the depth buffer with the glow target
	//we can do this since we are only testing depths
	shared_ptr<g2dDepthStencilBuffer> dbuf = m_SceneTarget->GetDepthStencilBuffer();
	DBG_ASSERT( dbuf != NULL, "No Depth Buffer, must resolve HDR depth to HDRTargetTexture first!" );
	m_glowTarget->SetDepthBuffer( dbuf );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

	// Render the glowing objects.
	// Each object can have its own params that describe
	// the way its glow is composited.
	int num_nodes = glowNodes.size();
	int nSpecularGlow=0, nConstGlow=0;

	const g3dSceneNode* pNode = NULL;
	const g3dFragment* pFragment = NULL;
	for (int n=0; n<num_nodes; n++)//num_nodes; n++)
	{
		pNode = glowNodes[n].m_pNode;
		pFragment = pNode->GetFragment();
		if (pFragment->IsShadowHull())
			continue;

		// clear the glow texture and draw to it
		m_glowTarget->MakeCurrent();
		m_glowTarget->Clear( maFloatRGBA() );

		g3dSingleLightRendering::SetDoGlowPass(true);

		// Set mode to additive with alpha blend so that
		// each light's contribution adds to the color
		// already in the buffer
		//
		// needed to guarantee the blend modes here. SetAdditive might not actually change them,
		// but prior render passes may have installed a custom one.
		g3dBlendStateMgr::SetBlendState(st_Blend);
		g3dVkUtil::AllowAdditiveChanges(false);

		const matMaterial* material = pFragment->GetMaterial();
		if (material->GlowData().m_bConstantGlow)
		{
			DrawNode(pNode, false, NULL, NULL);
			nConstGlow++;
		}
		else
		{
			const g3dLight* pOldLight = NULL;
			int nl = 0;
			for (int i=0; i<num_lights; i++)
			{
				g3dLight* pLight = lights[i];
				if( !pLight->IsEnabled() )
					continue;
				if( !pLight->GetAffectsGlow() )
					continue;

				if (m_sceneInfo->GetRenderStateCache()->IsLightInState(glowNodes[n].m_StateCache, pLight))
				{
					// Enable the one shadow light for this pass
					pOldLight = g3dSingleLightRendering::GetActiveLight();
					g3dSingleLightRendering::SetActiveLight( pLight );
					g3dLightMgrVk::Implementation()->SetLight( pLight );
					g3dLightMgrVk::Implementation()->EnableLight( pLight );

					DrawNode(pNode, true, pLight, dynamic_cast<g3dProjectedLight*>(pLight));

					nl++;
					//						break; // uncomment this line to break after only one light
					g3dLightMgrVk::Implementation()->DisableLight( pLight );
					g3dSingleLightRendering::SetActiveLight( pOldLight );
				}

				//g3dSingleLightRendering::SetActiveLight( NULL );
			}
			nSpecularGlow++;
		}
		g3dSingleLightRendering::SetDoGlowPass(false);

		// now render using the glow shader to smear out the glow.
		// draw to m_tempTarget. this target contains the pre-glow rendered scene.
		// we will blend m_glowTarget into this.
		// add this onto the main render target
		g3dBlendStateMgr::SetBlendState(st_Blend);

		g3dRenderGlow postGlow(m_glowTarget, m_pRenderTarget, pNode);//, znear);
		m_stats.m_nTriangles += postGlow.Render(i_time);
	}

	//restore default states
	shared_ptr<g2dDepthStencilBuffer> empty;
	m_glowTarget->SetDepthBuffer( empty );		//clear the shared depth buffer

	g3dVkUtil::AllowAdditiveChanges(true);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassGlow::DrawNode(const g3dSceneNode* i_pNode, bool i_bLit, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;

	int nTriangles = 0;
	const int num_layers = pMaterial->GetNumMaterialLayers();
	for (int layer_index=0; layer_index<num_layers; ++layer_index)
	{
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial, layer_index);

		pEffect->SetTechnique(matShaderEffect::e_SpecularGlow);

		// set shader globals
		g3dVkUtil::SetupShaderGlobals(pEffect);

		// geometry data
		g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode, layer_index);

		// shading data
		pEffect->SetupMaterial(pMaterial, layer_index);

		pEffect->SetupGlowPass(pMaterial->GetGlowData());
		if (i_bLit && pEffect->DoesLighting())
		{
			DBG_ASSERT(i_pLight != NULL, "null light in shdwpasslit drawnode");
			pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());
		}

		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
		m_stats.m_nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

		// reset glow to null state (glowsize reused in vtx shaders)
		pEffect->SetupGlowPass(effGlowData());
	}
}

void shdwPassGlow::InitStates()
{
	const VkColorComponentFlags writeAll = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		writeAll );
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		writeAll );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassGlow::CleanupStates()
{
	delete st_Blend;				st_Blend = NULL;
	delete st_NoBlend;				st_NoBlend = NULL;
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_LessE_NS;		ds_Test_LessE_NS = NULL;
}

}
