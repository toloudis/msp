#include "GraphicsDX11/shdw/shdwPassFog.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderEffect.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"
#include "Graphics/G3d/g3dFragment.hpp"

namespace
{
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
};	//namespace

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassFog::shdwPassFog()
	: m_sceneInfo(NULL),
	m_ColorBuffer(NULL),
	m_DepthBuffer(NULL),
	m_TempBuffer(NULL),
	m_Scene(NULL),
	m_Camera(NULL)
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassFog::shdwPassFog(g2dRenderTarget* io_ColorBuffer,
	matRenderTargetTexture* i_DepthBuffer,
	matRenderTargetTexture* i_TempBuffer,
	const g3dScene* i_Scene,
	const camCamera* i_Camera
	)
	: m_sceneInfo(NULL),
	m_ColorBuffer(io_ColorBuffer),
	m_DepthBuffer(i_DepthBuffer),
	m_TempBuffer(i_TempBuffer),
	m_Scene(i_Scene),
	m_Camera(i_Camera)
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassFog::~shdwPassFog()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassFog::SetBuffers(g2dRenderTarget* io_ColorBuffer,
	matRenderTargetTexture* i_DepthBuffer,
	matRenderTargetTexture* i_TempBuffer
	)
{
	m_ColorBuffer = io_ColorBuffer;
	m_DepthBuffer = i_DepthBuffer;
	m_TempBuffer = i_TempBuffer;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassFog::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassFog::Render" );

	m_stats.Reset();

	// 1. render eye-space depths into buffer.
	m_DepthBuffer->Clear(maFloatRGBA(1,1,1,1), true, 1.0, false);
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	shdwPassDepth depthPass(m_DepthBuffer, m_Camera, false, NULL, shdwPassDepth::eView);
	depthPass.SetSceneInfo(m_sceneInfo);
	m_DepthBuffer->MakeCurrent();
	depthPass.Render(i_time);
	//matTextureMgr::SaveTextureToFile(m_DepthBuffer, fsLocator(itString("C:\\Projects\\Depth.dds")));

	// get fog data to send to shader.
	fogParams fog_params;
	m_Scene->GetFogSettings(fog_params);

	effPlainShaderDX11* pFog = dynamic_cast<effPlainShaderDX11*>(g3dDX11Util::GetEffect("Fog"));
	if (!pFog)
		return 0;
	fxEffectDX11* pEffect = pFog->GetEffect();

	int technique = -1;
	switch (fog_params.m_nMode)
	{
	case g3dType::e_FogModeLinear:
		technique = pEffect->FindTechnique("LinearFog");
		break;
	case g3dType::e_FogModeExp:
		technique = pEffect->FindTechnique("ExponentialFog");
		break;
	case g3dType::e_FogModeExp2:
		technique = pEffect->FindTechnique("ExponentialSquaredFog");
		break;
	default:
		// no fog!!!
		return 0;
	}

	pEffect->SetConstant(pEffect->FindConstant("g_FogDepthStart"), fog_params.m_fStart);

	float tmpRange = fog_params.m_fEnd - fog_params.m_fStart;
	if (tmpRange == 0)
		tmpRange = 1000.0f;
	pEffect->SetConstant(pEffect->FindConstant("g_FogDepthRange"), tmpRange);
	pEffect->SetConstant(pEffect->FindConstant("g_FogAltitudeStart"), fog_params.m_fAltitudeStart);
	tmpRange = fog_params.m_fAltitudeEnd - fog_params.m_fAltitudeStart;
	if (tmpRange == 0)
		tmpRange = 10.0f;
	pEffect->SetConstant(pEffect->FindConstant("g_FogAltitudeRange"), tmpRange);
	pEffect->SetConstant(pEffect->FindConstant("g_FogDensity"), fog_params.m_fDensity);
	pEffect->SetConstant(pEffect->FindConstant("g_FogAltitudeDensity"), fog_params.m_fAltitudeDensity);

	pEffect->SetConstant(pEffect->FindConstant("g_FogColor"), fog_params.m_Color.Ptr(), 4 * sizeof(float));

	GetFogOrientation(fog_params);
	float orientation[3] = {0};
	orientation[0] = fog_params.m_Orientation.GetX();
	orientation[1] = fog_params.m_Orientation.GetY();
	orientation[2] = fog_params.m_Orientation.GetZ();
	pEffect->SetConstant(pEffect->FindConstant("g_FogOrientation"), orientation);
	
	float fovx = m_Camera->GetFOV() * maConstants::c_fAngleToRad;
	float m_InvFocalLen[2];
	m_InvFocalLen[0]   = tanf(fovx * 0.5f);
	m_InvFocalLen[1]   = tanf(fovx * 0.5f)/m_Camera->GetAspect();
	// NOTE: this has always passed the orientation, not m_InvFocalLen; kept as is
	// so fog looks the same as before the conversion.
	pEffect->SetConstant(pEffect->FindConstant("g_InvFocalLen"), orientation, 2 * sizeof(float));

	// transform to get world space position.
	pEffect->SetMatrix(pEffect->FindConstant("g_CameraToWorldSpace"), g3dSceneGlobal::GetCameraInverseTransform().Ptr());

	// 1. Put color to temp for use as an input texture.
	m_ColorBuffer->MakeCurrent();
	int w,h;
	m_ColorBuffer->GetDimensions(w,h);
	g3dDX11Util::CopyBackBufferToRenderTargetTex(m_TempBuffer);
	// 2. Use temp to read, and write result into color.
	pEffect->SetResource(pEffect->FindResource("tLinDepth"), m_DepthBuffer->GetSurface());
	pEffect->SetResource(pEffect->FindResource("tColors"), m_TempBuffer->GetSurface());

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	int nPasses = pEffect->GetPassCount(technique);
	for (int pass = 0; pass < nPasses; ++pass)
	{
		pEffect->Apply(technique, pass, g2dDX11Global::g_pDeviceContext);

		g3dDX11Util::DrawFullScreenQuad( w,h );
	}

	// unbind the inputs; tColors is about to be rendered into again
	ID3D11ShaderResourceView* nullTex[2] = {NULL, NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 2, nullTex);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dBlendStateMgr::SetBlendState(st_Blend);

	// draw geometry
#if 0	
	const camCamera* camera = m_sceneInfo->GetCamera();
	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(*camera);
	// Z Buffering

	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeFog(currentNode.m_pNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeFog(currentNode.m_pNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		
		g3dDrawStyleUtilDX11::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeFog(currentNode.m_pNode);
	}

	// is there a better way to handle transparent stuff for Fog?
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dSingleLightRendering::SetDoTransparentPass(true);
		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		g3dTransparencySortDX11* transparencySort = m_sceneInfo->GetTransparentNodes();
		const TranspNodeVector& transpNodes = transparencySort->GetTransparentNodes();
		n = transpNodes.size();
		for (i = 0; i < n; i++)
		{
			const TranspNode& currentNode = transpNodes[i];
			
			m_stats.m_nTriangles += DrawNodeFog(currentNode.m_pSceneNode);
		}

		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		g3dSingleLightRendering::SetDoTransparentPass(false);
	}

	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
#endif

	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassFog::DrawNodeFog(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique(matShaderEffect::e_FogPrep);		
//	pEffect->SetupFogPrep();

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);
	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	return nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassFog::GetFogOrientation(fogParams& io_FogParams)
{
	if(io_FogParams.m_bUseWorld)
	{
		io_FogParams.m_Orientation = m_Camera->GetUp();
	}
	else
	{
		io_FogParams.m_Orientation = maVector3d(0, 1, 0); 
	}
}

void shdwPassFog::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
}

void shdwPassFog::CleanupStates()
{
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Blend );
}