/****************************************************************************\
**  matRenderTargetTexture.cpp
**
**	See matRenderTargetTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
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
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
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

#include <mutex>

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matRenderTargetTexture::matRenderTargetTexture(bool i_bStoreDepths, bool i_bFloatDepth)
	:	m_bStoreDepths(i_bStoreDepths), m_bFloatDepth( i_bFloatDepth ),
		m_bHasMipMaps(false)
	{
		m_bNoDepthRequested = false;
		m_bAA = false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matRenderTargetTexture::~matRenderTargetTexture()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matRenderTargetTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matRenderTargetTexture::ReloadInfo()
	{
		this->SetWidth(m_RenderTarget->GetWidth());
		this->SetHeight(m_RenderTarget->GetHeight());
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_RenderTarget->GetFormat(), pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//--------------------------------------------------------------------
	void matRenderTargetTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
									  bool i_bAllocDepthBuf /*=true*/,
									  bool i_bAutoGenMipmap /*= false*/,
									  g2dResourceCounterVk::eResourceCategory i_Category/*= g2dResourceCounterVk::eRenderTarget*/,
									  bool i_bAntiAlias /*= false*/)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		// use floating point format for texture?
		const uint32_t format = (m_bStoreDepths) ? g2dVkGlobal::e_DXGI_R32_FLOAT : g2dVkGlobal::DxgiFormatFromPFD(i_PFD);

		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		if( i_bAntiAlias )
		{
			samples = g2dVkGlobal::ChooseSampleCount( vkdFormatFromDxgi(format) );
		}

		m_RenderTarget.reset(new g2dRenderTargetTextureVk());
		m_RenderTarget->Make(i_Width,i_Height,
			format,
			samples,
			i_Category,
			i_bAutoGenMipmap
		);

		m_bHasMipMaps = i_bAutoGenMipmap;
		m_bAA = i_bAntiAlias;

		// Set up depth buffer
		if (i_bAllocDepthBuf)
		{
			m_DepthStencil.reset(new g2dDepthStencilBufferVk());
			m_DepthStencil->Make(i_Width,i_Height,
				m_bFloatDepth ? g2dVkGlobal::e_DXGI_D32_FLOAT : g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT,
				samples,
				i_Category
			);
		}

		if (i_bAllocDepthBuf == false)
			m_bNoDepthRequested = true;

		//	A target nothing has drawn into yet reads as D3D11 gives it in
		//	practice (zeroed memory), not as whatever the allocation held:
		//	the opacity pass, as DX11's, clears only the first of a light's
		//	opacity maps and adds into the others
		const float zero[4] = { 0, 0, 0, 0 };
		vkdGlobal::Context().ClearRenderTarget(GetColorBuffer(), zero);

		// sync up new size and pixel format info
		ReloadInfo();
	}

	//------------------------------------------------------------------------
	//	Resolve will copy the contents to the given texture, resolving
	//	the multisample surface to a single sampled one.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::Resolve(matRenderTargetTexture* o_pTexture)
	{
		DBG_ASSERT( m_bAA && (GetWidth() == o_pTexture->GetWidth()) &&
			(GetHeight() == o_pTexture->GetHeight()) &&
			(m_RenderTarget->GetFormat() == o_pTexture->m_RenderTarget->GetFormat()), "Texture parameters don't match for Resolve!" );

		ResolveImage(m_RenderTarget->GetImage(), o_pTexture->GetTextureSurface());
	}

	//------------------------------------------------------------------------
	//	ResolveSubresource of mip 0
	//------------------------------------------------------------------------
	void matRenderTargetTexture::ResolveImage(vkdImage* i_pSource, vkdImage* i_pDest)
	{
		vkdContext& context = vkdGlobal::Context();
		std::lock_guard<std::recursive_mutex> lock(vkdGlobal::Device().StreamLock());
		VkImageResolve region = {};
		region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.extent = { i_pSource->Width(), i_pSource->Height(), 1 };
		vkCmdResolveImage(context.Commands(), i_pSource->Image(), VK_IMAGE_LAYOUT_GENERAL, i_pDest->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
	}

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	const g2dPFD& matRenderTargetTexture::GetPixelFormat() const
	{
		return matTexture::GetPixelFormat();
	}

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::GetDimensions(int& o_Width, int& o_Height) const
	{
		o_Width = GetWidth();
		o_Height = GetHeight();
	}

	//--------------------------------------------------------------------
	// Return an object pointer to use for rendering to this texture.
	//--------------------------------------------------------------------
	g2dRenderTarget* matRenderTargetTexture::GetRenderTargetAPI()
	{
		// In this case we are using multiple inheritance, but we could
		// also have used containment and made the render target a member
		// of this class.
		return this;
	}

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	void matRenderTargetTexture::BeginScene()
	{
		// direct device to output to our window
		configure_device();
	}

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::MakeCurrent()
	{
		// direct device to output to our window
		configure_device();
	}

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::EndScene()
	{
		// nothing needed for render target textures
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::Clear(const g2dRGBColor& i_Color)
	{
		configure_device();
		const float rgba[4] = { i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 0 };
		g2dVkGlobal::ClearCurrent(rgba);
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	void matRenderTargetTexture::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/,
									   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		// don't need to set render target to clear.
		const float c[4] = { i_Color.m_Red, i_Color.m_Green, i_Color.m_Blue, i_Color.m_Alpha };
		vkdContext& context = vkdGlobal::Context();
		context.ClearRenderTarget(GetColorBuffer(), c);

		if (i_ClearDepth && m_DepthStencil)
		{
			context.ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
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
	void matRenderTargetTexture::configure_device()
	{
		DBG_ASSERT(m_RenderTarget && (m_DepthStencil || m_bNoDepthRequested),
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
	float matRenderTargetTexture::GetSize() const
	{
		// both resource objects are memory managed at a lower level.
		return 0;
	}

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.
	//------------------------------------------------------------------------
	void matRenderTargetTexture::MakeDepthCurrent()
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
	//------------------------------------------------------------------------
	vkdImage* matRenderTargetTexture::GetDepthBuffer() const
	{
		return m_DepthStencil ? m_DepthStencil->GetDepthView() : NULL;
	}
	vkdImage* matRenderTargetTexture::GetColorBuffer() const
	{
		return m_RenderTarget ? m_RenderTarget->GetRenderTargetView() : NULL;
	}

	//--------------------------------------------------------------------
	//	GetTextureSurface returns the main image.
	//--------------------------------------------------------------------
	vkdImage* matRenderTargetTexture::GetTextureSurface()
	{
		return m_RenderTarget ? m_RenderTarget->GetImage() : NULL;
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matRenderTargetTexture::GetImage() const
	{
		return m_RenderTarget ? m_RenderTarget->GetImage() : NULL;
	}

	bool matRenderTargetTexture::CopyWithResolve( matRenderTargetTexture* o_pTexture, bool bNoResolve /*= false */, bool bResolveDepth /*= false*/ )
	{
		bool rVal = true;

		if( this == o_pTexture ) DBG_TRACE( "CopyWithResolve cannot copy same target!" );

		//direct copy if matching dimensions and format
		int iW, iH;
		int oW, oH;

		GetDimensions( iW, iH );
		o_pTexture->GetDimensions( oW, oH );

		//resolve if downsampling is required
		if( !bNoResolve && IsAA() && !o_pTexture->IsAA() )
		{
			Resolve( o_pTexture );
		}
		else if( (iH == oH) && GetPixelFormat().GetPixelFormat() == o_pTexture->GetPixelFormat().GetPixelFormat() )
		{
			//	CopySubresourceRegion of mip 0 to (0, 0)
			std::lock_guard<std::recursive_mutex> lock(vkdGlobal::Device().StreamLock());
			VkImageCopy region = {};
			region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			region.extent = { (uint32_t)(iW < oW ? iW : oW), (uint32_t)iH, 1 };
			vkCmdCopyImage(vkdGlobal::Context().Commands(), GetTextureSurface()->Image(), VK_IMAGE_LAYOUT_GENERAL,
						   o_pTexture->GetTextureSurface()->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		}
		else if( !IsAA() )	//resort to a scaled copy if source is not MSAA
		{
			g3dVkUtil::CopyTexToTarget( this, o_pTexture );
		}
		else
		{
			rVal = false;
		}

		//Resolve/Copy only if different
		if( bResolveDepth && GetDepthStencilBuffer() != o_pTexture->GetDepthStencilBuffer() )
		{
			//Make sure both have depths and that target is non MSAA
			if( GetHasDepthBuffer() && o_pTexture->GetHasDepthBuffer() && !o_pTexture->IsAA() )
			{
				g3dVkUtil::CopyDepth( this, o_pTexture );
			}
			else rVal = false;	//can't resolve/copy if requested
		}

		return rVal;
	}

	shared_ptr<g2dDepthStencilBuffer> matRenderTargetTexture::GetDepthStencilBuffer() const
	{
		return m_DepthStencil;
	}

	void matRenderTargetTexture::SetDepthBuffer( shared_ptr<g2dDepthStencilBuffer> i_DepthStencil )
	{
		m_DepthStencil = std::dynamic_pointer_cast<g2dDepthStencilBufferVk>(i_DepthStencil);
	}
}
