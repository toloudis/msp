/****************************************************************************\
**	shdwPassMapNormalsToScreen.hpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassMapNormalsToScreen.hpp"

#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

shdwPassMapNormalsToScreen::shdwPassMapNormalsToScreen(matTexture* i_pNormals, 
													   g2dRenderTarget* i_pScreen)
:	m_pNormals(i_pNormals),
	m_pDestination(i_pScreen)
{
	// don't create this on every frame.
	D3D11_SAMPLER_DESC SamplerDesc;
	ZeroMemory( &SamplerDesc, sizeof(SamplerDesc) );
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	HRESULT hr = ( g2dDX11Global::g_pDevice->CreateSamplerState( &SamplerDesc, &m_pSampleStatePoint ) );
}

shdwPassMapNormalsToScreen::~shdwPassMapNormalsToScreen()
{
	m_pSampleStatePoint->Release();
}

int shdwPassMapNormalsToScreen::Render(float i_Time)
{
	m_pDestination->MakeCurrent();
	int w,h;
	m_pDestination->GetDimensions(w,h);

	ID3D11SamplerState* aSamplers[] = { m_pSampleStatePoint };
    g2dDX11Global::g_pDeviceContext->PSSetSamplers( 0, 1, aSamplers );

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(m_pNormals);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);

//	g2dFullscreenQuad::DrawFullScreenQuad11( 
//		effShaderArray::GetMapNormalsToScreen(),w,h);

	g2dFullscreenQuad::DrawFullScreenQuad11( w, h );

	// unbind normals buffer as input
    ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, ppSRVNULL);

	return 2;
}
