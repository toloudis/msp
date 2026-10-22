/****************************************************************************\
**  matPlainTexture.cpp
**
**	See matPlainTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATPLAINTEXTURE_HPP
#include "GraphicsVulkan/Mat/matPlainTexture.hpp"
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
#ifndef MAT_EXCEPTIONX_HPP
#include "Graphics/Mat/matExceptionX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matPlainTexture::matPlainTexture()
	{

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matPlainTexture::~matPlainTexture()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matPlainTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matPlainTexture::ReloadInfo()
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
	void matPlainTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
							   void* i_PixelData /*= NULL*/)
	{
		if (g2dVkGlobal::RestrictPow2Textures())
		{
			if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
			{
				throw matInvalidTextureSizeX(fsLocator());
			}
		}

		shared_ptr<g2dTextureVk> new_texture(new g2dTextureVk);
		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);
		new_texture->Make(i_Width, i_Height, format);
		if (i_PixelData)
		{
			Upload(new_texture->GetImage(), format, i_PixelData, i_Width, i_Height);
		}

		// if we get to this point, the allocation succeeded and we can get rid of our old surface
		m_Texture = new_texture;
		this->SetWidth(i_Width);
		this->SetHeight(i_Height);
		this->SetPixelFormat(i_PFD);
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matPlainTexture::GetImage() const
	{
		return m_Texture ? m_Texture->GetImage() : NULL;
	}

	//--------------------------------------------------------------------
	//	SetSurface replaces the image
	//--------------------------------------------------------------------
	void matPlainTexture::SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat)
	{
		// out with the old, in with the new
		m_Texture.reset(new g2dTextureVk);
		m_Texture->Make(std::move(i_pImage), i_DxgiFormat);
	}
}
