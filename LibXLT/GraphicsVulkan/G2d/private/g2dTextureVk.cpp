/****************************************************************************\
**  g2dTextureVk.cpp
**
**	See g2dTextureVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dTextureVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	const uint32_t c_TextureUsage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	uint32_t g2dFullMipCount(uint32_t i_Width, uint32_t i_Height, uint32_t i_Depth)
	{
		uint32_t largest = i_Width > i_Height ? i_Width : i_Height;
		largest = largest > i_Depth ? largest : i_Depth;
		uint32_t mips = 1;
		while (largest > 1)
		{
			largest >>= 1;
			mips++;
		}
		return mips;
	}

	g2dTextureVk::g2dTextureVk()
	:	m_Category(g2dResourceCounterVk::eTexture),
		m_Width(0),
		m_Height(0),
		m_Format(0),
		m_MipLevels(0)
	{
	}

	g2dTextureVk::~g2dTextureVk()
	{
		g2dResourceCounterVk::RemoveResource(GetSize(), m_Category);
	}

	//------------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//------------------------------------------------------------------------
	void g2dTextureVk::Make(int i_Width, int i_Height, uint32_t i_DxgiFormat,
							g2dResourceCounterVk::eResourceCategory i_Category/*= g2dResourceCounterVk::eTexture*/)
	{
		DBG_ASSERT(!m_pImage, "g2dTextureVk::Make called while buffer already made");

		vkdImageDesc desc;
		desc.m_Format = vkdFormatFromDxgi(i_DxgiFormat);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Usage = c_TextureUsage;
		if (desc.m_Format == VK_FORMAT_UNDEFINED)
		{
			DBG_ERROR("g2dTextureVk: unsupported format " << i_DxgiFormat);
			throw g2dGeneralX();
		}
		Make(vkdImage::Create(vkdGlobal::Device(), desc, "g2dTextureVk"), i_DxgiFormat, i_Category);
	}

	//------------------------------------------------------------------------
	// This object will take ownership of the image!
	//------------------------------------------------------------------------
	void g2dTextureVk::Make(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat,
							g2dResourceCounterVk::eResourceCategory i_Category /*= g2dResourceCounterVk::eTexture*/)
	{
		DBG_ASSERT(!m_pImage, "g2dTextureVk::Make called while buffer already made");
		DBG_ASSERT(i_pImage, "g2dTextureVk::Make called to wrap a NULL image");

		m_pImage = std::move(i_pImage);

		// success... set local vars
		m_Format = i_DxgiFormat;
		m_Width = (short)m_pImage->Desc().m_Width;
		m_Height = (short)m_pImage->Desc().m_Height;
		m_MipLevels = (int)m_pImage->Desc().m_Mips;
		m_Category = i_Category;
		g2dResourceCounterVk::AddResource(GetSize(), i_Category);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float g2dTextureVk::GetSize()
	{
		int bpp = g2dVkGlobal::BitsPerPixel(m_Format);
		float baseSize = (float)m_Width*(float)m_Height*(bpp/8) / 1024.0f;

		// approximation assuming all mip levels are present.
		if (m_MipLevels > 1)
			baseSize *= 1.333333f;

		return baseSize;
	}
}
