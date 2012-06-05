#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/Eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"

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

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::delayed_transp_lit_render" );

	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pSceneNode);//m_pAlphaMaskMat;
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
		g3dLightMgrDX11::Implementation()->DisableAllLights();

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
			D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::delayed_transp_lit_render - Lit Light Pass" );

			// Enable the one shadow light for this pass
			g3dSingleLightRendering::SetActiveLight( pLight );
			g3dLightMgrDX11::Implementation()->SetLight( pLight );
			g3dLightMgrDX11::Implementation()->EnableLight( pLight );

			if( firstAlphaDrawn )	//only if we have drawn the alpha first
			{
				SetAccumulationBlend();
			}

			num_tris += g3dSceneRenderUtil::DrawNodeLit( i_pSceneNode, pLight, proj_light );
			firstAlphaDrawn = true;

			g3dLightMgrDX11::Implementation()->DisableLight( pLight );
			g3dSingleLightRendering::SetActiveLight( NULL );
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

	RestoreBlend();

	D3DPERF_EndEvent();

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
	m_stats.Reset();

	g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
	// trivial reject
	if (!transparencySort->HaveTransparentNodes()) return 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::Render" );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );

	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(false);

	m_pRenderTarget->MakeCurrent();

	//sort the nodes once
	m_sceneInfo->GetTransparentNodes()->SortTransparentNodes();

	if( s_bUseSolidAlphaTestPrepass )
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::Render - Alpha Z Prepass" );
		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dSceneGlobal::g_AlphaTestRef = 254.0f / 255.0f;	//just less than 1 for 8 bits of alpha

		m_stats.m_nTriangles += RenderTransparentNodes();

		g3dSceneGlobal::g_AlphaTestRef = 0.0f;	//disable alpha test
		D3DPERF_EndEvent();
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Less_NS );

	m_stats.m_nTriangles += RenderTransparentNodes();

	g3dSingleLightRendering::SetDoTransparentPass(false);

	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	D3DPERF_EndEvent();
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

	g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
	// trivial reject
	if (!transparencySort->HaveTransparentNodes()) return 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::RenderDepthPeeled" );

	g3dBlendStateMgr::SetBlendState( st_RenderInit );

	g3dSingleLightRendering::SetDoTransparentPass(true);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(false);

	//force depth peeled transparency to have no back face culling
	g3dDX11Util::SetCullEnable( s_bCullDepthPeel );

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
	g3dDX11Util::CopyTextureToDepth( m_depthAux, pDstDepth );
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

	D3D11_QUERY_DESC QueryDesc;

	g2dD3D11QueryPtr pOcclusionQuery;
	UINT64 numberOfPixelsDrawn = 1;	//set initial to enter loop
	DWORD maxPasses = MAX_TRANSPARENT_DEPTH_PEEL_PASSES;
	int counter = 0;

	if( g3dPrefs::CurrentPrefs().m_bDebugDepthPeel )
	{
		maxPasses = g3dPrefs::CurrentPrefs().m_nDebugDepthPeelLayers;
	}

	QueryDesc.Query = D3D11_QUERY_OCCLUSION;
	QueryDesc.MiscFlags = 0;

	g2dDX11Global::g_pDevice->CreateQuery( &QueryDesc, &pOcclusionQuery);

	while( numberOfPixelsDrawn > 0 && counter++ < maxPasses )
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::RenderDepthPeeled - Peel Pass" );

		pDstDepth->Clear( maFloatRGBA(1,1,1,1) );

		g3dDX11Util::CopyTextureToDepth( m_depthAux, pDstDepth );

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_Less_NS );

		// Add an begin marker to the command buffer queue.
		g2dDX11Global::g_pDeviceContext->Begin( pOcclusionQuery );

		//render transparent depths and peel from farthest to nearest, sets current depths to be peeled
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, shdwPassDepth::eScreenPeelGreater );
		depthPass.SetSceneInfo( m_sceneInfo );
		m_stats.m_nTriangles += depthPass.RenderTransparent();

		// Add an end marker to the command buffer queue.
		g2dDX11Global::g_pDeviceContext->End( pOcclusionQuery );

		// Force the driver to execute the commands from the command buffer.
		// Empty the command buffer and wait until the GPU is idle.
		while( S_OK != g2dDX11Global::g_pDeviceContext->GetData( pOcclusionQuery, &numberOfPixelsDrawn, sizeof(UINT64), 0 ));

		//process all passes unless in single peel mode (which is only done for the last pass)
		if( numberOfPixelsDrawn > 0 && (!g3dPrefs::CurrentPrefs().m_bDebugSinglePeel || counter == maxPasses) )
		{
			m_stats.m_nTriangles += RenderPeeledRColors();
		}

		//swap depth targets
		SwapDepthTargets( pDstDepth, pSrcDepth );
		D3DPERF_EndEvent();
	}

	pOcclusionQuery->Release();

	//finally composite accumulated transparency with target
	//only copy result back when there's no input render function,
	//if a render function is passed in then the caller need to handle the compositing itself
	CompositeTextureInverse( m_ColorAccumulationTarget, m_pRenderTarget );

	g3dDX11Util::SetCullEnable( true );
	g3dSingleLightRendering::SetDoTransparentPass(false);
	//disable blending changes (we'll control the blending for transparency manually)
	g3dDX11Util::AllowAdditiveChanges(true);

	// restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	D3DPERF_EndEvent();
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
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::RenderPeeledRColors" );

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
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dDX11Util::CopyTexToTarget( m_SinglePeelTarget, m_ColorAccumulationTarget ); //(copy needed to overcome MSAA sampling limitation)

	//restore states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dBlendStateMgr::SetBlendState( &st_SavedBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DPERF_EndEvent();

	return tris;
}

//composite overlays the source on top of the dest using the inverse of the src
//The shader inverts the alpha so that the alpha channel can blend inversely
//C =    Sc  + Dc * (1-Sa)
//A = (1-Sa) + Da
void shdwPassTransparent::CompositeTextureInverse(matTexture* src, g2dRenderTarget* tgt)
{
	UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassTransparent::CompositeTextureInverse" );

	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = pEffect->GetTechniqueByName("SimpleCopyInvAlpha");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions( w, h);

	g3dBlendStateMgr::SetBlendState( st_CopyTarget );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	D3DX11_TECHNIQUE_DESC techDesc;
	pEffectTechnique->GetDesc( &techDesc );
	for (uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pEffectTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);

		// alpha blending/z writing control by callar
		ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(src);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);

		g3dDX11Util::DrawFullScreenQuad( w, h );
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	ID3D11ShaderResourceView* nullTex[1] = { NULL };
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	D3DPERF_EndEvent();
}


int shdwPassTransparent::RenderDeferred(float i_time)
{
	int nTris = 0;

	g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();

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
		const std::vector<g3dLight*>& lights = g3dLightMgrDX11::Implementation()->GetLights( );
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

	g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();

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
	UINT8 EN_WRITE_COLOR = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	st_Initial = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Accumulate = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, EN_WRITE_COLOR );
	st_MultiRender = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CompositeRev = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_DEST_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_RenderInit = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CopyTarget = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

//static
void shdwPassTransparent::CleanupStates()
{
	SAFE_DELETE( st_Initial );
	SAFE_DELETE( st_Accumulate );
	SAFE_DELETE( st_MultiRender );
	SAFE_DELETE( st_CompositeRev );
	SAFE_DELETE( st_RenderInit );
	SAFE_DELETE( st_CopyTarget );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_Write_Less_NS );
	SAFE_DELETE( ds_Test_Less_NS );
	SAFE_DELETE( ds_Disable_NS ); 
	SAFE_DELETE( ds_Test_Equal_NS );
}