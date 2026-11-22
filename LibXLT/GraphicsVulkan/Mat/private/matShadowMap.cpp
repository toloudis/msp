/****************************************************************************\
**  matShadowMap.cpp
**
**	See matShadowMap.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATSHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matShadowMap.hpp"
#endif
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_G2DRENDERTARGETTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dRenderTargetTextureVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef MAT_EXCEPTIONX_HPP
#include "Graphics/Mat/matExceptionX.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"

namespace gvk
{
	namespace
	{
		const uint32_t l_DepthFormat = g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT;
	};

	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matShadowMap::matShadowMap()
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matShadowMap::~matShadowMap()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matShadowMap to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matShadowMap::ReloadInfo()
	{
		this->SetWidth(m_MainTexture->GetWidth());
		this->SetHeight(m_MainTexture->GetHeight());
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_MainTexture->GetFormat(), pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions.
	//--------------------------------------------------------------------
	void matShadowMap::Make(int i_Width, int i_Height)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		// use floating point format for texture?
#ifdef USE_VSM_SHADOWS
		const uint32_t format = g2dVkGlobal::e_DXGI_R32G32_FLOAT;
#else
		const uint32_t format = g2dVkGlobal::e_DXGI_R32_FLOAT;
#endif

		m_MainTexture.reset(new g2dRenderTargetTextureVk());
		m_MainTexture->Make(i_Width,i_Height,
			format,
			VK_SAMPLE_COUNT_1_BIT,
			g2dResourceCounterVk::eShadow
		);

		// Set up depth buffer
		m_DepthStencil.reset(new g2dDepthStencilBufferVk());
		m_DepthStencil->Make(i_Width,i_Height,
			l_DepthFormat,
			VK_SAMPLE_COUNT_1_BIT,
			g2dResourceCounterVk::eShadow
		);

		//	A map no light has rendered yet is read as D3D11 gives it in
		//	practice (zeroed memory), not as whatever the allocation held
		const float zero[4] = { 0, 0, 0, 0 };
		vkdGlobal::Context().ClearRenderTarget(GetColorBuffer(), zero);

		// sync up new size and pixel format info
		ReloadInfo();
	}

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	const g2dPFD& matShadowMap::GetPixelFormat() const
	{
		return matTexture::GetPixelFormat();
	}

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	void matShadowMap::GetDimensions(int& o_Width, int& o_Height) const
	{
		o_Width = GetWidth();
		o_Height = GetHeight();
	}

	//--------------------------------------------------------------------
	// Return an object pointer to use for rendering to this texture.
	//--------------------------------------------------------------------
	g2dRenderTarget* matShadowMap::GetRenderTargetAPI()
	{
		// In this case we are using multiple inheritance, but we could
		// also have used containment and made the render target a member
		// of this class.
		return this;
	}

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	void matShadowMap::BeginScene()
	{
		// direct device to output to our window
		configure_device();
	}

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	void matShadowMap::MakeCurrent()
	{
		// direct device to output to our window
		configure_device();
	}

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.
	//------------------------------------------------------------------------
	void matShadowMap::EndScene()
	{
		// nothing needed for render target textures
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void matShadowMap::Clear(const g2dRGBColor& i_Color)
	{
		configure_device();
		const float rgba[4] = { i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 0 };
		g2dVkGlobal::ClearCurrent(rgba);
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	void matShadowMap::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/,
							  float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		// don't need to set render target to clear.
		const float c[4] = { i_Color.m_Red, i_Color.m_Green, i_Color.m_Blue, i_Color.m_Alpha };
		vkdContext& context = vkdGlobal::Context();
		context.ClearRenderTarget(GetColorBuffer(), c);

		// temp: does it work to clear here?
		if (i_ClearDepth && m_DepthStencil)
		{
			context.ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	void matShadowMap::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		if (this->m_DepthStencil)
		{
			configure_device();
			vkdGlobal::Context().ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//------------------------------------------------------------------------
	// set up device to render to our window
	//------------------------------------------------------------------------
	void matShadowMap::configure_device()
	{
		DBG_ASSERT(m_MainTexture && m_DepthStencil,
			"Need to call Make() before rendering to texture");
		if (m_DepthStencil)
			g2dVkGlobal::SetRenderTargets(GetColorBuffer(), m_DepthStencil->GetDepthView());
		else
			g2dVkGlobal::SetColorTarget(GetColorBuffer());

		g2dVkGlobal::SetViewport(this->GetWidth(), this->GetHeight());
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matShadowMap::GetSize() const
	{
		// get size of main surface (could be color or depth buffer)
		float mainSize = matTexture::GetSize();

		// now add in size of 2nd buffer (depth buffer or the R32 color buffer)
		float otherSize = 0;

		return mainSize + otherSize;
	}

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.
	//------------------------------------------------------------------------
	void matShadowMap::MakeDepthCurrent()
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
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	bool matShadowMap::GetHasDepthBuffer() const
	{
		return m_DepthStencil != NULL;
	}

	//--------------------------------------------------------------------
	//	GetTextureSurface returns the main image.
	//--------------------------------------------------------------------
	vkdImage* matShadowMap::GetTextureSurface()
	{
		return m_MainTexture->GetImage();
	}
	vkdImage* matShadowMap::GetColorBuffer() const
	{
		return m_MainTexture ? m_MainTexture->GetRenderTargetView() : NULL;
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matShadowMap::GetImage() const
	{
		return m_MainTexture ? m_MainTexture->GetImage() : NULL;
	}

	shared_ptr<g2dDepthStencilBuffer> matShadowMap::GetDepthStencilBuffer() const
	{
		return m_DepthStencil;
	}
}
