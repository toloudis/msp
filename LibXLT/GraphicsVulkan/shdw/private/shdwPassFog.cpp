/****************************************************************************\
**	shdwPassFog.cpp
**
**	Fog over the rendered scene: view-space depths into a buffer, then
**	the fog effect over a copy of the colour buffer.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSFOG_HPP
#include "GraphicsVulkan/shdw/shdwPassFog.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
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
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
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
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <cmath>

namespace gvk
{

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
	G3D_PROFILE_SCOPE("PassFog");

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

	effPlainShaderVk* pFog = dynamic_cast<effPlainShaderVk*>(g3dVkUtil::GetEffect("Fog"));
	if (!pFog)
		return 0;
	fxEffectVk* pEffect = pFog->GetEffect();

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
	g3dVkUtil::CopyBackBufferToRenderTargetTex(m_TempBuffer);
	// 2. Use temp to read, and write result into color.
	pEffect->SetResource(pEffect->FindResource("tLinDepth"), m_DepthBuffer->GetImage());
	pEffect->SetResource(pEffect->FindResource("tColors"), m_TempBuffer->GetImage());

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	int nPasses = pEffect->GetPassCount(technique);
	for (int pass = 0; pass < nPasses; ++pass)
	{
		pEffect->Apply(technique, pass);

		g3dVkUtil::DrawFullScreenQuad( w,h );
	}

	// unbind the inputs; tColors is about to be rendered into again
	vkdImage* nullTex[2] = {NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0, 2, nullTex);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dBlendStateMgr::SetBlendState(st_Blend);

	//	(DX11's #if 0 block, which drew each node with DrawNodeFog, is left
	//	out: it is D3D9 code that has been compiled out since the DX11 port)

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassFog::DrawNodeFog(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique(matShaderEffect::e_FogPrep);
//	pEffect->SetupFogPrep();

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);
	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

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
	const VkColorComponentFlags writeAll = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		writeAll );
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		writeAll );
}

void shdwPassFog::CleanupStates()
{
	delete ds_Disable_NS;			ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
	delete st_NoBlend;				st_NoBlend = NULL;
	delete st_Blend;				st_Blend = NULL;
}

}
