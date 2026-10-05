/****************************************************************************\
**	shdwPassHair.cpp
**
**		Render pass to draw hair
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassHair.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/Eff/effStrandHair.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/G2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/G2d/g2dRenderTargetDX11.hpp"
#include "GraphicsDX11/Mat/matVolumeTexture.hpp"

//Note that Alpha test pass cannot work with depth peeling (so currently bypassed)
//depth peeling requires the exact depths at each layer, pre passing causes holes in the solid parts.
static bool s_bUseSolidAlphaTestPrepass = false;

static unsigned int s_alphaTestRef = 0x000000ff;

#define MAX_TRANSPARENT_DEPTH_PEEL_PASSES 254

namespace 
{
	bool l_sSaveOpacity = true;

	matRenderTargetTexture* l_pColor = NULL;	//hair main target color
	g2dRenderTargetDX11* l_pColorAA = NULL;	//hair main target color with MSAA
	g2dWindowDX11* l_pDepth = NULL;	//hair main target depth
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

	ID3D11Texture3D* l_pOpacityTexture3D = NULL;
	ID3D11UnorderedAccessView* l_pOpacityView = NULL;
	ID3D11ShaderResourceView* l_pOpacityResource = NULL;

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

		//	g3dDrawStyleUtilDX11::SetDrawStyle(i_pSceneNode->GetDrawStyle());

		// resolve material/effect
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pSceneNode);
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
		//	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
		//	ID3DXEffect* pD3DEffect = i_pEffect->GetFxEffect();

		//	effStrandHair* i_pHairEffect = dynamic_cast<effStrandHair*>(pEffect);
		effStrandHairData& HairData = pMaterial->StrandHairData();
		HairData.Default();

		int W, H;
		if( l_pColorAA ) l_pColorAA->GetDimensions( W, H );
		else l_pColor->GetDimensions( W, H );
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
			g3dLightMgrDX11::Implementation()->DisableAllLights();

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

					matVolumeTexture* pOpacityVol = const_cast<matVolumeTexture*>(dynamic_cast<const matVolumeTexture*>(proj_light->GetOpacityVolume()));
					if( pOpacityVol )
					{
						effStrandHair::SetVolumeTexture( pOpacityVol->GetSurface() );
					}
				}

				D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::hair_shadow_lit_render - Lit Light Pass" );

				// Enable the one shadow light for this pass
				g3dSingleLightRendering::SetActiveLight( pLight );
				g3dLightMgrDX11::Implementation()->SetLight( pLight );
				g3dLightMgrDX11::Implementation()->EnableLight( pLight );

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

				g3dLightMgrDX11::Implementation()->DisableLight( pLight );
				g3dSingleLightRendering::SetActiveLight( NULL );
				g3dSingleLightRendering::SetFirstLight( false );
				D3DPERF_EndEvent();
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
						g3dLightMgrDX11::Implementation()->SetLight( pLight );
						g3dLightMgrDX11::Implementation()->EnableLight( pLight );
					}
				}
			}
		}

		//reset material back to default
		HairData.Default();

		g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

		RestoreBlend();

		return num_tris;
	}

};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassHair::shdwPassHair()
	: m_pCamera(NULL),
	m_sceneInfo(NULL),
	m_ZBuffer( NULL ),
	m_pDepth( NULL ),
	m_depthTarget1( NULL ),
	m_depthTarget2( NULL ),
	m_depthAux( NULL ),
	m_ScratchTarget1( NULL ),
	m_ScratchTarget2( NULL )
{
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
	: m_pCamera(i_pCamera),
	m_sceneInfo(NULL),
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

	//hack to acquire actual targets
	if( g3dPrefs::CurrentPrefs().m_bHDRAA )
	{
		l_pColorAA = dynamic_cast<g2dRenderTargetDX11*>(i_destination);
		l_pColor = NULL;
	}
	else
	{
		l_pColorAA = NULL;
		l_pColor = dynamic_cast<matRenderTargetTexture*>(i_destination);
	}
	l_pDepth = dynamic_cast<g2dWindowDX11*>(i_ZBuffer);
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
	m_stats.Reset();

	g3dTransparencySortDX11* hairSort = m_sceneInfo->GetHairNodes();
	// trivial reject
	if (!hairSort->HaveTransparentNodes()) return 0;

	//disable culling for hair
	g3dDX11Util::SetCullEnable( false );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::Render" );

	//save states
	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dDepthStencilStateMgr::DepthStencilState ds_Capture;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Capture );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(false);

	l_pCamera = m_pCamera;

	matRenderTargetTexture* depth = NULL;
	if( g3dPrefs::CurrentPrefs().m_HairShadowType >= HAIR_SHADOW_DOSM4 &&
		g3dPrefs::CurrentPrefs().m_HairShadowType <= HAIR_SHADOW_DOSM32 )
	{
		depth = m_pDepth;
	}

	m_pRenderTarget->MakeCurrent();
	l_pOSMTextures[0] = m_pTargets[0];
	l_pOSMTextures[1] = m_pTargets[1];
	l_pOSMTextures[2] = m_pTargets[2];
	l_pOSMTextures[3] = m_pTargets[3];

	//sort the nodes once
	m_sceneInfo->GetTransparentNodes()->SortTransparentNodes();

	if( s_bUseSolidAlphaTestPrepass )
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::Render - Alpha Z Prepass" );

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
		D3DPERF_EndEvent();
	}

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::Render - Blend Pass" );

	//write depth if in solid or approx mode
	g3dDepthStencilStateMgr::SetDepthStencilState( 2 == g3dPrefs::CurrentPrefs().m_HairTransparencyMode ? ds_Test_Less_NS : ds_Test_Write_LessE_NS );

	m_stats.m_nTriangles += RenderHairNodes( false );

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Capture );

	D3DPERF_EndEvent();

	g3dSingleLightRendering::SetDoTransparentPass(false);

	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(true);

	//re-enable culling
	g3dDX11Util::SetCullEnable( true );

	l_pCamera = NULL;
	l_pOSMTextures[0] = NULL;
	l_pOSMTextures[1] = NULL;
	l_pOSMTextures[2] = NULL;
	l_pOSMTextures[3] = NULL;

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassHair::RenderDepthPeeled(float i_time )
{
	m_stats.Reset();
	matRenderTargetTexture* pDstDepth = NULL;
	matRenderTargetTexture* pSrcDepth = NULL;

	g3dTransparencySortDX11* hairSort = m_sceneInfo->GetHairNodes();
	// trivial reject
	if (!hairSort->HaveTransparentNodes()) return 0;

	//disable culling for hair
	g3dDX11Util::SetCullEnable( false );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::RenderDepthPeeled" );

	//save states
	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dDepthStencilStateMgr::DepthStencilState ds_Capture;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Capture );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(false);

	l_pCamera = m_pCamera;

	matRenderTargetTexture* depth = NULL;
	if( g3dPrefs::CurrentPrefs().m_HairShadowType >= HAIR_SHADOW_DOSM4 &&
		g3dPrefs::CurrentPrefs().m_HairShadowType <= HAIR_SHADOW_DOSM32 )
	{
		depth = m_pDepth;
	}

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
	g3dDX11Util::CopyTextureToDepth( m_depthAux, pDstDepth );
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

	D3D11_QUERY_DESC QueryDesc;

	g2dD3D11QueryPtr pOcclusionQuery;
	UINT64 numberOfPixelsDrawn = 1;	//set initial to enter loop
	UINT64 maxPasses = MAX_TRANSPARENT_DEPTH_PEEL_PASSES;
	int counter = 0;

	if( g3dPrefs::CurrentPrefs().m_bHairDepthPeel )
	{
		maxPasses = g3dPrefs::CurrentPrefs().m_nHairDepthPeelLayers;
	}

	QueryDesc.Query = D3D11_QUERY_OCCLUSION;
	QueryDesc.MiscFlags = 0;

	g2dDX11Global::g_pDevice->CreateQuery( &QueryDesc, &pOcclusionQuery);

	while( numberOfPixelsDrawn > 0 && counter++ < maxPasses )
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::RenderDepthPeeled - Peel Pass" );

		pDstDepth->Clear( maFloatRGBA(1,1,1,1), true, 1.0f, false );

		g3dDX11Util::CopyTextureToDepth( m_depthAux, pDstDepth );

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g2dDX11Global::g_pDeviceContext->Begin( pOcclusionQuery );

		//render transparent depths and peel from farthest to nearest, sets current depths to be peeled
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, shdwPassDepth::eScreenPeelGreater );
		depthPass.SetSceneInfo( m_sceneInfo );
		m_stats.m_nTriangles += depthPass.RenderHair();

		// Add an end marker to the command buffer queue.
		g2dDX11Global::g_pDeviceContext->End( pOcclusionQuery );

		// Force the driver to execute the commands from the command buffer.
		// Empty the command buffer and wait until the GPU is idle.
		while( S_OK != g2dDX11Global::g_pDeviceContext->GetData( pOcclusionQuery, &numberOfPixelsDrawn, sizeof(UINT64), 0 ));

		//process all passes unless in single peel mode (which is only done for the last pass)
		if( numberOfPixelsDrawn > 0 )
		{
			m_stats.m_nTriangles += RenderPeeledRColors();
		}

		//swap depth targets
		SwapDepthTargets( pDstDepth, pSrcDepth );
		D3DPERF_EndEvent();
	}

	pOcclusionQuery->Release();

	//finally composite accumulated transparency with target
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dBlendStateMgr::SetBlendState( st_CopyTarget );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDX11Util::CopyTexToTarget( m_ScratchTarget2, m_pRenderTarget ); //(copy needed to overcome MSAA sampling limitation)

	g3dDX11Util::SetCullEnable( true );
	g3dSingleLightRendering::SetDoTransparentPass(false);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Capture );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassHair::RenderHairNodes( bool i_bPeeled )
{
	int nverts = 0;
	g3dTransparencySortDX11* hairSort = m_sceneInfo->GetHairNodes();

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
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassHair::RenderPeeledRColors" );

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

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dDX11Util::CopyTexToTarget( m_ScratchTarget1, m_ScratchTarget2 ); //(copy needed to overcome MSAA sampling limitation)

	//restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dBlendStateMgr::SetBlendState( &st_SavedBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DPERF_EndEvent();

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

void shdwPassHair::InitStates()
{
	const DWORD EN_WRITE_COLOR = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	st_InitialAA = new g3dBlendStateMgr::BlendState( true, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_InitialSolidAA = new g3dBlendStateMgr::BlendState( true, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_AccumulateAA = new g3dBlendStateMgr::BlendState( true, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRenderAA = new g3dBlendStateMgr::BlendState( true, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Initial = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_InitialSolid = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Accumulate = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRender = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CompositeFor = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CompositeRev = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_DEST_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_RenderInit = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CopyTarget = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	//temp to manage a volume texture

	D3D11_TEXTURE3D_DESC desc;

	desc.Width = 256;
	desc.Height = 256;
	desc.Depth = 4;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = g2dDX11Global::g_pDevice->CreateTexture3D( &desc, NULL, &l_pOpacityTexture3D );

	//create the unordered access views
	D3D11_UNORDERED_ACCESS_VIEW_DESC vdesc;

	vdesc.Format = desc.Format;
	vdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	vdesc.Texture3D.MipSlice = 0;
	vdesc.Texture3D.FirstWSlice = 0;
	vdesc.Texture3D.WSize = desc.Depth;

	hr = g2dDX11Global::g_pDevice->CreateUnorderedAccessView( l_pOpacityTexture3D, &vdesc, &l_pOpacityView );

	//create the shader resource views
	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;

	sdesc.Format = desc.Format;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	sdesc.Texture3D.MostDetailedMip = 0;
	sdesc.Texture3D.MipLevels = 1;

	hr = g2dDX11Global::g_pDevice->CreateShaderResourceView( l_pOpacityTexture3D, &sdesc, &l_pOpacityResource );
}

void shdwPassHair::CleanupStates()
{
	SAFE_RELEASE( l_pOpacityResource );
	SAFE_RELEASE( l_pOpacityView );
	SAFE_RELEASE( l_pOpacityTexture3D );

	SAFE_DELETE( st_InitialAA );
	SAFE_DELETE( st_InitialSolidAA );
	SAFE_DELETE( st_AccumulateAA );
	SAFE_DELETE( st_MultiRenderAA );
	SAFE_DELETE( st_Initial );
	SAFE_DELETE( st_InitialSolid );
	SAFE_DELETE( st_Accumulate );
	SAFE_DELETE( st_MultiRender );
	SAFE_DELETE( st_CompositeFor );
	SAFE_DELETE( st_CompositeRev );
	SAFE_DELETE( st_RenderInit );
	SAFE_DELETE( st_CopyTarget );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_Less_NS );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Equal_NS );
}