/*****************************************************************************
**  shdwPassTransparent.cpp
**
**      See shdwPassTransparent.hpp.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
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
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
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
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDOCCLUSIONQUERY_HPP
#include "GraphicsVulkan/vkd/vkdOcclusionQuery.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

bool l_bHaveShadowCastingLights = false;

//Note that Alpha test pass cannot work with depth peeling (so currently bypassed)
//depth peeling requires the exact depths at each layer, pre passing causes holes in the solid parts.
static bool s_bUseSolidAlphaTestPrepass = true;

static unsigned int s_alphaTestRef = 0x000000ff;
static bool s_bCullDepthPeel = true;

#define MAX_TRANSPARENT_DEPTH_PEEL_PASSES 254

namespace
{

	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_Accumulate = NULL;
	g3dBlendStateMgr::BlendState* st_MultiRender = NULL;
	g3dBlendStateMgr::BlendState* st_CompositeRev = NULL;
	g3dBlendStateMgr::BlendState* st_RenderInit = NULL;
	g3dBlendStateMgr::BlendState* st_CopyTarget = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Equal_NS = NULL;

	//setup local blending functions for simplicity
	void SetInitialBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_Initial );
	}
	void SetInitialAdditiveBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_RenderInit );
	}

	void SetAccumulationBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_Accumulate );
	}

	//sets the blend mode for rendering multilights
	void SetMultiRenderBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_MultiRender );
	}

	//sets the blend mode used for compositing the peeled transparent layer with the scene
	void SetCompositeBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_CompositeRev );
	}

	void RestoreBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_Initial );
	}
//------------------------------------------------------------------------
// rendering a single transparent node in a multi-pass
// configuration, some nodes need multiple passes?
//------------------------------------------------------------------------
int delayed_transp_lit_render( g3dSceneNode* i_pSceneNode,
								g3dRenderStateCache i_CacheState,
								const g3dRenderStateTraverser& i_StateTraverser )
{
	int num_tris = 0;

	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pSceneNode);//m_pAlphaMaskMat;
	if (pMaterial->GetBelongsToLightShaft())
		SetInitialAdditiveBlend();
	else
		SetInitialBlend();


	// once the first alpha pass is laid down, no longer blend in new alphas for this node.
	bool firstAlphaDrawn = false;

	// Ambient pass
	if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
	{
		i_StateTraverser.SetRenderState( i_CacheState,
			g3dSingleLightRendering::GetDoSingleLightRendering() );

		// ambient env map
		num_tris += g3dSceneRenderUtil::DrawNodeEnvironment(i_pSceneNode,
			i_StateTraverser.GetEnvironmentState(i_CacheState));
		firstAlphaDrawn = true;
	}

	if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
	{
		// Disable all lights and then turn them on one at a time
		g3dLightMgrVk::Implementation()->DisableAllLights();

		// Get list of lights from cache and work with them directly
		const g3dRenderState& render_state = i_StateTraverser.GetRenderState( i_CacheState );

		const int num_lights = render_state.m_Lights.size();
		for (int i=0; i<num_lights; i++)
		{
			g3dLight* pLight = render_state.m_Lights[i];
			if( !pLight->IsEnabled() )
				continue;
			if ( !pLight->GetCastsShadow() )
				continue;

			const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);
			if (g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
			{
				if (proj_light != NULL)
				{
					if (g3dSceneRenderUtil::get_box_vis(i_pSceneNode->GetWorldBox(), &proj_light->GetTotalMatrix(), true) == e_Reject)
					{
						continue;
					}
				}
			}

			// Enable the one shadow light for this pass
			g3dSingleLightRendering::SetActiveLight( pLight );
			g3dLightMgrVk::Implementation()->SetLight( pLight );
			g3dLightMgrVk::Implementation()->EnableLight( pLight );

			if( firstAlphaDrawn )	//only if we have drawn the alpha first
			{
				SetAccumulationBlend();
			}

			num_tris += g3dSceneRenderUtil::DrawNodeLit( i_pSceneNode, pLight, proj_light );
			firstAlphaDrawn = true;

			g3dLightMgrVk::Implementation()->DisableLight( pLight );
			g3dSingleLightRendering::SetActiveLight( NULL );
		}

		// Restore lights so that StateTraverser can use cache info.
		// This should restore the lights in LightMgr to the state
		// after the call to i_StateTraverser.SetRenderState( i_CacheState ) above
		for (int i=0; i<num_lights; i++)
		{
			g3dLight* pLight = render_state.m_Lights[i];
			if( pLight->IsEnabled() )
			{
				if (!g3dSingleLightRendering::GetDoSingleLightRendering() ||
					!pLight->GetCastsShadow())
				{
					g3dLightMgrVk::Implementation()->SetLight( pLight );
					g3dLightMgrVk::Implementation()->EnableLight( pLight );
				}
			}
		}
	}

	RestoreBlend();

	return num_tris;
}
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassTransparent::shdwPassTransparent()
	: m_pLayer(NULL), m_pCamera(NULL),
	m_sceneInfo(NULL),
	m_depthTarget1(NULL),
	m_depthTarget2(NULL),
	m_depthAux(NULL),
	m_SinglePeelTarget(NULL),
	m_ColorAccumulationTarget(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassTransparent::shdwPassTransparent(
	g3dLayer* i_pLayer,
	const camCamera* i_pCamera,
	matRenderTargetTexture* i_depthTarget1,
	matRenderTargetTexture* i_depthTarget2,
	matRenderTargetTexture* i_depthAux,
	matRenderTargetTexture* i_ScratchTarget,
	matRenderTargetTexture* i_ScratchTarget2,
	g2dRenderTarget* i_destination)
	: m_pLayer(i_pLayer), m_pCamera(i_pCamera),
	m_sceneInfo(NULL),
	m_depthTarget1(i_depthTarget1),
	m_depthTarget2(i_depthTarget2),
	m_depthAux(i_depthAux),
	m_SinglePeelTarget(i_ScratchTarget),
	m_ColorAccumulationTarget(i_ScratchTarget2)
{
	SetRenderTarget(i_destination);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassTransparent::~shdwPassTransparent()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassTransparent::SetBuffers(
	matRenderTargetTexture* i_depthTarget1,
	matRenderTargetTexture* i_depthTarget2,
	matRenderTargetTexture* i_depthAux,
	matRenderTargetTexture* i_ScratchTarget,
	matRenderTargetTexture* i_ScratchTarget2,
	g2dRenderTarget* i_destination)
{
	m_depthTarget1 = i_depthTarget1;
	m_depthTarget2 = i_depthTarget2;
	m_depthAux = i_depthAux;
	m_SinglePeelTarget = i_ScratchTarget;
	m_ColorAccumulationTarget = i_ScratchTarget2;
	SetRenderTarget(i_destination);
}

//--------------------------------------------------------------------------------------------------------
// This function renders barycentric sorted transparent objects
//------------Render Process----------------
//Sort nodes
//Render solid alpha (no blending, zwrite, alpha test)
//	for single pass render all nodes
//  for multipass only render ambient
//Render transparency (blending, no zwrite)
//  for single pass render all nodes
//  for multipass render each nodes ambient and accumulated lights.
//--------------------------------------------------------------------------------------------------------
int shdwPassTransparent::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassTransparent");
	m_stats.Reset();

	g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
	// trivial reject
	if (!transparencySort->HaveTransparentNodes()) return 0;

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );

	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(false);

	m_pRenderTarget->MakeCurrent();

	//sort the nodes once
	m_sceneInfo->GetTransparentNodes()->SortTransparentNodes();

	if( s_bUseSolidAlphaTestPrepass )
	{
		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dSceneGlobal::g_AlphaTestRef = 254.0f / 255.0f;	//just less than 1 for 8 bits of alpha

		m_stats.m_nTriangles += RenderTransparentNodes();

		g3dSceneGlobal::g_AlphaTestRef = 0.0f;	//disable alpha test
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Less_NS );

	m_stats.m_nTriangles += RenderTransparentNodes();

	g3dSingleLightRendering::SetDoTransparentPass(false);

	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	return m_stats.m_nTriangles;
}

//------------Reverse Render Process----------------
// Setup ping pong z buffers (each has attached depth buffer)
// Render Solid objects to auxiliary z buffer (sets up nearest depths)
//   Note that the depth will be cleared to the farthest values. (zfar)
// Render Transparent opaque to auxiliary z buffer (no blending, zwrite, only writes depths of opaque parts)
//	 for single pass render all nodes
//   for multipass only render ambient
// Render Peeled Color
// Swap Depth Targets (we now use original depths as source to compare against)
// For each Depth Peeled Layer
//   Copy auxiliary z buffer to active depths (restores depth buffer with re-rendering solids)
//	 Render Transparent to active z buffer (no blending, zwrite, only writes depths less than source)
//     This sets depths within the current depth buffer to a single depth value that
//     are farther than the previous depth (source) but closer than the current depth (depth buffer).
//     in order for this to work, the destination depth and z must be cleared to zfar on each pass.
//   Render Reversed Peeled Colors to Accumulation target
//   Swap Depth Targets
//   Composite accumulated transparencies with render target
//--------------------------------------------------------------------------------------------------------
int shdwPassTransparent::RenderDepthPeeled(float i_time )
{
	m_stats.Reset();
	matRenderTargetTexture* pDstDepth = NULL;
	matRenderTargetTexture* pSrcDepth = NULL;

	g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
	// trivial reject
	if (!transparencySort->HaveTransparentNodes()) return 0;

	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(false);

	//force depth peeled transparency to have no back face culling
	g3dVkUtil::SetCullEnable( s_bCullDepthPeel );

	//render all solid into nearest depth map
	//render depths
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	float Far = m_pCamera->GetFarClip();

	//assign initial ping pong buffers
	SwapDepthTargets( pDstDepth, pSrcDepth );

	//clear the destination depth buffer since we'll be rendering to the aux depths
	pDstDepth->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );

	m_ColorAccumulationTarget->Clear(maFloatRGBA(0,0,0,1));

	//clear Active Depth Buffer
	m_depthAux->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );

	//draw solid depths
	m_pRenderTarget->MakeCurrent();
	g3dSceneGlobal::g_AlphaTestRef = 254.0f/255.0f;	//only let solid part through
	m_stats.m_nTriangles += RenderTransparentNodes();

	//initialize depths with solid objects
	// Use input colorPeeledFunc to determine if it needs to render depth of invisible object
	shdwPassDepth depthPass( m_depthAux, m_pCamera, false, NULL, shdwPassDepth::eDefault );
	depthPass.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPass.Render( i_time );
	g3dSceneGlobal::g_AlphaTestRef = 0.0f;

	//---------Test for transparent pass
	//set solid depths
	g3dVkUtil::CopyTextureToDepth( m_depthAux, pDstDepth );
	//render transparent depths
	shdwPassDepth depthPassTrans( pDstDepth, m_pCamera, false, NULL, shdwPassDepth::eScreenAlpha );
	depthPassTrans.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPassTrans.RenderTransparent();

	m_stats.m_nTriangles += RenderPeeledRColors();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dSceneGlobal::g_AlphaTestRef = 254.0f/255.0f;	//only let solid part through
	shdwPassDepth depthPassZ( m_depthAux, m_pCamera, false, NULL, shdwPassDepth::eScreenBiased );
	depthPassZ.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPassZ.RenderTransparent();
	g3dSceneGlobal::g_AlphaTestRef = 0.0f;

	SwapDepthTargets( pDstDepth, pSrcDepth );

	uint64_t numberOfPixelsDrawn = 1;	//set initial to enter loop
	unsigned int maxPasses = MAX_TRANSPARENT_DEPTH_PEEL_PASSES;
	unsigned int counter = 0;

	if( g3dPrefs::CurrentPrefs().m_bDebugDepthPeel )
	{
		maxPasses = g3dPrefs::CurrentPrefs().m_nDebugDepthPeelLayers;
	}

	vkdOcclusionQuery occlusionQuery( vkdGlobal::Device() );

	while( numberOfPixelsDrawn > 0 && counter++ < maxPasses )
	{
		pDstDepth->Clear( maFloatRGBA(1,1,1,1) );

		g3dVkUtil::CopyTextureToDepth( m_depthAux, pDstDepth );

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_Less_NS );

		// Add an begin marker to the command buffer queue.
		occlusionQuery.Begin( vkdGlobal::Context() );

		//render transparent depths and peel from farthest to nearest, sets current depths to be peeled
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, shdwPassDepth::eScreenPeelGreater );
		depthPass.SetSceneInfo( m_sceneInfo );
		m_stats.m_nTriangles += depthPass.RenderTransparent();

		// Add an end marker to the command buffer queue.
		occlusionQuery.End( vkdGlobal::Context() );

		// Force the driver to execute the commands from the command buffer.
		// Empty the command buffer and wait until the GPU is idle.
		numberOfPixelsDrawn = occlusionQuery.GetResult( vkdGlobal::Context() );

		//process all passes unless in single peel mode (which is only done for the last pass)
		if( numberOfPixelsDrawn > 0 && (!g3dPrefs::CurrentPrefs().m_bDebugSinglePeel || counter == maxPasses) )
		{
			m_stats.m_nTriangles += RenderPeeledRColors();
		}

		//swap depth targets
		SwapDepthTargets( pDstDepth, pSrcDepth );
	}

	//finally composite accumulated transparency with target
	//only copy result back when there's no input render function,
	//if a render function is passed in then the caller need to handle the compositing itself
	CompositeTextureInverse( m_ColorAccumulationTarget, m_pRenderTarget );

	g3dVkUtil::SetCullEnable( true );
	g3dSingleLightRendering::SetDoTransparentPass(false);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------------------------------------------
// This function Renders the color values of objects that satisfy the
//   current depth value (equals) and blends that with the HDR Target in reversed order
//------------Render Process----------------
// Clear the temp target
// Render Transparent Objects to temp target (No Blending, Equal Depth testing, No Z Writes)
// Blend temp under render target
//--------------------------------------------------------------------------------------------------------
int shdwPassTransparent::RenderPeeledRColors()
{
	// target should have no depth buf. depths inherited from prior current depth tgt.
	//reset target
	m_SinglePeelTarget->Clear(maFloatRGBA(0,0,0,0));
	m_SinglePeelTarget->MakeCurrent();
	//save and set states

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Equal_NS );

	g3dBlendStateMgr::BlendState st_SavedBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_SavedBlend );

	int tris = RenderTransparentNodes();	//only render matching depths at peeled layer (equals)

	//composite transparency layer with target
	SetCompositeBlend();
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dVkUtil::CopyTexToTarget( m_SinglePeelTarget, m_ColorAccumulationTarget ); //(copy needed to overcome MSAA sampling limitation)

	//restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dBlendStateMgr::SetBlendState( &st_SavedBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	return tris;
}

//composite overlays the source on top of the dest using the inverse of the src
//The shader inverts the alpha so that the alpha channel can blend inversely
//C =    Sc  + Dc * (1-Sa)
//A = (1-Sa) + Da
void shdwPassTransparent::CompositeTextureInverse(matTexture* src, g2dRenderTarget* tgt)
{
	uint32_t uiPass;

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
	int technique = pEffect->FindTechnique("SimpleCopyInvAlpha");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions( w, h);

	g3dBlendStateMgr::SetBlendState( st_CopyTarget );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for (uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);

		// alpha blending/z writing control by callar
		vkdImage* aRes[1] = { g3dVkTextureUtil::GetImage(src) };
		vkdGlobal::Context().SetShaderResources(0, 1, aRes);

		g3dVkUtil::DrawFullScreenQuad( w, h );
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	vkdImage* nullTex[1] = { NULL };
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);
}


int shdwPassTransparent::RenderDeferred(float i_time)
{
	int nTris = 0;

	g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();

	// Render the transparent nodes
	if (g3dSingleLightRendering::GetDoSingleLightRendering())
	{
		// Sort once now, multiple passes later
		transparencySort->SortTransparentNodes();

		// set up for ambient pass as default.
		// multiple passes will need to change and then restore this
		g3dSceneRenderUtil::enable_lights();

		// for trivial rejection, check to see if any lights cast shadows
		l_bHaveShadowCastingLights = false;
		const std::vector<g3dLight*>& lights = g3dLightMgrVk::Implementation()->GetLights( );
		const int num_lights = lights.size();
		for (int i=0; i<num_lights; i++)
		{
			g3dLight* pLight = lights[i];
			if (pLight->IsEnabled() && pLight->GetCastsShadow())
			{
				l_bHaveShadowCastingLights = true;
				break;
			}
		}

		// Call user function on each node after sorting.
		// If the object receives shadows, then multiple passes
		// will be done on that node by itself.
		nTris += transparencySort->RenderUserFuncDeferred( delayed_transp_lit_render, *(m_sceneInfo->GetRenderStateCache()) );
	}
	return nTris;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassTransparent::RenderTransparentNodes()
{
	int nTris = 0;

	g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();

	// Render the transparent nodes
	if (g3dSingleLightRendering::GetDoSingleLightRendering())
	{

		// set up for ambient pass as default.
		// multiple passes will need to change and then restore this
		g3dSceneRenderUtil::enable_lights();

		// Call user function on each node after sorting.
		// If the object receives shadows, then multiple passes
		// will be done on that node by itself.
		nTris += transparencySort->RenderUserFunc( delayed_transp_lit_render, delayed_transp_lit_render, *(m_sceneInfo->GetRenderStateCache()) );
	}
	else
	{
		SetMultiRenderBlend();
		nTris += transparencySort->RenderTransparentNodes(*(m_sceneInfo->GetRenderStateCache()) );
	}
	return nTris;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassTransparent::SwapDepthTargets( matRenderTargetTexture*& io_pDst, matRenderTargetTexture*& io_pSrc )
{
	static bool bToggle = false;	//ping pong flag

	if( io_pDst == NULL && io_pSrc == NULL )	//initialize
	{
		io_pDst = m_depthTarget1;
		io_pSrc = m_depthTarget2;
		bToggle = false;
	}
	else	//swap
	{
		//swap depth targets
		io_pSrc = bToggle ? m_depthTarget2 : m_depthTarget1;
		io_pDst = bToggle ? m_depthTarget1 : m_depthTarget2;
		bToggle = !bToggle;
	}

}

//static
void shdwPassTransparent::InitStates()
{
	VkColorComponentFlags EN_WRITE_COLOR = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
	VkColorComponentFlags EN_WRITE_ALL = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	st_Initial = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_Accumulate = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRender = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_CompositeRev = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_DST_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_RenderInit = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_CopyTarget = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

//static
void shdwPassTransparent::CleanupStates()
{
	delete st_Initial; st_Initial = NULL;
	delete st_Accumulate; st_Accumulate = NULL;
	delete st_MultiRender; st_MultiRender = NULL;
	delete st_CompositeRev; st_CompositeRev = NULL;
	delete st_RenderInit; st_RenderInit = NULL;
	delete st_CopyTarget; st_CopyTarget = NULL;
	delete ds_Test_Write_LessE_NS; ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_Write_Less_NS; ds_Test_Write_Less_NS = NULL;
	delete ds_Test_Less_NS; ds_Test_Less_NS = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
	delete ds_Test_Equal_NS; ds_Test_Equal_NS = NULL;
}

}
