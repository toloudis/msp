/****************************************************************************\
**  g2dRenderTargetTextureVk.cpp
**
**	See g2dRenderTargetTextureVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DRENDERTARGETTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dRenderTargetTextureVk.hpp"
#endif
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
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	g2dRenderTargetTextureVk::g2dRenderTargetTextureVk()
	:	m_Category(g2dResourceCounterVk::eRenderTarget),
		m_Width(0),
		m_Height(0),
		m_Mips(0),
		m_Format(0),
		m_Multisample(VK_SAMPLE_COUNT_1_BIT)
	{
	}

	g2dRenderTargetTextureVk::~g2dRenderTargetTextureVk()
	{
	}

	//------------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//------------------------------------------------------------------------
	void g2dRenderTargetTextureVk::Make(int i_Width, int i_Height, uint32_t i_DxgiFormat, VkSampleCountFlagBits i_Multisample,
										g2dResourceCounterVk::eResourceCategory i_Category/*= g2dResourceCounterVk::eRenderTarget*/,
										bool i_bGenerateMips /*= false*/)
	{
		DBG_ASSERT(!m_pImage, "g2dRenderTargetTextureVk::Make called while buffer already made");

		//------Create texture and view
		vkdImageDesc desc;
		desc.m_Format = vkdFormatFromDxgi(i_DxgiFormat);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Mips = i_bGenerateMips ? g2dFullMipCount(i_Width, i_Height) : 1;
		desc.m_Samples = i_Multisample;
		desc.m_Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | c_TextureUsage;
		if (desc.m_Format == VK_FORMAT_UNDEFINED)
		{
			DBG_ERROR("g2dRenderTargetTextureVk: unsupported format " << i_DxgiFormat);
			throw g2dGeneralX();
		}
		m_pImage = vkdImage::Create(vkdGlobal::Device(), desc, "g2dRenderTargetTextureVk");

		// success... set local vars
		m_Format = i_DxgiFormat;
		m_Width = i_Width;
		m_Height = i_Height;
		m_Multisample = i_Multisample;
		m_Mips = i_bGenerateMips ? 0 : 1;
		m_Category = i_Category;
	}

	//	D3D renders to mip 0 of a render target view of the whole texture;
	//	a Vulkan attachment is a view of one mip
	vkdImage* g2dRenderTargetTextureVk::GetRenderTargetView() const
	{
		if (!m_pImage)
		{
			return NULL;
		}
		return m_pImage->Desc().m_Mips > 1 ? m_pImage->Subresource(0, 0) : m_pImage.get();
	}
}
