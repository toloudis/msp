/*****************************************************************************
**  gtD3D11.cpp
**
**      see gtD3D11.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Dx11/gtD3D11.hpp"

#include <dxgi.h>

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
