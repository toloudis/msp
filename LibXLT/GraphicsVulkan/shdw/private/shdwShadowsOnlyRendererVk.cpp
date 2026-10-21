/****************************************************************************\
**	shdwShadowsOnlyRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_SHADOWSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowsOnlyRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
//#include "Graphics/Eff/effStrandHairData.hpp"
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/eff/effTexturedData.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
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
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_EFF_STRANDHAIR_HPP
#include "GraphicsVulkan/Eff/effStrandHair.hpp"
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
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
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
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
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
	int l_nNumTriangles = 0;

	// the hdr pixel format we use.
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 16*4);

	g3dBlendStateMgr::BlendState* stp_AddBlend;
	g3dBlendStateMgr::BlendState* stp_MulBlend;
	g3dBlendStateMgr::BlendState* stp_MulNoBlend;
	g3dBlendStateMgr::BlendState* stp_IlluminationBlend;

	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Disable_NS;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwShadowsOnlyRendererVk::InitStates()
{
	stp_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_MulNoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_IlluminationBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwShadowsOnlyRendererVk::CleanupStates()
{
	delete stp_AddBlend;
	delete stp_MulBlend;
	delete stp_MulNoBlend;
	delete stp_IlluminationBlend;

	delete dsp_Test_Write_LessE_NS;
	delete dsp_Disable_NS;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwShadowsOnlyRendererVk::shdwShadowsOnlyRendererVk(bool i_bMasking)
: m_Width(0), m_Height(0), m_bMasking(i_bMasking)
{
	m_pIlluminationMat = new matMaterial("IlluminationOnly.fx");
	m_pShadowMat = new matMaterial("ShadowsOnly.fx");
	m_pShadowData = dynamic_cast<effTexturedData*>(m_pShadowMat->GetEffectData());
	DBG_ASSERT(m_pShadowData, "Could not load shader needed for Ambient Occlusion rendering");
	m_pHairMat = new matMaterial("HairDefault.fx");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwShadowsOnlyRendererVk::~shdwShadowsOnlyRendererVk()
{
	delete m_pHairMat;
	delete m_pShadowMat;
	delete m_pIlluminationMat;
	ReleaseResources();
}

void shdwShadowsOnlyRendererVk::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwShadowsOnlyRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("ShadowsOnlyRenderer");

	if (!g3dPrefs::CurrentPrefs().m_bProjLightsOn)
		return 0;

	m_pWindow = i_pWindow;

	CreateSurfaces(i_pWindow);

#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentRenderTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentRenderTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentRenderTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentRenderTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}
#endif
	currentRenderTarget->MakeCurrent();

	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	m_pCamera = &i_Camera;
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// clear target and initialize the Z buffer
	maFloatRGBA bg = m_bMasking ? maFloatRGBA(1.0f,1.0f,1.0f,1.0f) : maFloatRGBA(0,0,0,0);
	currentRenderTarget->Clear(bg, true, 1.0f, false );

	// Initialize the triangle count
	l_nNumTriangles = 0;

	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// fill Z with opaques.
	// this allows for all passes to do correct blending.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTriangles += depthPrePass.Render(i_fSimTime);

	if (m_bMasking)
	{
		g3dBlendStateMgr::SetBlendState(stp_IlluminationBlend);
	}
	else
	{
		g3dBlendStateMgr::SetBlendState(stp_AddBlend);
	}

	g3dVkUtil::AllowAdditiveChanges(false);

	// draw!!
	int i, j, n;
	// draw opaques per-light, shaded, and add to hdr buffer
	// lights
	const std::vector<g3dLight*>& lights = g3dLightMgrVk::Implementation()->GetLights( );
	int num_lights = lights.size();
	for (i = 0; i < num_lights; i++)
	{
		g3dLight* pLight = lights[i];
		if ( !pLight->IsEnabled() )
			continue;
		if ( !pLight->GetCastsShadow() )
			continue;

		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);
		// masking mode only uses proj lights?
		if (m_bMasking && (proj_light == NULL))
			continue;

		g3dSingleLightRendering::SetActiveLight( pLight );
		g3dLightMgrVk::Implementation()->SetLight( pLight );
		g3dLightMgrVk::Implementation()->EnableLight( pLight );

		const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
		n = shadowNodes.size();
		for (j = 0; j < n; j++)
		{
			l_nNumTriangles += CullOrDrawNode(shadowNodes[j], pLight, proj_light);
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
				if (!it->m_pSceneNode->GetFragment()->IsShadowHull())
				{
					if (m_SceneDatabase.GetRenderStateCache()->IsLightInState(it->m_RenderStateCache, pLight))
					{
						if (proj_light && g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
						{
							if (g3dSceneRenderUtil::get_box_vis(it->m_pSceneNode->GetWorldBox(),
								&(proj_light->GetTotalMatrix()), true) == e_Reject)
							{
								continue;
							}
						}
						// if masking, we should only have proj lights anyway...
						if (m_bMasking && proj_light)
						{
							g3dProjectedLight shadowLight(*proj_light);
							shadowLight.SetIntensity(maFloatRGBA(1,1,1,1));
							shadowLight.SetIntensityFactor(1);
							shadowLight.SetShadowColor(maFloatRGBA(0,0,0,0));
							shadowLight.SetShadowIntensity(1);
							shadowLight.SetTexture(NULL);
							shadowLight.SetFalloff0(1);
							shadowLight.SetFalloff1(0);
							shadowLight.SetFalloff2(0);
							shadowLight.SetFalloff3(0);
							shadowLight.SetFalloffStart(0);

							l_nNumTriangles += DrawHairNode(it->m_pSceneNode, &shadowLight, &shadowLight);
						}
						else
						{
							l_nNumTriangles += DrawHairNode(it->m_pSceneNode, pLight, proj_light);
						}
					}
				}



			}
		}
#endif//HAIR_SUPPORTED

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
					l_nNumTriangles += CullOrDrawNode(NS, pLight, proj_light);
				}
			}
		}

		g3dLightMgrVk::Implementation()->DisableLight( pLight );
		g3dSingleLightRendering::SetActiveLight( NULL );
	}

	g3dVkUtil::AllowAdditiveChanges(true);

#ifdef NEW_AA_CODE
	if( !currentRenderTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
	{
		DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
	}
#else
	if (m_pFrameBuffer->HDRAATarget() != NULL)
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
	}
#endif
	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTriangles += rLayer.Render(i_fSimTime);
	}


	vkdImage* nullTex[1] = {NULL};
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

{//PROFILE("debug");
	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	g2dScreen::SetDebugInfo(2, num);
}
	return l_nNumTriangles;
}

int shdwShadowsOnlyRendererVk::DrawNode(const g3dSceneNode* i_pNode,
										 g3dLight* i_pLight,
										 const g3dProjectedLight* i_pProjLight)
{

	// resolve material/effect
	matShaderEffect* pEffect = matShaderMgr::GetEffect( m_bMasking ? *m_pShadowMat : *m_pIlluminationMat );
	effShaderBaseVk* pEffVk = dynamic_cast<effShaderBaseVk*>(pEffect);
	fxEffect* pID3DXEffect = pEffVk->GetFxEffect();

	pID3DXEffect->GetVariableByName("g_UseCosine")->AsScalar()->SetBool((!m_bMasking && g3dPrefs::CurrentPrefs().m_bIlluminationUsesNormals) ? true : false);

	DBG_ASSERT(i_pLight != NULL, "null light in shadows only drawnode");
	if (i_pProjLight != NULL)
	{
		matShaderEffect::Technique tec = g3dSceneRenderUtil::SelectShadowTechnique(i_pProjLight);
		pEffect->SetTechnique(tec);
	}
	else
		pEffect->SetTechnique(matShaderEffect::e_SingleLight);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// light before material so that material can override light settings if needed.
	pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());

	pEffect->SetupMaterial( m_pShadowMat );

	//setup transparent params
	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_pNode->GetFragment()->GetMaterial();
	if( pOrigMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pOrigMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pOrigMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pID3DXEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? true : false);
	pID3DXEffect->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dVkTextureUtil::GetImage(pTex) : NULL);
	pID3DXEffect->GetVariableByName("g_transparency")->AsScalar()->SetFloat(val);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, m_pShadowMat, pEffect );
	return nTriangles;
}

int shdwShadowsOnlyRendererVk::DrawHairNode(const g3dSceneNode* i_pNode,
										 g3dLight* i_pLight,
										 const g3dProjectedLight* i_pProjLight)
{
//	effStrandHairData HairData;
	effStrandHairData& HairData = m_pHairMat->StrandHairData();

	HairData.m_InvScreenSize = maVector2d( 1.0f / m_Width, 1.0f / m_Height );

//	maMatrix4x4 proj;
//	m_pCamera->GetProjectionMatrix( proj );
//	HairData.m_ProjAspect = maVector2d( proj.m_Mat[0], proj.m_Mat[5] );

	HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;

	// resolve material/effect
	matShaderEffect* pEffect = matShaderMgr::GetEffect( *m_pHairMat );
	effStrandHair* pHairEffect = dynamic_cast<effStrandHair*>(pEffect);
	DBG_ASSERT( pHairEffect, "Non hair effect used." );
	fxEffect* pID3DXEffect = pHairEffect->GetFxEffect();

//	effShaderBaseVk* pEffVk = dynamic_cast<effShaderBaseVk*>(pEffect);
//	ID3DXEffect* pID3DXEffect = pEffVk->GetFxEffect();
	pID3DXEffect->GetVariableByName("g_UseCosine")->AsScalar()->SetBool((!m_bMasking && g3dPrefs::CurrentPrefs().m_bIlluminationUsesNormals) ? true : false);

	DBG_ASSERT(i_pLight != NULL, "null light in shadows only drawnode");
	if (i_pProjLight != NULL)
	{
		matShaderEffect::Technique tec = g3dSceneRenderUtil::SelectShadowTechnique(i_pProjLight);
		pEffect->SetTechnique(tec);

		maMatrix4x4 vmat = i_pProjLight->GetCameraMatrix();
		vmat.Transpose();
		HairData.m_LightViewPlane = maVector4d( &vmat.m_Mat[8] );//access the 3rd row
	}
	else pEffect->SetTechnique(matShaderEffect::e_SingleLight);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	// light before material so that material can override light settings if needed.
	pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());

	pEffect->SetupMaterial(m_pHairMat);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, m_pHairMat, pHairEffect );
	return nTriangles;
}

//--------------------------------------------------------------------
//	Test node against light set and light frustum. If it passes, draw it.
//--------------------------------------------------------------------
int shdwShadowsOnlyRendererVk::CullOrDrawNode(const sNodePlusState& i_Node, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight)
{
	int nTriangles = 0;
	if (!i_Node.m_pNode->GetFragment()->IsShadowHull())
	{
		if (m_SceneDatabase.GetRenderStateCache()->IsLightInState(i_Node.m_StateCache, i_pLight))
		{
			if (i_pProjLight && g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
			{
				if (g3dSceneRenderUtil::get_box_vis(i_Node.m_pNode->GetWorldBox(),
					&(i_pProjLight->GetTotalMatrix()), true) == e_Reject)
				{
					return 0;
				}
			}
			// if masking, we should only have proj lights anyway...
			if (m_bMasking && i_pProjLight)
			{
				g3dProjectedLight shadowLight(*i_pProjLight);
				shadowLight.SetIntensity(maFloatRGBA(1,1,1,1));
				shadowLight.SetIntensityFactor(1);
				shadowLight.SetShadowColor(maFloatRGBA(0,0,0,0));
				shadowLight.SetShadowIntensity(1);
				shadowLight.SetTexture(NULL);
				shadowLight.SetFalloff0(1);
				shadowLight.SetFalloff1(0);
				shadowLight.SetFalloff2(0);
				shadowLight.SetFalloff3(0);
				shadowLight.SetFalloffStart(0);
				nTriangles += DrawNode(i_Node.m_pNode, &shadowLight, &shadowLight);
			}
			else
			{
				nTriangles += DrawNode(i_Node.m_pNode, i_pLight, i_pProjLight);
			}
		}
	}
	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwShadowsOnlyRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwShadowsOnlyRendererVk::GetRawBuffer()
{
	return (m_bMasking) ? NULL : m_pFrameBuffer->HDRRenderTargetTex();
}

g2dPFD::PixelFormat shdwShadowsOnlyRendererVk::GetRawBufferPFD()
{
	return l_hdrPFD.GetPixelFormat();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwShadowsOnlyRendererVk::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(stp_MulNoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dVkUtil::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(stp_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

}
