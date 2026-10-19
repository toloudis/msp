/****************************************************************************\
**  matVolumeTexture.cpp
**
**	See matVolumeTexture.hpp.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATVOLUMETEXTURE_HPP
#include "GraphicsVulkan/Mat/matVolumeTexture.hpp"
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
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	matVolumeTexture::matVolumeTexture()
	:	m_bUnorderedAccess(false), m_NumLayers(0), m_Format(0)
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matVolumeTexture::~matVolumeTexture()
	{
	}

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matVolumeTexture to regenerate its
	//	parameters from the image.
	//--------------------------------------------------------------------
	void matVolumeTexture::ReloadInfo()
	{
		const vkdImageDesc& desc = m_pImage->Desc();

		// Find the volume texture's number of layers:
		m_NumLayers = desc.m_Depth;
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
	void matVolumeTexture::Make(int i_Width, int i_Height, int i_Depth, const g2dPFD& i_PFD, BIND_TYPE i_Bindings, void* i_pInitialData )
	{
		const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD(i_PFD);
		vkdImageDesc desc;
		desc.m_Type = VK_IMAGE_TYPE_3D;
		desc.m_Format = vkdFormatFromDxgi(format);
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Depth = i_Depth;
		desc.m_Usage = c_TextureUsage | ((i_Bindings & BIND_UNORDERED_ACCESS) ? VK_IMAGE_USAGE_STORAGE_BIT : 0);
		if (desc.m_Format == VK_FORMAT_UNDEFINED)
		{
			DBG_ERROR("matVolumeTexture: unsupported format " << format);
			throw g2dGeneralX();
		}
		std::unique_ptr<vkdImage> new_texture = vkdImage::Create(vkdGlobal::Device(), desc, "matVolumeTexture");

		// big assumption: initial data is packed tightly and pitch
		// is related directly to pixel size for this format
		if (i_pInitialData)
		{
			Upload(new_texture.get(), format, i_pInitialData, i_Width, i_Height, i_Depth);
		}

		// if we get to this point, the allocation succeeded and we can get rid of our old surface
		SetSurface(std::move(new_texture), format);
		m_bUnorderedAccess = (i_Bindings & BIND_UNORDERED_ACCESS) != 0;
		this->ReloadInfo();
	}

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in bytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float matVolumeTexture::GetSize() const
	{
		// volume texture is w x h x d x Bpp
		float face_size = matTexture::GetSize();
		float volume_size = face_size * m_NumLayers;

		// also, account for possiblity of mipmaps.
		if (m_pImage && m_pImage->Desc().m_Mips > 1)
			return (volume_size * 4) / 3;

		return volume_size;
	}

	//--------------------------------------------------------------------
	//	GetImage returns the image, usable for shaders.
	//--------------------------------------------------------------------
	vkdImage* matVolumeTexture::GetImage() const
	{
		return m_pImage.get();
	}

	//--------------------------------------------------------------------
	//	SetSurface replaces the image
	//--------------------------------------------------------------------
	void matVolumeTexture::SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat)
	{
		// out with the old, in with the new
		if (i_pImage && i_pImage->Desc().m_Type == VK_IMAGE_TYPE_3D)
		{
			m_pImage = std::move(i_pImage);
			m_Format = i_DxgiFormat;
			m_bUnorderedAccess = false;
		}
		else
		{
			DBG_ERROR("SetSurface called with incorrect resource type");
		}
	}
}
