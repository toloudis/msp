/****************************************************************************\
**  g2dRenderTargetVk.cpp
**
**	See g2dRenderTargetVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DRENDERTARGETVK_HPP
#include "GraphicsVulkan/G2d/g2dRenderTargetVk.hpp"
#endif
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_G2DTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dTextureVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	g2dRenderTargetVk::g2dRenderTargetVk()
	:	m_Width(0),
		m_Height(0)
	{
	}

	g2dRenderTargetVk::~g2dRenderTargetVk()
	{
		g2dResourceCounterVk::RemoveResource(GetSize(), g2dResourceCounterVk::eRenderTarget);
	}

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::BeginScene()
	{
	}

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::MakeCurrent()
	{
		// direct device to output to our window
		configure_device();
	}

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::MakeDepthCurrent()
	{
		if (m_DepthStencil)
		{
			g2dVkGlobal::SetDepthTarget(m_DepthStencil->GetDepthView());
		}
		else
		{
			DBG_TRACE("MakeDepthCurrent called with no depth buffer.");
		}
	}

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::EndScene()
	{
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::Clear(const g2dRGBColor& i_Color)
	{
		configure_device();
		const float rgba[4] = { i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 0 };
		g2dVkGlobal::ClearCurrent(rgba);
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*=false*/, float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		configure_device();
		const float rgba[4] = { i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha() };
		g2dVkGlobal::ClearCurrent(rgba, i_ClearDepth && m_DepthStencil, i_Depth, i_ClearStencil, i_Stencil);
	}

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		if (m_DepthStencil)
		{
			configure_device();
			vkdGlobal::Context().ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	const g2dPFD& g2dRenderTargetVk::GetPixelFormat() const
	{
		return m_PFD;
	}

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::GetDimensions(int& o_Width, int& o_Height) const
	{
		o_Width = m_Width;
		o_Height = m_Height;
	}

	//------------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, bool i_AntiAlias /*= false*/)
	{
		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);
		const VkFormat vk_format = vkdFormatFromDxgi(format);
		const VkSampleCountFlagBits samples = i_AntiAlias ? g2dVkGlobal::ChooseSampleCount(vk_format) : VK_SAMPLE_COUNT_1_BIT;

		//------Create Target texture and view
		vkdImageDesc desc;
		desc.m_Format = vk_format;
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Samples = samples;
		desc.m_Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		m_pTarget = vkdImage::Create(vkdGlobal::Device(), desc, "g2dRenderTargetVk");

		//------Create Depth/Stencil texture and view
		m_DepthStencil.reset(new g2dDepthStencilBufferVk());
		m_DepthStencil->Make(i_Width, i_Height, g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT, samples);

		// success... set local vars
		m_PFD = i_PFD;
		m_Width = i_Width;
		m_Height = i_Height;

		g2dResourceCounterVk::AddResource(GetSize(), g2dResourceCounterVk::eRenderTarget);
	}

	//------------------------------------------------------------------------
	//	Resolve will copy the contents to the given texture, resolving
	//	the multisample surface to a single sampled one.
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::Resolve(matRenderTargetTexture* o_pTexture)
	{
		matRenderTargetTexture::ResolveImage(m_pTarget.get(), o_pTexture->GetTextureSurface());
	}

	//------------------------------------------------------------------------
	// set up device to render to our window
	//------------------------------------------------------------------------
	void g2dRenderTargetVk::configure_device()
	{
		DBG_ASSERT(m_pTarget && m_DepthStencil,
			"Need to call Make() before rendering to target");
		g2dVkGlobal::SetRenderTargets(m_pTarget.get(), m_DepthStencil->GetDepthView());
		g2dVkGlobal::SetViewport(m_Width, m_Height);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float g2dRenderTargetVk::GetSize()
	{
		// assume 32bit depth buffer is always here.
		return m_Width*m_Height*(m_PFD.BitsPerPixel()/8 + 32/8) / 1024.0f;
	}

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	bool g2dRenderTargetVk::GetHasDepthBuffer() const
	{
		return m_DepthStencil != NULL;
	}

	shared_ptr<g2dDepthStencilBuffer> g2dRenderTargetVk::GetDepthStencilBuffer() const
	{
		return m_DepthStencil;
	}
}
