/****************************************************************************\
**	shdwPassHair.cpp
**
**		Render pass to draw hair
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSHAIR_HPP
#include "GraphicsVulkan/shdw/shdwPassHair.hpp"
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
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/Eff/effStrandHairData.hpp"
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
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_MATVOLUMETEXTURE_HPP
#include "GraphicsVulkan/Mat/matVolumeTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_EFF_STRANDHAIR_HPP
#include "GraphicsVulkan/Eff/effStrandHair.hpp"
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

//Note that Alpha test pass cannot work with depth peeling (so currently bypassed)
//depth peeling requires the exact depths at each layer, pre passing causes holes in the solid parts.
static bool s_bUseSolidAlphaTestPrepass = false;

#define MAX_TRANSPARENT_DEPTH_PEEL_PASSES 254

namespace
{
	g2dRenderTarget* l_pColor = NULL;	//hair main target color (with MSAA if HDR AA is on)
	matRenderTargetTexture*	l_pOSMTextures[4] = {NULL};
	const camCamera *l_pCamera = NULL;

	g3dBlendStateMgr::BlendState st_OldBlend;

	g3dBlendStateMgr::BlendState* st_InitialAA = NULL;
	g3dBlendStateMgr::BlendState* st_InitialSolidAA = NULL;
	g3dBlendStateMgr::BlendState* st_AccumulateAA = NULL;
	g3dBlendStateMgr::BlendState* st_MultiRenderAA = NULL;
	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_InitialSolid = NULL;
	g3dBlendStateMgr::BlendState* st_Accumulate = NULL;
	g3dBlendStateMgr::BlendState* st_MultiRender = NULL;
	g3dBlendStateMgr::BlendState* st_CompositeFor = NULL;
	g3dBlendStateMgr::BlendState* st_CompositeRev = NULL;
	g3dBlendStateMgr::BlendState* st_RenderInit = NULL;
	g3dBlendStateMgr::BlendState* st_CopyTarget = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Equal_NS = NULL;

	//setup local blending functions for simplicity
	void SetInitialBlend( bool i_bSolid )
	{
		g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

		if( g3dPrefs::CurrentPrefs().m_bHDRAA )
		{
			g3dBlendStateMgr::SetBlendState( i_bSolid ? st_InitialSolidAA : st_InitialAA );
		}
		else
		{
			g3dBlendStateMgr::SetBlendState( i_bSolid ? st_InitialSolid : st_Initial );
		}
	}

	void SetAccumulationBlend()
	{
		g3dBlendStateMgr::SetBlendState( g3dPrefs::CurrentPrefs().m_bHDRAA ? st_AccumulateAA : st_Accumulate );
	}

	//sets the blend mode for rendering multilights
	void SetMultiRenderBlend()
	{
		g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

		g3dBlendStateMgr::SetBlendState( g3dPrefs::CurrentPrefs().m_bHDRAA ? st_MultiRenderAA : st_MultiRender );
	}

	//sets the blend mode used for compositing the peeled transparent layer with the scene
	void SetCompositeBlend()
	{
		g3dBlendStateMgr::SetBlendState( st_CompositeRev );
	}

	void RestoreBlend()
	{
		g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	}

	bool CullLight( g3dLight* pLight, const maAxisBox& NodeBounds )
	{
		if( !pLight->IsEnabled() ) return true;
		if ( !pLight->GetCastsShadow() ) return true;

		//check if outside light frustum
		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);
		if( proj_light )
		{
			if( !g3dPrefs::CurrentPrefs().m_bProjLightsOn ) return true;
			if (g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull )
			{
				if (g3dSceneRenderUtil::get_box_vis( NodeBounds, &proj_light->GetTotalMatrix(), true) == e_Reject)
				{
					return true;
				}
			}
		}
		else if( !g3dPrefs::CurrentPrefs().m_bPtLightsOn ) return true;
		return false;
	}
//------------------------------------------------------------------------
// rendering a single transparent node in a multi-pass
// configuration, some nodes need multiple passes?
//------------------------------------------------------------------------
	int hair_shadow_lit_render( g3dSceneNode* i_pSceneNode,
		g3dRenderStateCache i_CacheState,
		const g3dRenderStateTraverser& i_StateTraverser )
	{
		int num_tris = 0;

		SetInitialBlend( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode );

		// once the first alpha pass is laid down, no longer blend in new alphas for this node.
		bool firstAlphaDrawn = false;

		// resolve material/effect
		const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pSceneNode);

		effStrandHairData& HairData = pMaterial->StrandHairData();
		HairData.Default();

		int W, H;
		l_pColor->GetDimensions( W, H );
		HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

		if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
		{
			HairData.m_SubPixelPower = 1000000.0f;
		}
		else
		{
			HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
		}

		//check for number of contributing lights
		int nContribLights = 0;

		// Get list of lights from cache and work with them directly
		const g3dRenderState& render_state = i_StateTraverser.GetRenderState( i_CacheState );

		const int num_lights = render_state.m_Lights.size();
		for (int i=0; i<num_lights; i++)
		{
			if( CullLight( render_state.m_Lights[i], i_pSceneNode->GetWorldBox() ) ) continue;
			nContribLights++;
		}

		// Environment pass
		//only do env pass if we aren't applying it with the first light
		bool bLit = g3dPrefs::CurrentPrefs().m_bEnableLitPass;
		if( g3dPrefs::CurrentPrefs().m_bEnableEnvironment &&
			(!bLit || (bLit && !nContribLights )) )
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

			const int num_lights = render_state.m_Lights.size();
			for (int i=0; i<num_lights; i++)
			{
				g3dLight* pLight = render_state.m_Lights[i];
				if( CullLight( pLight, i_pSceneNode->GetWorldBox() ) ) continue;
				const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);

				//set projected light info
				if( proj_light )
				{
					HairData.m_ZNear = proj_light->GetHairMinBound();
					HairData.m_ZFar = proj_light->GetHairMaxBound();

					maMatrix4x4 vmat = proj_light->GetCameraMatrix();
					vmat.Transpose();
					HairData.m_LightViewPlane = maVector4d( &vmat.m_Mat[8] );//access the 3rd row

					for( int i = 0; i < 8; i++ )
					{
						HairData.m_pOSM[i] = const_cast<matTexture*>(proj_light->GetOpacityShadowMap(i));
					}

					if( g3dPrefs::CurrentPrefs().m_HairShadowType >= HAIR_SHADOW_DOSM4 &&
						g3dPrefs::CurrentPrefs().m_HairShadowType <= HAIR_SHADOW_DOSM32 )
					{
						HairData.m_pDepthTexture = const_cast<matTexture*>(proj_light->GetShadowMap());
					}

					const matVolumeTexture* pOpacityVol = dynamic_cast<const matVolumeTexture*>(proj_light->GetOpacityVolume());
					if( pOpacityVol )
					{
						effStrandHair::SetVolumeTexture( pOpacityVol->GetImage() );
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
				else
				{
					g3dSingleLightRendering::SetFirstLight( g3dPrefs::CurrentPrefs().m_bEnableEnvironment );
				}

				num_tris += g3dSceneRenderUtil::DrawNodeLit( i_pSceneNode, pLight, proj_light, &i_StateTraverser.GetEnvironmentState(i_CacheState) );
				firstAlphaDrawn = true;

				effStrandHair::SetVolumeTexture( NULL );

				g3dLightMgrVk::Implementation()->DisableLight( pLight );
				g3dSingleLightRendering::SetActiveLight( NULL );
				g3dSingleLightRendering::SetFirstLight( false );
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

		//reset material back to default
		HairData.Default();

		g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

		RestoreBlend();

		return num_tris;
	}

};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassHair::shdwPassHair()
	: m_sceneInfo(NULL),
	m_pCamera(NULL),
	m_ZBuffer( NULL ),
	m_pDepth( NULL ),
	m_depthTarget1( NULL ),
	m_depthTarget2( NULL ),
	m_depthAux( NULL ),
	m_ScratchTarget1( NULL ),
	m_ScratchTarget2( NULL )
{
	m_pTargets[0] = m_pTargets[1] = m_pTargets[2] = m_pTargets[3] = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassHair::shdwPassHair(
	const camCamera* i_pCamera,
	matRenderTargetTexture* (&i_Targets)[4],
	g2dRenderTarget* i_destination,
	g2dRenderTarget* i_ZBuffer,
	matRenderTargetTexture* i_Depth,
	matRenderTargetTexture* i_depthTarget1,
	matRenderTargetTexture* i_depthTarget2,
	matRenderTargetTexture* i_depthAux,
	matRenderTargetTexture* i_ScratchTarget1,
	matRenderTargetTexture* i_ScratchTarget2)
	: m_sceneInfo(NULL),
	m_pCamera(i_pCamera),
	m_ZBuffer( i_ZBuffer ),
	m_pDepth( i_Depth ),
	m_depthTarget1( i_depthTarget1 ),
	m_depthTarget2( i_depthTarget2 ),
	m_depthAux( i_depthAux ),
	m_ScratchTarget1( i_ScratchTarget1 ),
	m_ScratchTarget2( i_ScratchTarget2 )
{

	SetRenderTarget(i_destination);

	m_pTargets[0] = i_Targets[0];
	m_pTargets[1] = i_Targets[1];
	m_pTargets[2] = i_Targets[2];
	m_pTargets[3] = i_Targets[3];

	l_pColor = i_destination;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassHair::~shdwPassHair()
{
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
int shdwPassHair::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassHair");
	m_stats.Reset();

	g3dTransparencySortVk* hairSort = m_sceneInfo->GetHairNodes();
	// trivial reject
	if (!hairSort->HaveTransparentNodes()) return 0;

	//disable culling for hair
	g3dVkUtil::SetCullEnable( false );

	//save states
	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dDepthStencilStateMgr::DepthStencilState ds_Capture;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Capture );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(false);

	l_pCamera = m_pCamera;

	m_pRenderTarget->MakeCurrent();
	l_pOSMTextures[0] = m_pTargets[0];
	l_pOSMTextures[1] = m_pTargets[1];
	l_pOSMTextures[2] = m_pTargets[2];
	l_pOSMTextures[3] = m_pTargets[3];

	//sort the nodes once
	m_sceneInfo->GetTransparentNodes()->SortTransparentNodes();

	if( s_bUseSolidAlphaTestPrepass )
	{
		if (!g3dSingleLightRendering::GetDoDOFPrepPass())
		{
			if( 1 == g3dPrefs::CurrentPrefs().m_HairTransparencyMode)
			{
				g3dSceneGlobal::g_AlphaTestRef = 254.0f/255.0f;
			}
			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
		}
		m_stats.m_nTriangles += RenderHairNodes( false );
		g3dSceneGlobal::g_AlphaTestRef = 0.0f;
	}

	//write depth if in solid or approx mode
	g3dDepthStencilStateMgr::SetDepthStencilState( 2 == g3dPrefs::CurrentPrefs().m_HairTransparencyMode ? ds_Test_Less_NS : ds_Test_Write_LessE_NS );

	m_stats.m_nTriangles += RenderHairNodes( false );

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Capture );

	g3dSingleLightRendering::SetDoTransparentPass(false);

	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(true);

	//re-enable culling
	g3dVkUtil::SetCullEnable( true );

	l_pCamera = NULL;
	l_pOSMTextures[0] = NULL;
	l_pOSMTextures[1] = NULL;
	l_pOSMTextures[2] = NULL;
	l_pOSMTextures[3] = NULL;

	return m_stats.m_nTriangles;
}

int shdwPassHair::RenderDepthPeeled(float i_time )
{
	G3D_PROFILE_SCOPE("PassHair");
	m_stats.Reset();
	matRenderTargetTexture* pDstDepth = NULL;
	matRenderTargetTexture* pSrcDepth = NULL;

	g3dTransparencySortVk* hairSort = m_sceneInfo->GetHairNodes();
	// trivial reject
	if (!hairSort->HaveTransparentNodes()) return 0;

	//disable culling for hair
	g3dVkUtil::SetCullEnable( false );

	//save states
	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dDepthStencilStateMgr::DepthStencilState ds_Capture;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Capture );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(false);

	l_pCamera = m_pCamera;

	m_pRenderTarget->MakeCurrent();
	l_pOSMTextures[0] = m_pTargets[0];
	l_pOSMTextures[1] = m_pTargets[1];
	l_pOSMTextures[2] = m_pTargets[2];
	l_pOSMTextures[3] = m_pTargets[3];

	//render all solid into nearest depth map
	//render depths
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	//assign initial ping pong buffers
	SwapDepthTargets( pDstDepth, pSrcDepth );

	//clear the destination depth buffer since we'll be rendering to the aux depths
	pDstDepth->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );
	m_ScratchTarget2->Clear(maFloatRGBA(0,0,0,1));	//alpha must be set to 1 for reverse compositing

	//clear Aux Depth Buffer
	m_depthAux->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );

	//initialize depths with solid objects
	// Use input colorPeeledFunc to determine if it needs to render depth of invisible object
	shdwPassDepth depthPass( m_depthAux, m_pCamera, false, NULL, shdwPassDepth::eDefault );
	depthPass.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPass.Render( i_time );

	//set solid depths
	g3dVkUtil::CopyTextureToDepth( m_depthAux, pDstDepth );
	//render transparent depths
	shdwPassDepth depthPassTrans( pDstDepth, m_pCamera, false, NULL, shdwPassDepth::eScreenAlpha );
	depthPassTrans.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPassTrans.RenderHair();

	m_stats.m_nTriangles += RenderPeeledRColors();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	shdwPassDepth depthPassZ( m_depthAux, m_pCamera, false, NULL, shdwPassDepth::eScreenBiased );
	depthPassZ.SetSceneInfo( m_sceneInfo );
	m_stats.m_nTriangles += depthPassZ.RenderHair();

	SwapDepthTargets( pDstDepth, pSrcDepth );

	uint64_t numberOfPixelsDrawn = 1;	//set initial to enter loop
	uint64_t maxPasses = MAX_TRANSPARENT_DEPTH_PEEL_PASSES;
	uint64_t counter = 0;

	if( g3dPrefs::CurrentPrefs().m_bHairDepthPeel )
	{
		maxPasses = g3dPrefs::CurrentPrefs().m_nHairDepthPeelLayers;
	}

	vkdOcclusionQuery occlusionQuery( vkdGlobal::Device() );

	while( numberOfPixelsDrawn > 0 && counter++ < maxPasses )
	{
		pDstDepth->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );

		g3dVkUtil::CopyTextureToDepth( m_depthAux, pDstDepth );

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		occlusionQuery.Begin( vkdGlobal::Context() );

		//render transparent depths and peel from farthest to nearest, sets current depths to be peeled
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, shdwPassDepth::eScreenPeelGreater );
		depthPass.SetSceneInfo( m_sceneInfo );
		m_stats.m_nTriangles += depthPass.RenderHair();

		occlusionQuery.End( vkdGlobal::Context() );

		// Waits for the GPU (as DX11's GetData loop)
		numberOfPixelsDrawn = occlusionQuery.GetResult( vkdGlobal::Context() );

		//process all passes unless in single peel mode (which is only done for the last pass)
		if( numberOfPixelsDrawn > 0 )
		{
			m_stats.m_nTriangles += RenderPeeledRColors();
		}

		//swap depth targets
		SwapDepthTargets( pDstDepth, pSrcDepth );
	}

	//finally composite accumulated transparency with target
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dBlendStateMgr::SetBlendState( st_CopyTarget );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dVkUtil::CopyTexToTarget( m_ScratchTarget2, m_pRenderTarget ); //(copy needed to overcome MSAA sampling limitation)

	g3dVkUtil::SetCullEnable( true );
	g3dSingleLightRendering::SetDoTransparentPass(false);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dVkUtil::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Capture );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassHair::RenderHairNodes( bool i_bPeeled )
{
	int nverts = 0;
	g3dTransparencySortVk* hairSort = m_sceneInfo->GetHairNodes();

	// Render the transparent nodes
	if (g3dSingleLightRendering::GetDoSingleLightRendering())
	{
		// set up for ambient pass as default.
		// multiple passes will need to change and then restore this
		g3dSceneRenderUtil::enable_lights();

		// Call user function on each node after sorting.
		// If the object receives shadows, then multiple passes
		// will be done on that node by itself.
		nverts += hairSort->RenderUserFunc( hair_shadow_lit_render, hair_shadow_lit_render, *(m_sceneInfo->GetRenderStateCache()) );
	}
	else
	{
		SetMultiRenderBlend();
		nverts += hairSort->RenderTransparentNodes(*(m_sceneInfo->GetRenderStateCache()) );
		RestoreBlend();
	}
	return nverts;
}

//--------------------------------------------------------------------------------------------------------
// This function Renders the color values of objects that satisfy the
//   current depth value (equals) and blends that with the HDR Target in reversed order
//------------Render Process----------------
// Clear the temp target
// Render Transparent Objects to temp target (No Blending, Equal Depth testing, No Z Writes)
// Blend temp under render target
//--------------------------------------------------------------------------------------------------------
int shdwPassHair::RenderPeeledRColors()
{
	// target should have no depth buf. depths inherited from prior current depth tgt.
	//reset target
	m_ScratchTarget1->MakeCurrent();
	m_ScratchTarget1->Clear(maFloatRGBA(0,0,0,0));	//alpha must be set to 1 for rev compositing

	//save and set render states
	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Equal_NS );

	g3dBlendStateMgr::BlendState st_SavedBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_SavedBlend );

	int tris = RenderHairNodes( true );	//only render matching depths at peeled layer (equals)

	//composite transparency layer with target
	SetCompositeBlend();

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dVkUtil::CopyTexToTarget( m_ScratchTarget1, m_ScratchTarget2 ); //(copy needed to overcome MSAA sampling limitation)

	//restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dBlendStateMgr::SetBlendState( &st_SavedBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	return tris;
}


void shdwPassHair::SwapDepthTargets( matRenderTargetTexture*& io_pDst, matRenderTargetTexture*& io_pSrc )
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

//	(DX11's InitStates also makes a 256x256x4 R8 volume with a UAV and an
//	SRV that nothing reads: the pointers are file-local and never used; not
//	ported)
void shdwPassHair::InitStates()
{
	const VkColorComponentFlags EN_WRITE_COLOR = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
	const VkColorComponentFlags EN_WRITE_ALL = EN_WRITE_COLOR | VK_COLOR_COMPONENT_A_BIT;

	st_InitialAA = new g3dBlendStateMgr::BlendState( true, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_InitialSolidAA = new g3dBlendStateMgr::BlendState( true, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_AccumulateAA = new g3dBlendStateMgr::BlendState( true, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRenderAA = new g3dBlendStateMgr::BlendState( true, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_Initial = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_InitialSolid = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_Accumulate = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRender = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_CompositeFor = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_CompositeRev = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_DST_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_RenderInit = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, EN_WRITE_ALL );
	st_CopyTarget = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD, EN_WRITE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassHair::CleanupStates()
{
	delete st_InitialAA; st_InitialAA = NULL;
	delete st_InitialSolidAA; st_InitialSolidAA = NULL;
	delete st_AccumulateAA; st_AccumulateAA = NULL;
	delete st_MultiRenderAA; st_MultiRenderAA = NULL;
	delete st_Initial; st_Initial = NULL;
	delete st_InitialSolid; st_InitialSolid = NULL;
	delete st_Accumulate; st_Accumulate = NULL;
	delete st_MultiRender; st_MultiRender = NULL;
	delete st_CompositeFor; st_CompositeFor = NULL;
	delete st_CompositeRev; st_CompositeRev = NULL;
	delete st_RenderInit; st_RenderInit = NULL;
	delete st_CopyTarget; st_CopyTarget = NULL;
	delete ds_Test_Write_LessE_NS; ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_Less_NS; ds_Test_Less_NS = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
	delete ds_Test_Equal_NS; ds_Test_Equal_NS = NULL;
}

}
