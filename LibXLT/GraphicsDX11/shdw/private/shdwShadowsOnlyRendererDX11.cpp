/****************************************************************************\
**	shdwShadowsOnlyRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwShadowsOnlyRendererDX11.hpp"

#include "Graphics/Cam/camCamera.hpp"
//#include "Graphics/Eff/effStrandHairData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/Eff/effStrandHair.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/G2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"

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
void shdwShadowsOnlyRendererDX11::InitStates()
{
	stp_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_MulNoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_IlluminationBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwShadowsOnlyRendererDX11::CleanupStates()
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
shdwShadowsOnlyRendererDX11::shdwShadowsOnlyRendererDX11(bool i_bMasking)
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
shdwShadowsOnlyRendererDX11::~shdwShadowsOnlyRendererDX11()
{
	delete m_pHairMat;
	delete m_pShadowMat;
	delete m_pIlluminationMat;
	ReleaseResources();
}

void shdwShadowsOnlyRendererDX11::ReleaseResources()
{
	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwShadowsOnlyRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwShadowsOnlyRendererDX11::Render" );

	if (!g3dPrefs::CurrentPrefs().m_bProjLightsOn)
		return 0;

	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

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

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

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

	g3dDX11Util::AllowAdditiveChanges(false);

	// draw!!
	int i, j, n;
	// draw opaques per-light, shaded, and add to hdr buffer
	// lights
	const std::vector<g3dLight*>& lights = g3dLightMgrDX11::Implementation()->GetLights( );
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
		g3dLightMgrDX11::Implementation()->SetLight( pLight );
		g3dLightMgrDX11::Implementation()->EnableLight( pLight );

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
			g3dTransparencySortDX11* pHairNodes = m_SceneDatabase.GetHairNodes();
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
			g3dTransparencySortDX11* pTransNodes = m_SceneDatabase.GetTransparentNodes();
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

		g3dLightMgrDX11::Implementation()->DisableLight( pLight );
		g3dSingleLightRendering::SetActiveLight( NULL );
	}

	g3dDX11Util::AllowAdditiveChanges(true);

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


	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

{//PROFILE("debug");
	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	g2dScreen::SetDebugInfo(2, num);
}
	D3DPERF_EndEvent();
	return l_nNumTriangles;
}

int shdwShadowsOnlyRendererDX11::DrawNode(const g3dSceneNode* i_pNode, 
										 g3dLight* i_pLight, 
										 const g3dProjectedLight* i_pProjLight)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwShadowsOnlyRendererDX11::DrawNode" );

	// resolve material/effect
	matShaderEffect* pEffect = matShaderMgr::GetEffect( m_bMasking ? *m_pShadowMat : *m_pIlluminationMat );
	effShaderBaseDX11* pEffDX11 = dynamic_cast<effShaderBaseDX11*>(pEffect);
	fxEffect* pID3DXEffect = pEffDX11->GetFxEffect();

	pID3DXEffect->GetVariableByName("g_UseCosine")->AsScalar()->SetBool((!m_bMasking && g3dPrefs::CurrentPrefs().m_bIlluminationUsesNormals) ? TRUE : FALSE);

	DBG_ASSERT(i_pLight != NULL, "null light in shadows only drawnode");
	if (i_pProjLight != NULL)
	{
		matShaderEffect::Technique tec = g3dSceneRenderUtil::SelectShadowTechnique(i_pProjLight);
		pEffect->SetTechnique(tec);
	}
	else
		pEffect->SetTechnique(matShaderEffect::e_SingleLight);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

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

	pID3DXEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pID3DXEffect->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pID3DXEffect->GetVariableByName("g_transparency")->AsScalar()->SetFloat(val);

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, m_pShadowMat, pEffect );
	D3DPERF_EndEvent();
	return nTriangles;
}

int shdwShadowsOnlyRendererDX11::DrawHairNode(const g3dSceneNode* i_pNode, 
										 g3dLight* i_pLight, 
										 const g3dProjectedLight* i_pProjLight)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwShadowsOnlyRendererDX11::DrawHairNode" );
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

//	effShaderBaseDX11* pEffDX11 = dynamic_cast<effShaderBaseDX11*>(pEffect);
//	ID3DXEffect* pID3DXEffect = pEffDX11->GetFxEffect();
	pID3DXEffect->GetVariableByName("g_UseCosine")->AsScalar()->SetBool((!m_bMasking && g3dPrefs::CurrentPrefs().m_bIlluminationUsesNormals) ? TRUE : FALSE);

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
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// light before material so that material can override light settings if needed.
	pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());

	pEffect->SetupMaterial(m_pHairMat);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, m_pHairMat, pHairEffect );
	D3DPERF_EndEvent();
	return nTriangles;
}

//--------------------------------------------------------------------
//	Test node against light set and light frustum. If it passes, draw it.
//--------------------------------------------------------------------
int shdwShadowsOnlyRendererDX11::CullOrDrawNode(const sNodePlusState& i_Node, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight)
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
void shdwShadowsOnlyRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
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
g2dRenderTarget* shdwShadowsOnlyRendererDX11::GetRawBuffer()
{
	return (m_bMasking) ? NULL : m_pFrameBuffer->HDRRenderTargetTex();
}

g2dPFD::PixelFormat shdwShadowsOnlyRendererDX11::GetRawBufferPFD()
{
	return l_hdrPFD.GetPixelFormat();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwShadowsOnlyRendererDX11::CopyToBackBuf(matRenderTargetTexture* pTex)
{
	g3dBlendStateMgr::SetBlendState(stp_MulNoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDX11Util::CopyTexToTarget(pTex, m_pWindow);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(stp_MulBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
}

