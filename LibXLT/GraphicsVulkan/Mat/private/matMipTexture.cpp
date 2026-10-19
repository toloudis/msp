/****************************************************************************\
**  matMipTexture.cpp
**
**	See matMipTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATMIPTEXTURE_HPP
#include "GraphicsVulkan/Mat/matMipTexture.hpp"
#endif
#ifndef GVK_G2DTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dTextureVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
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
#ifndef MAT_EXCEPTIONX_HPP
#include "Graphics/Mat/matExceptionX.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matMipTexture::matMipTexture()
	{

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matMipTexture::~matMipTexture()
	{
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matMipTexture::GetSize() const
	{
		float base_size = matTexture::GetSize();

		// mip-map texture size is about 1/3 bigger than that of normal texture
		return (base_size * 4) / 3;
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matMipTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matMipTexture::ReloadInfo()
	{
		this->SetWidth(m_Texture->GetWidth());
		this->SetHeight(m_Texture->GetHeight());
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_Texture->GetFormat(), pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//--------------------------------------------------------------------
	void matMipTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, int i_NumMipLevels,
							 void* i_PixelData /*= NULL*/)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				DBG_ERROR("Invalid texture size " << i_Width << "x" << i_Height);
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);
		vkdImageDesc desc;
		desc.m_Format = vkdFormatFromDxgi(format);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Mips = i_NumMipLevels > 0 ? i_NumMipLevels : g2dFullMipCount(i_Width, i_Height);
		desc.m_Usage = c_TextureUsage;
		if (desc.m_Format == VK_FORMAT_UNDEFINED)
		{
			DBG_ERROR("matMipTexture: unsupported format " << format);
			throw g2dGeneralX();
		}
		std::unique_ptr<vkdImage> image = vkdImage::Create(vkdGlobal::Device(), desc, "matMipTexture");
		if (i_PixelData)
		{
			//	D3D11 would read initial data for every mip from one level's
			//	(past its end); here the rest are made from mip 0
			Upload(image.get(), format, i_PixelData, i_Width, i_Height);
			if (desc.m_Mips > 1)
			{
				image->GenerateMips(vkdGlobal::Context());
			}
		}

		// if we get to this point, the allocation succeeded and we can get rid of our old surface
		this->SetSurface(std::move(image), format);
		this->SetWidth(i_Width);
		this->SetHeight(i_Height);
		this->SetPixelFormat(i_PFD);
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matMipTexture::GetImage() const
	{
		return m_Texture ? m_Texture->GetImage() : NULL;
	}

	//--------------------------------------------------------------------
	//	SetSurface replaces the image
	//--------------------------------------------------------------------
	void matMipTexture::SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat)
	{
		// out with the old, in with the new
		m_Texture.reset(new g2dTextureVk);
		m_Texture->Make(std::move(i_pImage), i_DxgiFormat);
	}

	//--------------------------------------------------------------------
	//	GetMipLevels gets the current texture mip level
	//--------------------------------------------------------------------
	int matMipTexture::GetMipLevels()
	{
		return m_Texture ? m_Texture->GetMipLevels() : 0;
	}
}
