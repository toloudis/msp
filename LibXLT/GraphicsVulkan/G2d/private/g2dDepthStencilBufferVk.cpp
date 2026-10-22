/****************************************************************************\
**  g2dDepthStencilBufferVk.cpp
**
**	See g2dDepthStencilBufferVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	g2dDepthStencilBufferVk::g2dDepthStencilBufferVk(vkdDevice* i_pDevice)
	:	m_Category(g2dResourceCounterVk::eRenderTarget),
		m_Width(0),
		m_Height(0),
		m_Format(0),
		m_Multisample(VK_SAMPLE_COUNT_1_BIT),
		m_pDevice(i_pDevice)
	{
	}

	g2dDepthStencilBufferVk::~g2dDepthStencilBufferVk()
	{
		g2dResourceCounterVk::RemoveResource(GetSize(), m_Category);
	}

	//------------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//------------------------------------------------------------------------
	void g2dDepthStencilBufferVk::Make(int i_Width, int i_Height, uint32_t i_DxgiFormat, VkSampleCountFlagBits i_Multisample,
									   g2dResourceCounterVk::eResourceCategory i_Category/*= g2dResourceCounterVk::eRenderTarget*/)
	{
		DBG_ASSERT(!m_pDepthTexture, "g2dDepthStencilBufferVk::Make called while buffer already made");

		//	24 bit depth and 8 bit stencil, written as depth-stencil and read
		//	(depth only) by shaders, whatever i_DxgiFormat says, as DX11
		vkdImageDesc desc;
		desc.m_Format = VK_FORMAT_D24_UNORM_S8_UINT;
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Samples = i_Multisample;
		desc.m_Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
					   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		desc.m_Aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		m_pDepthTexture = vkdImage::Create(m_pDevice ? *m_pDevice : vkdGlobal::Device(), desc, "g2dDepthStencilBufferVk");

		// success... set local vars
		m_Format = i_DxgiFormat;
		m_Width = i_Width;
		m_Height = i_Height;
		m_Multisample = i_Multisample;

		m_Category = i_Category;
		g2dResourceCounterVk::AddResource(GetSize(), i_Category);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float g2dDepthStencilBufferVk::GetSize()
	{
		// assume 32bit depth buffer is always here.
		int bpp = 32;
		return (float)m_Width*(float)m_Height*(bpp/8) / 1024.0f;
	}

	//	The depth aspect, as DX11's R24_UNORM_X8_TYPELESS view
	vkdImage* g2dDepthStencilBufferVk::GetDepthShaderResource() const
	{
		return m_pDepthTexture ? m_pDepthTexture->DepthView() : NULL;
	}
}
