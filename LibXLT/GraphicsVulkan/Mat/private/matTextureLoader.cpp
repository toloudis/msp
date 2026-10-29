/****************************************************************************\
**  matTextureLoader.cpp
**
**	See matTextureLoader.hpp: the ports of matTextureLoaderWin,
**	matTextureLoaderTGAWin and matTextureLoaderHDRWin (one file: they
**	share the decode, reduce and mip steps).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATTEXTURELOADER_HPP
#include "GraphicsVulkan/Mat/private/matTextureLoader.hpp"
#endif
#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif
#ifndef GVK_MATMIPFILTER_HPP
#include "GraphicsVulkan/Mat/matMipFilter.hpp"
#endif
#ifndef GVK_MATMIPTEXTURE_HPP
#include "GraphicsVulkan/Mat/matMipTexture.hpp"
#endif
#ifndef GVK_MATPLAINTEXTURE_HPP
#include "GraphicsVulkan/Mat/matPlainTexture.hpp"
#endif
#ifndef GVK_G2DTEXTUREVK_HPP
#include "GraphicsVulkan/G2d/g2dTextureVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
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
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/fs/fsResourceTracker.hpp"
#endif
#include <string>
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_TEXTURETRACKING_HPP
#include "Graphics/Mat/matTextureTracking.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/It/itStringUtil.hpp"
#endif

namespace gvk
{

//============================================================================
//============================================================================
namespace
{

enum FileType
{
	e_DDS,
	e_TGA,
	e_BMP,
	e_JPG,
	e_PNG,
	e_TIFF,
	e_GIF,
	e_WMP
};

//------------------------------------------------------------------------
//	matTextureLoaderWin's pick_format: by the file name's extension
//------------------------------------------------------------------------
FileType pick_format(const fsLocator& i_FileName)
{
	//	decide what kind of file we have
	std::string fname = i_FileName.GetLastName();
	std::string ext;
	itStringUtil::GetExtension(fname, ext);

	if ((ext == "bmp") || (ext == "BMP"))
	{
		return e_BMP;
	}
	else if ((ext == "jpg") || (ext == "JPG") || (ext == "JPEG") || (ext == "jpeg"))
	{
		return e_JPG;
	}
	else if ((ext == "tga") || (ext == "TGA"))
	{
		return e_TGA;
	}
	else if ((ext == "png") || (ext == "PNG"))
	{
		return e_PNG;
	}
	else if ((ext == "dds") || (ext == "DDS"))
	{
		return e_DDS;
	}
	else if ((ext == "tif") || (ext == "TIF") || (ext == "Tif"))
	{
		return e_TIFF;
	}
	else if ((ext == "tiff") || (ext == "TIFF") || (ext == "Tiff"))
	{
		return e_TIFF;
	}
	throw g2dUnknownImageFileTypeX();
}

matImageFileType decode_type(FileType i_Type)
{
	switch (i_Type)
	{
		case e_DDS:		return matImageFileType::e_DDS;
		case e_TGA:		return matImageFileType::e_TGA;
		case e_BMP:		return matImageFileType::e_BMP;
		case e_JPG:		return matImageFileType::e_JPG;
		case e_PNG:		return matImageFileType::e_PNG;
		case e_TIFF:	return matImageFileType::e_TIFF;
		case e_GIF:		return matImageFileType::e_GIF;
		case e_WMP:		return matImageFileType::e_WMP;
	}
	return matImageFileType::e_Unknown;
}

std::string file_name(const fsLocator& i_Locator)
{
	std::string name;
	fsFileUtil::LocatorToANSIFilename(i_Locator, name);
	return name;
}

//----------------------------------------------------------------------------
//	DirectX::Resize to the width/height reduction, then GenerateMipMaps with
//	i_Levels (0 = all; 1 keeps the reduced image), then CreateTexture
//----------------------------------------------------------------------------
bool make_texture(const matDecodedImage& i_Decoded, int i_WidthReduce, int i_HeightReduce, uint32_t i_Levels,
				  matMipFilter i_Filter, const fsLocator& i_Locator, std::unique_ptr<vkdImage>& o_Image, uint32_t& o_Format)
{
	// resize to width/height reduce
	matDecodedImage reduced;
	if (!matReduce(i_Decoded, i_WidthReduce, i_HeightReduce, reduced))
	{
		DBG_WARNING("Can't resize texture (format " << i_Decoded.m_DxgiFormat << "): " << file_name(i_Locator));
		return false;
	}

	// generate mipmaps
	matDecodedImage chain;
	if (i_Levels == 1)
	{
		chain = std::move(reduced);
	}
	else if (!matGenerateMips(reduced, i_Levels, chain, i_Filter))
	{
		DBG_WARNING("Can't make mips for texture (format " << reduced.m_DxgiFormat << "): " << file_name(i_Locator));
		return false;
	}

	// fill in the image
	const std::string name = file_name(i_Locator);
	o_Image = matTextureLoader::CreateImage(chain, name.c_str());
	o_Format = chain.m_DxgiFormat;
	return o_Image != NULL;
}

//----------------------------------------------------------------------------
//	do_load_texture: the file decoded as its extension says
//----------------------------------------------------------------------------
bool do_load_texture(std::unique_ptr<vkdImage>& o_Image, uint32_t& o_Format, const fsLocator& i_Locator,
					 int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	FileType file_type = pick_format(i_Locator);
	matDecodedImage decoded;
	std::string error;
	if (!matDecodeImage(i_Locator, decode_type(file_type), decoded, &error))
	{
		DBG_WARNING("Can't decode texture " << file_name(i_Locator) << ": " << error);
		return false;
	}
	return make_texture(decoded, i_WidthReduce, i_HeightReduce, i_bIsMipMap ? 0 : 1, matMipFilter::e_Triangle,
						i_Locator, o_Image, o_Format);
}

//----------------------------------------------------------------------------
//	The image into the texture, or a failed load reported as
//	matTextureLoaderWin reports it
//----------------------------------------------------------------------------
template <class TEXTURE>
void finish_load(TEXTURE* io_Texture, const fsLocator& i_Locator, bool i_bLoaded, std::unique_ptr<vkdImage>& io_Image,
				 uint32_t i_Format)
{
	if ( !i_bLoaded )
	{
		fsResourceTracker::Remove(i_Locator);

		std::string filename;
		fsFileUtil::LocatorToUnicodeString(i_Locator, filename);
		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("Error in loading texture: " << filename);
			matTextureTracking::AddMissingTexture(i_Locator);
			return;
		}
		else
		{
			DBG_ERROR("Error in loading texture: " << filename);
			throw fsUnknownX(i_Locator);
		}
	}

	// clear out current io_Texture, and replace with the image
	io_Texture->SetSurface(std::move(io_Image), i_Format);
	io_Texture->ReloadInfo();
	fsResourceTracker::MarkEnd(i_Locator);
}

void load_mip_texture(matMipTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::unique_ptr<vkdImage> image;
	uint32_t format = 0;
	const bool loaded = do_load_texture(image, format, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
	finish_load(io_Texture, i_Locator, loaded, image, format);
}

void load_plain_texture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::unique_ptr<vkdImage> image;
	uint32_t format = 0;
	const bool loaded = do_load_texture(image, format, i_Locator, i_WidthReduce, i_HeightReduce, false);
	finish_load(io_Texture, i_Locator, loaded, image, format);
}

//----------------------------------------------------------------------------
//	The TGA and HDR loaders: decoded as that type, mips with the box filter
//	(DirectXTex's TEX_FILTER_FANT)
//----------------------------------------------------------------------------
template <class TEXTURE>
void load_typed(TEXTURE* io_Texture, const fsLocator& i_Locator, matImageFileType i_Type, int i_WidthReduce, int i_HeightReduce,
				uint32_t i_Levels)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::unique_ptr<vkdImage> image;
	uint32_t format = 0;
	matDecodedImage decoded;
	std::string error;
	bool loaded = matDecodeImage(i_Locator, i_Type, decoded, &error);
	if (!loaded)
	{
		DBG_WARNING("Can't decode texture " << file_name(i_Locator) << ": " << error);
	}
	else
	{
		loaded = make_texture(decoded, i_WidthReduce, i_HeightReduce, i_Levels, matMipFilter::e_Box, i_Locator, image, format);
	}
	finish_load(io_Texture, i_Locator, loaded, image, format);
}

}	// end of namespace

namespace matTextureLoader
{

//------------------------------------------------------------------------
//	Every subresource, layer-major then mip, as DirectX::CreateTexture
//------------------------------------------------------------------------
std::unique_ptr<vkdImage> CreateImage(const matDecodedImage& i_Image, const char* i_Name)
{
	const char* substitution = NULL;
	vkdImageDesc desc;
	desc.m_Type = i_Image.m_Depth > 1 ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
	desc.m_Format = vkdFormatFromDxgi(i_Image.m_DxgiFormat, &substitution);
	desc.m_Width = i_Image.m_Width;
	desc.m_Height = i_Image.m_Height;
	desc.m_Depth = i_Image.m_Depth;
	desc.m_Mips = i_Image.m_Mips;
	desc.m_Layers = i_Image.m_Layers;
	desc.m_bCube = i_Image.m_bCube;
	desc.m_Usage = c_TextureUsage;
	if (desc.m_Format == VK_FORMAT_UNDEFINED)
	{
		DBG_WARNING("Texture format " << i_Image.m_DxgiFormat << " has no Vulkan format: " << i_Name);
		return NULL;
	}
	if (desc.m_Width == 0 || desc.m_Height == 0 || i_Image.m_Subs.size() < (size_t)desc.m_Layers * desc.m_Mips)
	{
		DBG_WARNING("Empty texture image: " << i_Name);
		return NULL;
	}

	std::unique_ptr<vkdImage> image = vkdImage::Create(vkdGlobal::Device(), desc, i_Name);
	for (uint32_t layer = 0; layer < desc.m_Layers; layer++)
	{
		for (uint32_t mip = 0; mip < desc.m_Mips; mip++)
		{
			const matDecodedImage::Sub& sub = i_Image.m_Subs[layer * desc.m_Mips + mip];
			image->Upload(vkdGlobal::Context(), mip, layer, &i_Image.m_Bytes[sub.m_Offset], sub.m_RowPitch, sub.m_SlicePitch);
		}
	}
	return image;
}

//------------------------------------------------------------------------
// The decoders handle images of many file types
//------------------------------------------------------------------------
void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator)
{
	load_plain_texture(io_Texture, i_Locator, 0, 0);
}

void LoadPNG(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

//------------------------------------------------------------------------
// NOTE! As in DX11 this loads whatever the extension says (JPEG files
// come here)
//------------------------------------------------------------------------
void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

void LoadBMP(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

void LoadTIFF(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

void LoadTIFF(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	load_plain_texture(io_Texture, i_Locator, i_WidthReduce, i_HeightReduce);
}

//------------------------------------------------------------------------
//	DX11 loads the resource with FindResource (Windows); only the
//	DEMO_VERSION watermark uses one. The texture stays empty, as on DX11
//	for a name that isn't a resource.
//------------------------------------------------------------------------
void LoadResource(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
{
	GVK_NOT_YET("matTextureLoader::LoadResource (Windows resources)");
	DBG_LOG("loading resource " << i_Locator << " (" << file_name(i_Locator) << "): not on Vulkan");
}

}	// namespace matTextureLoader

namespace matTextureLoaderTGA
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
	{
		load_typed(io_Texture, i_Locator, matImageFileType::e_TGA, i_WidthReduce, i_HeightReduce, 1);
	}

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
	{
		load_typed(io_Texture, i_Locator, matImageFileType::e_TGA, i_WidthReduce, i_HeightReduce, i_bIsMipMap ? 0 : 1);
	}
}

namespace matTextureLoaderHDR
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
	{
		load_typed(io_Texture, i_Locator, matImageFileType::e_HDR, i_WidthReduce, i_HeightReduce, 1);
	}

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadHDR(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
	{
		load_typed(io_Texture, i_Locator, matImageFileType::e_HDR, i_WidthReduce, i_HeightReduce, i_bIsMipMap ? 0 : 1);
	}
}

}
