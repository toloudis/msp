#include "GraphicsDX11/shdw/shdwPassEnvBackground.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

#include <boost/lexical_cast.hpp>

namespace
{
	matMaterial l_EnvBgMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;

	void setupEnvGlobal(ID3DX11Effect* i_pD3DEffect, 
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
		i_pD3DEffect->GetVariableByName("envTexture")->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(i_EnvState.m_DiffuseMap));
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
	UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassEnvBackground::Render" );

	// Set Render state status
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	const matMaterial* pMaterial = &l_EnvBgMat;
	effShaderBaseDX11* pEffect = (effShaderBaseDX11*)(matShaderMgr::GetEffect(*pMaterial));
	ID3DX11Effect* pD3DEffect = pEffect->GetD3DXEffect();
	setupEnvGlobal(pD3DEffect, m_AmbientState, m_pCamera);
	pEffect->SetTechnique("Default");

	m_pRenderTarget->MakeCurrent();
	int w, h;
	m_pRenderTarget->GetDimensions(w, h);
	int nPasses = pEffect->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffect->BeginPass(uiPass);
		g3dDX11Util::DrawFullScreenQuad(w, h);
		pEffect->EndPass();
	}
	pEffect->End();

	D3DPERF_EndEvent();

	return 0;
}

void shdwPassEnvBackground::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);
	
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassEnvBackground::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( ds_Disable_NS );
}
