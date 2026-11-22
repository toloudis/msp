/****************************************************************************\
**  matCubeRenderTargetTexture.cpp
**
**	See matCubeRenderTargetTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATCUBERENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matCubeRenderTargetTexture.hpp"
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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
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
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
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

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matCubeRenderTargetTexture::matCubeRenderTargetTexture()
	:	matRenderTargetTexture(false, false),
		m_CurrentFaceTarget(0),
		m_DxgiFormat(0)
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matCubeRenderTargetTexture::~matCubeRenderTargetTexture()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matCubeRenderTargetTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matCubeRenderTargetTexture::ReloadInfo()
	{
		const vkdImageDesc& desc = m_pCubeImage->Desc();
		this->SetWidth(desc.m_Width);
		this->SetHeight(desc.m_Height);
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_DxgiFormat, pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dGeneralX, and leave the old surface intact.
	//--------------------------------------------------------------------
	void matCubeRenderTargetTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
										  bool i_bAllocDepthBuf/*=true*/)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		DBG_ASSERT(i_Width == i_Height, "Width must equal height for a cube texture");

		// cube tex is array of 6 2d tex, with a full mip chain (D3D11's MipLevels 0)
		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);
		vkdImageDesc desc;
		desc.m_Format = vkdFormatFromDxgi(format);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Mips = g2dFullMipCount(i_Width, i_Height);
		desc.m_Layers = 6;
		desc.m_bCube = true;
		desc.m_Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | c_TextureUsage;
		if (desc.m_Format == VK_FORMAT_UNDEFINED)
		{
			DBG_ERROR("matCubeRenderTargetTexture: unsupported pixel format");
			throw g2dGeneralX();
		}
		std::unique_ptr<vkdImage> new_image = vkdImage::Create(vkdGlobal::Device(), desc, "matCubeRenderTargetTexture");

		// D3D11 creates the texture filled with zeros
		const float zero[4] = { 0, 0, 0, 0 };
		vkdGlobal::Context().ClearRenderTarget(new_image.get(), zero);

		// if we get to this point, the allocation succeeded and we can get rid of our old surface
		m_bHasMipMaps = true;
		m_pCubeImage = std::move(new_image);
		m_DxgiFormat = format;
		this->ReloadInfo();

		// Set up depth buffer
		if (i_bAllocDepthBuf)
		{
			m_DepthStencil.reset(new g2dDepthStencilBufferVk());
			m_DepthStencil->Make(i_Width,i_Height,
				g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT,
				VK_SAMPLE_COUNT_1_BIT
			);
		}

		if (i_bAllocDepthBuf == false)
			m_bNoDepthRequested = true;

		// sync up new size and pixel format info
		ReloadInfo();
		SetFaceTarget(0);
	}

	//------------------------------------------------------------------------
	// set one of the 6 faces as the current render target.
	//------------------------------------------------------------------------
	void matCubeRenderTargetTexture::SetFaceTarget(int i_CubeFace)
	{
		DBG_ASSERT(i_CubeFace>=0 && i_CubeFace<6, "Set bad cube face index");
		m_CurrentFaceTarget = i_CubeFace;
		configure_device();
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void matCubeRenderTargetTexture::Clear(const g2dRGBColor& i_Color)
	{
		for (int i = 0; i < 6; i++)
		{
			SetFaceTarget(i);
			const float rgba[4] = { i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 0 };
			g2dVkGlobal::ClearCurrent(rgba);
		}
	}

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	void matCubeRenderTargetTexture::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/,
										   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
	{
		// don't need to set render target to clear.
		const float c[4] = { i_Color.m_Red, i_Color.m_Green, i_Color.m_Blue, i_Color.m_Alpha };
		vkdContext& context = vkdGlobal::Context();
		context.ClearRenderTarget(face(m_CurrentFaceTarget), c);

		// temp: does it work to clear here?
		if (i_ClearDepth && m_DepthStencil)
		{
			context.ClearDepthStencil(m_DepthStencil->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matCubeRenderTargetTexture::GetSize() const
	{
		// 6 color buffers and 1 depth buffer.
		// get size of main surface, times 6 for the cube faces.
		float mainSize = matTexture::GetSize() * 6;

		// we always generate mipmaps. see Make().
		mainSize = (mainSize * 4) / 3;

		// Don't add the size of a depthbuffer because it automatically adds itself
		// when created.
		return mainSize;
	}

	//------------------------------------------------------------------------
	// set up device to render to our window
	//------------------------------------------------------------------------
	void matCubeRenderTargetTexture::configure_device()
	{
		DBG_ASSERT(m_pCubeImage && (m_DepthStencil || m_bNoDepthRequested),
			"Need to call Make() before rendering to texture");
		if (m_DepthStencil)
			g2dVkGlobal::SetRenderTargets(face(m_CurrentFaceTarget), m_DepthStencil->GetDepthView());
		else
			g2dVkGlobal::SetColorTarget(face(m_CurrentFaceTarget));

		g2dVkGlobal::SetViewport(this->GetWidth(), this->GetHeight());
	}

	//--------------------------------------------------------------------
	//	GetImage returns the cube image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matCubeRenderTargetTexture::GetImage() const
	{
		return m_pCubeImage.get();
	}

	vkdImage* matCubeRenderTargetTexture::face(int i_CubeFace) const
	{
		return m_pCubeImage ? m_pCubeImage->Subresource(0, i_CubeFace) : NULL;
	}
}
