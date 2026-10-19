/*****************************************************************************
**  gtTarget.cpp
**
**      See gtTarget.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtTarget.hpp"

#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

//	DX11
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif

//	Vulkan
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <wrl/client.h>

#include <gtest/gtest.h>

#include <cstring>

using Microsoft::WRL::ComPtr;

gtTarget::gtTarget(gtBackend::Kind i_Kind, int i_Width, int i_Height, const float i_Clear[4])
	: m_Kind(i_Kind)
{
	const g2dPFD rgba8(0, 8, 8, 8, 16, 8, 24, 8, 32);
	m_pTexture = matTextureMgr::CreateRenderTargetTexture(i_Width, i_Height, false, &rgba8, false, true);
	EXPECT_TRUE(m_pTexture && m_pTexture->GetRenderTargetAPI()) << "no render target";
	if (Target())
	{
		Target()->MakeCurrent();
		Target()->Clear(maFloatRGBA(i_Clear[0], i_Clear[1], i_Clear[2], i_Clear[3]), true, 1.0f);
	}
}

gtTarget::~gtTarget()
{
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);
}

g2dRenderTarget* gtTarget::Target() const
{
	return m_pTexture ? m_pTexture->GetRenderTargetAPI() : NULL;
}

gtPicture gtTarget::Read() const
{
	gtPicture picture;
	if (!m_pTexture)
		return picture;
	picture.m_Width = m_pTexture->GetWidth();
	picture.m_Height = m_pTexture->GetHeight();
	if (m_Kind == gtBackend::e_Vulkan)
	{
		gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(m_pTexture);
		EXPECT_TRUE(image != NULL);
		if (!image)
			return picture;
		gvk::vkdGlobal::Context().Flush();
		picture.m_Rgba = image->ReadBack(0, 0);
		return picture;
	}

	const matTextureDX11* texture = dynamic_cast<const matTextureDX11*>(m_pTexture);
	EXPECT_TRUE(texture && texture->GetResource());
	if (!texture || !texture->GetResource())
		return picture;
	ID3D11Device* device = g2dDX11Global::g_pDevice;
	ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
	D3D11_TEXTURE2D_DESC desc;
	static_cast<ID3D11Texture2D*>(texture->GetResource())->GetDesc(&desc);
	D3D11_TEXTURE2D_DESC staging_desc = desc;
	staging_desc.MipLevels = staging_desc.ArraySize = 1;
	staging_desc.SampleDesc.Count = 1;
	staging_desc.SampleDesc.Quality = 0;
	staging_desc.Usage = D3D11_USAGE_STAGING;
	staging_desc.BindFlags = 0;
	staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	staging_desc.MiscFlags = 0;
	ComPtr<ID3D11Texture2D> staging;
	if (FAILED(device->CreateTexture2D(&staging_desc, NULL, &staging)))
	{
		ADD_FAILURE() << "no staging texture";
		return picture;
	}
	context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, texture->GetResource(), 0, NULL);
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		const size_t row = (size_t)desc.Width * 4;
		picture.m_Rgba.resize(row * desc.Height);
		for (UINT y = 0; y < desc.Height; y++)
		{
			memcpy(&picture.m_Rgba[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row);
		}
		context->Unmap(staging.Get(), 0);
	}
	return picture;
}
