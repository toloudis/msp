/*****************************************************************************
**  gtD3D11.hpp
**
**      gtD3D11 gives a test a Direct3D 11 device: GraphicsDX11 is the
**      reference the Vulkan port is compared against. It uses the default
**      hardware adapter, or WARP (the software rasterizer, e.g. on CI) if
**      there is none; Create() fails only if neither works, and the test is
**      then skipped.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <string>
#include <vector>

class gtD3D11
{
	public:
		//	false (with o_Error) if no D3D11 device can be created
		bool Create(std::string& o_Error);

		ID3D11Device*			Device() const		{ return m_pDevice.Get(); }
		ID3D11DeviceContext*	Context() const		{ return m_pContext.Get(); }
		const std::string&		AdapterName() const	{ return m_AdapterName; }
		bool					IsWarp() const		{ return m_bWarp; }

		//	An R8G8B8A8_UNORM texture with i_pRgba (rows tightly packed; NULL:
		//	uninitialized), bindable as a shader resource and, with
		//	i_bRenderTarget, as a render target. o_ppView/o_ppTarget may be NULL.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateTexture(int i_Width, int i_Height, const uint8_t* i_pRgba, bool i_bRenderTarget,
			ID3D11ShaderResourceView** o_ppView, ID3D11RenderTargetView** o_ppTarget);
		//	The texture's first mip as RGBA8 rows, tightly packed
		std::vector<uint8_t> ReadBack(ID3D11Texture2D* i_pTexture);

		//	Points GraphicsDX11's globals (g2dDX11Global::g_pDevice,
		//	g_pDeviceContext) at this device while it lives, for GraphicsDX11
		//	code that uses them (g2dFullscreenQuad)
		class Globals
		{
			public:
				explicit Globals(gtD3D11& i_D3D);
				~Globals();
		};

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>		m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	m_pContext;
		std::string									m_AdapterName;
		bool										m_bWarp = false;
};

//	GT_D3D11(name): declares gtD3D11 name and skips the test if it can't be created
#define GT_D3D11(name) \
	gtD3D11 name; \
	{ \
		std::string gt_d3d11_error; \
		if (!name.Create(gt_d3d11_error)) \
		{ \
			GTEST_SKIP() << "No Direct3D 11 device: " << gt_d3d11_error; \
		} \
	}
