/*****************************************************************************
**  gtD3D11.cpp
**
**      see gtD3D11.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Dx11/gtD3D11.hpp"

#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/G2d/g2dDX11GlobalWin.hpp"
#endif

#include <dxgi.h>

#include <cstring>

bool gtD3D11::Create(std::string& o_Error)
{
	const D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = ::D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &level, 1, D3D11_SDK_VERSION,
									 &m_pDevice, NULL, &m_pContext);
	if (FAILED(hr))
	{
		m_bWarp = true;
		hr = ::D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, &level, 1, D3D11_SDK_VERSION,
								 &m_pDevice, NULL, &m_pContext);
	}
	if (FAILED(hr))
	{
		char buf[64];
		sprintf_s(buf, "D3D11CreateDevice failed (0x%08X)", (unsigned)hr);
		o_Error = buf;
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	DXGI_ADAPTER_DESC desc = {};
	if (SUCCEEDED(m_pDevice.As(&dxgiDevice)) && SUCCEEDED(dxgiDevice->GetAdapter(&adapter)) &&
		SUCCEEDED(adapter->GetDesc(&desc)))
	{
		char name[256] = {};
		::WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, name, sizeof(name) - 1, NULL, NULL);
		m_AdapterName = name;
	}
	return true;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> gtD3D11::CreateTexture(int i_Width, int i_Height, const uint8_t* i_pRgba, bool i_bRenderTarget,
	ID3D11ShaderResourceView** o_ppView, ID3D11RenderTargetView** o_ppTarget)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (i_bRenderTarget ? D3D11_BIND_RENDER_TARGET : 0);
	D3D11_SUBRESOURCE_DATA init = { i_pRgba, (UINT)i_Width * 4, 0 };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	m_pDevice->CreateTexture2D(&desc, i_pRgba ? &init : NULL, &texture);
	if (texture && o_ppView)
	{
		m_pDevice->CreateShaderResourceView(texture.Get(), NULL, o_ppView);
	}
	if (texture && o_ppTarget)
	{
		m_pDevice->CreateRenderTargetView(texture.Get(), NULL, o_ppTarget);
	}
	return texture;
}

std::vector<uint8_t> gtD3D11::ReadBack(ID3D11Texture2D* i_pTexture)
{
	D3D11_TEXTURE2D_DESC desc;
	i_pTexture->GetDesc(&desc);
	desc.MipLevels = desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;
	std::vector<uint8_t> image((size_t)desc.Width * desc.Height * 4);
	if (FAILED(m_pDevice->CreateTexture2D(&desc, NULL, &staging)))
	{
		return image;
	}
	m_pContext->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, i_pTexture, 0, NULL);
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(m_pContext->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		for (UINT y = 0; y < desc.Height; y++)
		{
			memcpy(&image[(size_t)y * desc.Width * 4], (const uint8_t*)mapped.pData + y * mapped.RowPitch, desc.Width * 4);
		}
		m_pContext->Unmap(staging.Get(), 0);
	}
	return image;
}

gtD3D11::Globals::Globals(gtD3D11& i_D3D)
{
	g2dDX11Global::g_pDevice = i_D3D.Device();
	g2dDX11Global::g_pDeviceContext = i_D3D.Context();
}

gtD3D11::Globals::~Globals()
{
	g2dDX11Global::g_pDevice = NULL;
	g2dDX11Global::g_pDeviceContext = NULL;
}
