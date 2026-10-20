/****************************************************************************\
**  matReflectiveShadowMap.cpp
**
**	See matReflectiveShadowMap.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATREFLECTIVESHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matReflectiveShadowMap.hpp"
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
		const uint32_t l_ColorFormat = g2dVkGlobal::e_DXGI_R32G32B32A32_FLOAT;
	};

	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matReflectiveShadowMap::matReflectiveShadowMap()
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matReflectiveShadowMap::~matReflectiveShadowMap()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the map to regenerate its parameters from the
	//	images.
	//--------------------------------------------------------------------
	void matReflectiveShadowMap::ReloadInfo()
	{
		this->SetWidth(m_MainTexture[0]->GetWidth());
		this->SetHeight(m_MainTexture[0]->GetHeight());
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_MainTexture[0]->GetFormat(), pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions.
	//--------------------------------------------------------------------
	void matReflectiveShadowMap::Make(int i_Width, int i_Height)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		// main texture for storing depth/wpos, normal, flux
		for (int i = 0; i < e_NumRSMRenderTarget; i++)
		{
			m_MainTexture[i].reset(new g2dRenderTargetTextureVk());
			m_MainTexture[i]->Make(i_Width,i_Height,
				l_ColorFormat,
				VK_SAMPLE_COUNT_1_BIT,
				g2dResourceCounterVk::eShadow
				);
		}

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
		for (int i = 0; i < e_NumRSMRenderTarget; i++)
			vkdGlobal::Context().ClearRenderTarget(GetRenderTargetView(i), zero);

		// sync up new size and pixel format info
		ReloadInfo();
	}

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	const g2dPFD& matReflectiveShadowMap::GetPixelFormat() const
	{
		return matTexture::GetPixelFormat();
	}

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::GetDimensions(int& o_Width, int& o_Height) const
	{
		o_Width = GetWidth();
		o_Height = GetHeight();
	}

	//--------------------------------------------------------------------
	//	GetTextureSurface returns one target's image.
	//--------------------------------------------------------------------
	vkdImage* matReflectiveShadowMap::GetTextureSurface(int i_index)
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request render target view index out of range");
		return m_MainTexture[i_index]->GetImage();
	}

	//--------------------------------------------------------------------
	// Return an object pointer to use for rendering to this texture.
	//--------------------------------------------------------------------
	g2dRenderTarget* matReflectiveShadowMap::GetRenderTargetAPI()
	{
		// In this case we are using multiple inheritance, but we could
		// also have used containment and made the render target a member
		// of this class.
		return this;
	}

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::BeginScene()
	{
		// direct device to output to our window
		BeginScene(0);
	}

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::BeginScene(int i_index)
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
		configure_device(i_index);
	}

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::MakeCurrent()
	{
		// direct device to output to our window
		vkdImage* rtview[e_NumRSMRenderTarget];
		for (int i = 0; i < e_NumRSMRenderTarget; i++)
			rtview[i] = GetRenderTargetView(i);

		if (m_DepthStencil)
			vkdGlobal::Context().SetRenderTargets(e_NumRSMRenderTarget, rtview, m_DepthStencil->GetDepthView());
		else
			g2dVkGlobal::SetColorTarget(GetRenderTargetView(0));

		g2dVkGlobal::SetViewport(this->GetWidth(), this->GetHeight());
	}

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::MakeCurrent(int i_index)
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
		configure_device(i_index);
	}

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::EndScene()
	{
		// nothing needed for render target textures
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::Clear(const g2dRGBColor& i_Color)
	{
		Clear(0, i_Color);
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::Clear(int i_index, const g2dRGBColor& i_Color)
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
		configure_device(i_index);
		const float rgba[4] = { i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 0 };
		g2dVkGlobal::ClearCurrent(rgba);
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/,
										float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		// don't need to set render target to clear.
		const float c[4] = { i_Color.m_Red, i_Color.m_Green, i_Color.m_Blue, i_Color.m_Alpha };
		vkdContext& context = vkdGlobal::Context();
		for (int i = 0; i < e_NumRSMRenderTarget; i++)
			context.ClearRenderTarget(GetRenderTargetView(i), c);

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
	void matReflectiveShadowMap::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		if (this->m_DepthStencil)
		{
			configure_device(0);
			vkdGlobal::Context().ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//------------------------------------------------------------------------
	// set up device to render to our window
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::configure_device(int i_index)
	{
		if (m_DepthStencil)
			g2dVkGlobal::SetRenderTargets(GetRenderTargetView(i_index), m_DepthStencil->GetDepthView());
		else
			g2dVkGlobal::SetColorTarget(GetRenderTargetView(i_index));

		g2dVkGlobal::SetViewport(this->GetWidth(), this->GetHeight());
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matReflectiveShadowMap::GetSize() const
	{
		// get size of main surface (could be color or depth buffer)
		float mainSize = matTexture::GetSize();

		// now add in size of 2nd buffer (depth buffer or the R32 color buffer)
		// todo: add in correct other buffers' memory usage
		float otherSize = 0;

		return mainSize + otherSize;
	}

	vkdImage* matReflectiveShadowMap::GetColorBuffer() const
	{
		return GetRenderTargetView(0);
	}

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.
	//------------------------------------------------------------------------
	void matReflectiveShadowMap::MakeDepthCurrent()
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
	bool matReflectiveShadowMap::GetHasDepthBuffer() const
	{
		return m_DepthStencil != NULL;
	}

	vkdImage* matReflectiveShadowMap::GetRenderTargetView(int i_index) const
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request render target view index out of range");
		return m_MainTexture[i_index] ? m_MainTexture[i_index]->GetRenderTargetView() : NULL;
	}

	vkdImage* matReflectiveShadowMap::GetShaderView(int i_index) const
	{
		DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request surface view index out of range");
		return m_MainTexture[i_index] ? m_MainTexture[i_index]->GetImage() : NULL;
	}

	//--------------------------------------------------------------------
	//	GetImage returns the first target's image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matReflectiveShadowMap::GetImage() const
	{
		return GetShaderView(0);
	}

	shared_ptr<g2dDepthStencilBuffer> matReflectiveShadowMap::GetDepthStencilBuffer() const
	{
		return m_DepthStencil;
	}

	void matReflectiveShadowMap::SetDepthBuffer( shared_ptr<g2dDepthStencilBuffer> i_DepthStencil )
	{
		m_DepthStencil = std::dynamic_pointer_cast<g2dDepthStencilBufferVk>(i_DepthStencil);
	}
}
