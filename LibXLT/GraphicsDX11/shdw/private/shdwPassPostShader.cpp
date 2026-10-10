#include "GraphicsDX11/shdw/shdwPassPostShader.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPostProcessing.hpp"
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
	UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassPostShader::Render" );

	// Set Render state status
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	// Get post effect
	effShaderParams* pShaderParam = g3dPostProcessing::GetPostEffect().get();
	const matShaderEffect* pMatEffect = pShaderParam->GetShader();
	effShaderBaseDX11* pEffect = (effShaderBaseDX11*)pMatEffect;
	fxEffect* pD3DEffect = pEffect->GetFxEffect();
	setupPfxGlobal(pD3DEffect, m_pScratchTex);
	pEffect->SetTechnique("Default");

	// Draw post effect onto scratch texture
	m_pScratchTex->MakeCurrent();
	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(m_pDestTex);
	if (pShaderParam->m_pShaderBindings)
		pShaderParam->m_pShaderBindings->Bind();
	
	int nPasses = pEffect->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffect->BeginPass(uiPass);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad(m_pDestTex->GetWidth(), m_pDestTex->GetHeight());
		pEffect->EndPass();
	}
	pEffect->End();

	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	// Copy result back to destination texture
	g3dDX11Util::CopyTexToTarget(m_pScratchTex, m_pDestTex->GetRenderTargetAPI());

	D3DPERF_EndEvent();

	return 0;
}

void shdwPassPostShader::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_NEVER, FALSE, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassPostShader::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( ds_Disable_NS );
}
