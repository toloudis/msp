/****************************************************************************\
**	shdwPassPostShader.cpp
**
**		see .hpp
**
**  Riva Chang
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSPOSTSHADER_HPP
#include "GraphicsVulkan/shdw/shdwPassPostShader.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_POSTPROCESSING_HPP
#include "Graphics/g3d/g3dPostProcessing.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif


namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;

	void setupPfxGlobal(fxEffect* i_pD3DEffect, matRenderTargetTexture* i_pTex)
	{
		i_pD3DEffect->GetVariableByName("g_ImageWidth")->AsScalar()->SetFloat((float)(i_pTex->GetWidth()));
		i_pD3DEffect->GetVariableByName("g_ImageHeight")->AsScalar()->SetFloat((float)(i_pTex->GetHeight()));
	}
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassPostShader::shdwPassPostShader(matRenderTargetTexture* i_ScratchTex, matRenderTargetTexture* i_destination)
	: m_pScratchTex(i_ScratchTex), m_pDestTex(i_destination)
{
	m_pRenderTarget = i_destination->GetRenderTargetAPI();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassPostShader::~shdwPassPostShader()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassPostShader::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassPostShader");
	unsigned int uiPass;

	// Set Render state status
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	// Get post effect
	effShaderParams* pShaderParam = g3dPostProcessing::GetPostEffect().get();
	const matShaderEffect* pMatEffect = pShaderParam->GetShader();
	effShaderBaseVk* pEffect = (effShaderBaseVk*)pMatEffect;
	fxEffect* pD3DEffect = pEffect->GetFxEffect();
	setupPfxGlobal(pD3DEffect, m_pScratchTex);
	pEffect->SetTechnique("Default");

	// Draw post effect onto scratch texture
	m_pScratchTex->MakeCurrent();
	vkdImage* aRes = g3dVkTextureUtil::GetImage(m_pDestTex);
	if (pShaderParam->m_pShaderBindings)
		pShaderParam->m_pShaderBindings->Bind();

	int nPasses = pEffect->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffect->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, &aRes);
		g3dVkUtil::DrawFullScreenQuad(m_pDestTex->GetWidth(), m_pDestTex->GetHeight());
		pEffect->EndPass();
	}
	pEffect->End();

	vkdImage* nullTex[1] = {NULL};
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

	// Copy result back to destination texture
	g3dVkUtil::CopyTexToTarget(m_pScratchTex, m_pDestTex->GetRenderTargetAPI());

	return 0;
}

void shdwPassPostShader::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassPostShader::CleanupStates()
{
	delete st_NoBlend;		st_NoBlend = NULL;
	delete ds_Disable_NS;	ds_Disable_NS = NULL;
}

}
