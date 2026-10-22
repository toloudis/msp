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

#include <algorithm>
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
	picture.m_Rgba = gtReadTexture(m_Kind, m_pTexture, 4);
	return picture;
}

std::vector<uint8_t> gtReadTexture(gtBackend::Kind i_Kind, const matTexture* i_pTexture, int i_BytesPerTexel,
								   int i_Mip, int i_Layer)
{
	if (i_Kind == gtBackend::e_Vulkan)
	{
		gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(i_pTexture);
		EXPECT_TRUE(image != NULL);
		return gtReadVkImage(image, i_Mip, i_Layer);
	}

	const matTextureDX11* texture = dynamic_cast<const matTextureDX11*>(i_pTexture);
	EXPECT_TRUE(texture && texture->GetResource());
	if (!texture || !texture->GetResource())
		return std::vector<uint8_t>();
	D3D11_RESOURCE_DIMENSION dimension;
	texture->GetResource()->GetType(&dimension);
	if (dimension == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
		return gtReadD3D11Volume(static_cast<ID3D11Texture3D*>(texture->GetResource()), i_BytesPerTexel, i_Mip);
	return gtReadD3D11Texture(static_cast<ID3D11Texture2D*>(texture->GetResource()), i_BytesPerTexel, i_Mip, i_Layer);
}

std::vector<uint8_t> gtReadD3D11Volume(ID3D11Texture3D* i_pTexture, int i_BytesPerTexel, int i_Mip)
{
	std::vector<uint8_t> texels;
	if (!i_pTexture)
		return texels;
	ID3D11Device* device = g2dDX11Global::g_pDevice;
	ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
	D3D11_TEXTURE3D_DESC desc;
	i_pTexture->GetDesc(&desc);
	const UINT subresource = D3D11CalcSubresource((UINT)i_Mip, 0, desc.MipLevels);
	desc.Width = (std::max)(1u, desc.Width >> i_Mip);
	desc.Height = (std::max)(1u, desc.Height >> i_Mip);
	desc.Depth = (std::max)(1u, desc.Depth >> i_Mip);
	D3D11_TEXTURE3D_DESC staging_desc = desc;
	staging_desc.MipLevels = 1;
	staging_desc.Usage = D3D11_USAGE_STAGING;
	staging_desc.BindFlags = 0;
	staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	staging_desc.MiscFlags = 0;
	ComPtr<ID3D11Texture3D> staging;
	if (FAILED(device->CreateTexture3D(&staging_desc, NULL, &staging)))
	{
		ADD_FAILURE() << "no staging volume";
		return texels;
	}
	context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, i_pTexture, subresource, NULL);
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		const size_t row = (size_t)desc.Width * i_BytesPerTexel;
		texels.resize(row * desc.Height * desc.Depth);
		for (UINT z = 0; z < desc.Depth; z++)
		{
			for (UINT y = 0; y < desc.Height; y++)
			{
				memcpy(&texels[(z * desc.Height + y) * row],
					   (const uint8_t*)mapped.pData + z * mapped.DepthPitch + y * mapped.RowPitch, row);
			}
		}
		context->Unmap(staging.Get(), 0);
	}
	return texels;
}

std::vector<uint8_t> gtReadVkImage(gvk::vkdImage* i_pImage, int i_Mip, int i_Layer)
{
	if (!i_pImage)
		return std::vector<uint8_t>();
	gvk::vkdGlobal::Context().Flush();
	return i_pImage->ReadBack((uint32_t)i_Mip, (uint32_t)i_Layer);
}

std::vector<uint8_t> gtReadD3D11Texture(ID3D11Texture2D* i_pTexture, int i_BytesPerTexel, int i_Mip, int i_Layer)
{
	std::vector<uint8_t> texels;
	if (!i_pTexture)
		return texels;
	ID3D11Device* device = g2dDX11Global::g_pDevice;
	ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
	D3D11_TEXTURE2D_DESC desc;
	i_pTexture->GetDesc(&desc);
	const UINT subresource = D3D11CalcSubresource((UINT)i_Mip, (UINT)i_Layer, desc.MipLevels);
	desc.Width = (std::max)(1u, desc.Width >> i_Mip);
	desc.Height = (std::max)(1u, desc.Height >> i_Mip);
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
		return texels;
	}
	context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, i_pTexture, subresource, NULL);
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		const size_t row = (size_t)desc.Width * i_BytesPerTexel;
		texels.resize(row * desc.Height);
		for (UINT y = 0; y < desc.Height; y++)
		{
			memcpy(&texels[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row);
		}
		context->Unmap(staging.Get(), 0);
	}
	return texels;
}
