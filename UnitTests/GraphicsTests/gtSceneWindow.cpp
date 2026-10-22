/*****************************************************************************
**  gtSceneWindow.cpp
**
**      See gtSceneWindow.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtSceneWindow.hpp"
#include "gtPlatform.hpp"

#include <gtest/gtest.h>

#ifndef G2D_SYSTEM_HPP
#include "Graphics/G2d/g2dSystem.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef GRAPHICSDX11_LAYER_HPP
#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#endif
#ifndef G2D_WINDOWDX11_HPP
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#endif
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_G2DWINDOWVK_HPP
#include "GraphicsVulkan/G2d/g2dWindowVk.hpp"
#endif

#include <wrl/client.h>

#include <cstring>
#include <utility>

namespace
{
	using Microsoft::WRL::ComPtr;

	g2dSystem* system_2d(gtBackend::Kind i_Kind)
	{
		return i_Kind == gtBackend::e_DX11 ? GraphicsDX11Layer::GetSystem2D() : GraphicsVulkanLayer::GetSystem2D();
	}
}

gtSceneWindow::gtSceneWindow(gtBackend::Kind i_Kind, int i_Width, int i_Height)
	: m_Kind(i_Kind)
	, m_pNative(gtPlatform::CreateHiddenWindow(i_Width, i_Height))
	, m_pWindow(system_2d(i_Kind)->CreateSubWindow(m_pNative))
{
}

gtSceneWindow::~gtSceneWindow()
{
	system_2d(m_Kind)->DestroyWindow(m_pWindow);
	gtPlatform::DestroyHiddenWindow(m_pNative);
}

void gtSceneWindow::Resize(int i_Width, int i_Height)
{
	gtPlatform::ResizeHiddenWindow(m_pNative, i_Width, i_Height);
	m_pWindow->ResizeWindow(i_Width, i_Height);
}

gtPicture gtSceneWindow::Present()
{
	gtPicture picture;
	if (m_Kind == gtBackend::e_Vulkan)
	{
		gvk::g2dWindowVk* window = dynamic_cast<gvk::g2dWindowVk*>(m_pWindow);
		EXPECT_TRUE(window != NULL);
		window->DebugReadBackNextFrame();
		window->Present();
		picture.m_Rgba = window->DebugPixels();
		if (window->SwapchainFormat() == VK_FORMAT_B8G8R8A8_UNORM)
		{
			for (size_t i = 0; i + 3 < picture.m_Rgba.size(); i += 4)
				std::swap(picture.m_Rgba[i], picture.m_Rgba[i + 2]);
		}
	}
	else
	{
		picture = read_dx11();
		m_pWindow->Present();
	}
	int x = 0, y = 0;
	m_pWindow->GetPresentRect(x, y, picture.m_Width, picture.m_Height);
	picture.m_Width += 2 * x;
	picture.m_Height += 2 * y;
	return picture;
}

gtPicture gtSceneWindow::read_dx11()
{
	gtPicture picture;
	g2dWindowDX11* window = dynamic_cast<g2dWindowDX11*>(m_pWindow);
	EXPECT_TRUE(window != NULL);
	if (!window)
		return picture;
	g2dD3D11RenderTargetPtr view = window->GetBackBuffer();
	ComPtr<ID3D11Resource> resource;
	view->GetResource(&resource);
	view->Release();
	D3D11_TEXTURE2D_DESC desc;
	static_cast<ID3D11Texture2D*>(resource.Get())->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	ComPtr<ID3D11Texture2D> staging;
	if (FAILED(g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &staging)))
	{
		ADD_FAILURE() << "no staging texture";
		return picture;
	}
	ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
	context->CopyResource(staging.Get(), resource.Get());
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		const size_t row = (size_t)desc.Width * 4;
		picture.m_Rgba.resize(row * desc.Height);
		for (UINT y = 0; y < desc.Height; y++)
			memcpy(&picture.m_Rgba[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row);
		context->Unmap(staging.Get(), 0);
	}
	return picture;
}
