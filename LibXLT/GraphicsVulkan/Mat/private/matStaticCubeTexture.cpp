/****************************************************************************\
**  matStaticCubeTexture.cpp
**
**	See matStaticCubeTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATSTATICCUBETEXTURE_HPP
#include "GraphicsVulkan/Mat/matStaticCubeTexture.hpp"
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
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matStaticCubeTexture::matStaticCubeTexture()
	:	m_Format(0)
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matStaticCubeTexture::~matStaticCubeTexture()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matStaticCubeTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matStaticCubeTexture::ReloadInfo()
	{
		const vkdImageDesc& desc = m_pImage->Desc();

	    DBG_ASSERT(desc.m_Layers == 6, "matStaticCubeTexture is not a true cube texture");

		this->SetWidth(desc.m_Width);
		this->SetHeight(desc.m_Height);
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(m_Format, pfd);
		this->SetPixelFormat(pfd);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.
	//--------------------------------------------------------------------
	void matStaticCubeTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD)
	{
		DBG_ASSERT(i_Width == i_Height, "Width must equal height for a cube texture");

		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);

		// cube tex is array of 6 2d tex
		vkdImageDesc desc;
		desc.m_Format = vkdFormatFromDxgi(format);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Mips = g2dFullMipCount(i_Width, i_Height);
		desc.m_Layers = 6;
		desc.m_bCube = true;
		desc.m_Usage = c_TextureUsage;

		// if we get to this point, the allocation succeeded and we can get rid of our old surface
		this->SetSurface(vkdImage::Create(vkdGlobal::Device(), desc, "matStaticCubeTexture"), format);
		this->ReloadInfo();
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matStaticCubeTexture::GetSize() const
	{
		// cube map texture size is 6 times bigger, accounting for the 6 cube faces.
		float face_size = matTexture::GetSize();
		float cube_size = face_size * 6;

		// also, account for possiblity of mipmaps.
		if (m_pImage && m_pImage->Desc().m_Mips > 1)
			return (cube_size * 4) / 3;

		return cube_size;
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matStaticCubeTexture::GetImage() const
	{
		return m_pImage.get();
	}

	//--------------------------------------------------------------------
	//	SetSurface replaces the image
	//--------------------------------------------------------------------
	void matStaticCubeTexture::SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat)
	{
		// out with the old, in with the new
		if (i_pImage && i_pImage->Desc().m_bCube)
		{
			m_pImage = std::move(i_pImage);
			m_Format = i_DxgiFormat;
		}
		else
		{
			DBG_ERROR("SetSurface called with incorrect resource type");
		}
	}
}
