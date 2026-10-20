#ifndef GVK_SHDW_PASSENVBACKGROUND_HPP
#include "GraphicsVulkan/shdw/shdwPassEnvBackground.hpp"
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
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
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
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
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
	matMaterial l_EnvBgMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;

	void setupEnvGlobal(fxEffect* i_pD3DEffect,
						const g3dAmbientEnvState& i_EnvState,
						const camCamera* i_pCam)
	{
		i_pD3DEffect->GetVariableByName("camera_up")->AsVector()->SetFloatVector(i_pCam->GetUp().GetPtr());
		i_pD3DEffect->GetVariableByName("camera_dir")->AsVector()->SetFloatVector(i_pCam->GetDirection().GetPtr());
		i_pD3DEffect->GetVariableByName("camera_left")->AsVector()->SetFloatVector(i_pCam->GetLeft().GetPtr());

		float fovx = i_pCam->GetFOV() * maConstants::c_fAngleToRad;
		float w = i_pCam->GetNearClip() * tanf(fovx * 0.5f);
		float h = w / i_pCam->GetAspect();
		i_pD3DEffect->GetVariableByName("plane_width")->AsScalar()->SetFloat(w);
		i_pD3DEffect->GetVariableByName("plane_height")->AsScalar()->SetFloat(h);
		i_pD3DEffect->GetVariableByName("plane_dist")->AsScalar()->SetFloat(i_pCam->GetNearClip());

		// ambient state
		i_pD3DEffect->GetVariableByName("envColor")->AsVector()->SetFloatVector(i_EnvState.m_DiffuseColor.GetPtr());
		i_pD3DEffect->GetVariableByName("envAngle")->AsScalar()->SetFloat(i_EnvState.m_DiffuseAngle * maConstants::c_fAngleToRad);
		i_pD3DEffect->GetVariableByName("envFactor")->AsScalar()->SetFloat(i_EnvState.m_DiffuseFactor);
		i_pD3DEffect->GetVariableByName("hasEnvTexture")->AsScalar()->SetBool((i_EnvState.m_DiffuseMap != NULL));
		i_pD3DEffect->GetVariableByName("envTexture")->AsShaderResource()->SetResource(g3dVkTextureUtil::GetImage(i_EnvState.m_DiffuseMap));
	}
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassEnvBackground::shdwPassEnvBackground(g2dRenderTarget* i_pRenderTarget,
											 const camCamera* i_pCamera,
											 const g3dAmbientEnvState& i_AmbientState)
	: m_pCamera(i_pCamera), m_AmbientState(i_AmbientState)
{
	m_pRenderTarget = i_pRenderTarget;

	// lazy init so that this material can be reused across instantiations.
	if (!l_EnvBgMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/EnvBackground.fx"), matShaderMgr::GetSpecialEffect("EnvBackground.fx"));
		l_EnvBgMat.SetShaderParams(p);
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassEnvBackground::~shdwPassEnvBackground()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassEnvBackground::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassEnvBackground");
	uint32_t uiPass;

	// Set Render state status
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	const matMaterial* pMaterial = &l_EnvBgMat;
	effShaderBaseVk* pEffect = (effShaderBaseVk*)(matShaderMgr::GetEffect(*pMaterial));
	fxEffect* pD3DEffect = pEffect->GetFxEffect();
	setupEnvGlobal(pD3DEffect, m_AmbientState, m_pCamera);
	pEffect->SetTechnique("Default");

	m_pRenderTarget->MakeCurrent();
	int w, h;
	m_pRenderTarget->GetDimensions(w, h);
	int nPasses = pEffect->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffect->BeginPass(uiPass);
		g3dVkUtil::DrawFullScreenQuad(w, h);
		pEffect->EndPass();
	}
	pEffect->End();


	return 0;
}

void shdwPassEnvBackground::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassEnvBackground::CleanupStates()
{
	delete st_NoBlend; st_NoBlend = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
}

}
