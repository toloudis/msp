/****************************************************************************\
**  g2dImageSaveVk.cpp
**
**	See g2dImageSaveVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DIMAGESAVEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageSaveVk.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef G2D_HDRUTIL_HPP
#include "Graphics/G2d/g2dHDRUtil.hpp"
#endif
#ifndef CPTR_SCANLINEEXRUTIL_HPP
#include "Graphics/G2d/g2dScanlineOpenEXRUtil.hpp"
#endif
#ifndef G2D_TGAUTIL_HPP
#include "Graphics/G2d/g2dTGAUtil.hpp"
#endif
#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif

//	static: ImportExport (glTF) links its own stb_image_write into MachStudio
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace gvk
{
namespace
{
	enum FileType {
		e_BMP,
		e_JPG,
		e_TGA,
		e_PNG,
		e_DDS,
		e_PPM,
		e_DIB,
		e_HDR,
		e_PFM,
		e_EXR,
		e_TIFF
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	FileType pick_format(const fsLocator& i_FileName)
	{
		//	decide what kind of file we have
		itString fname = i_FileName.GetLastName();
		itString ext;
		fname.GetExtension(ext);

		if ((ext == itString("bmp")) || (ext == itString("BMP")))
		{
			return e_BMP;
		}
		else if ((ext == itString("jpg")) || (ext == itString("JPG")) || (ext == itString("JPEG")) || (ext == itString("jpeg")))
		{
			return e_JPG;
		}
		else if ((ext == itString("tga")) || (ext == itString("TGA")))
		{
			return e_TGA;
		}
		else if ((ext == itString("png")) || (ext == itString("PNG")))
		{
			return e_PNG;
		}
		else if ((ext == itString("dds")) || (ext == itString("DDS")))
		{
			return e_DDS;
		}
		else if ((ext == itString("ppm")) || (ext == itString("PPM")))
		{
			return e_PPM;
		}
		else if ((ext == itString("dib")) || (ext == itString("DIB")))
		{
			return e_DIB;
		}
		else if ((ext == itString("hdr")) || (ext == itString("HDR")))
		{
			return e_HDR;
		}
		else if ((ext == itString("pfm")) || (ext == itString("PFM")))
		{
			return e_PFM;
		}
		else if ((ext == itString("exr")) || (ext == itString("EXR")))
		{
			return e_EXR;
		}
		else if ((ext == itString("tif")) || (ext == itString("TIF")) || (ext == itString("Tif")))
		{
			return e_TIFF;
		}
		else if ((ext == itString("tiff")) || (ext == itString("TIFF")) || (ext == itString("Tiff")))
		{
			return e_TIFF;
		}
		throw g2dUnknownImageFileTypeX();
	}

	void append_bytes(void* i_pContext, void* i_pData, int i_Size)
	{
		std::vector<uint8_t>& bytes = *static_cast<std::vector<uint8_t>*>(i_pContext);
		const uint8_t* data = static_cast<const uint8_t*>(i_pData);
		bytes.insert(bytes.end(), data, data + i_Size);
	}

	//------------------------------------------------------------------------
	//	DX11's SaveToFile (DirectXTex's SaveToWICFile) for PNG: an 8-bit
	//	RGBA image as a 32-bit RGBA PNG
	//------------------------------------------------------------------------
	void save_png(const fsLocator& i_FileName, const g2dImage* i_pImage)
	{
		const g2dPFD& pfd = i_pImage->GetPixelFormat();
		g2dImage* image = const_cast<g2dImage*>(i_pImage);
		if (pfd.GetNumChannels() != 4 || pfd.GetBitsPerChannel() != 8 || pfd.IsFloat() || pfd.FirstRedBit() != 0)
		{
			GVK_NOT_YET("g2dImageSaveVk::Save (PNG of an image that isn't 8-bit RGBA)");
			throw g2dImageSaveX();
		}
		std::vector<uint8_t> bytes;
		const void* pixels = image->Lock();
		const int written = pixels ? stbi_write_png_to_func(append_bytes, &bytes, image->GetWidth(), image->GetHeight(), 4,
															 pixels, image->GetStride()) : 0;
		image->Release();
		if (!written)
		{
			DBG_TRACE("stbi_write_png failed");
			throw g2dImageSaveX();
		}

		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_FileName, filename);
		std::ofstream file(std::filesystem::path(filename.GetString()), std::ios::binary | std::ios::trunc);
		file.write(reinterpret_cast<const char*>(bytes.data()), (std::streamsize)bytes.size());
		if (!file)
		{
			DBG_TRACE("Couldn't write " << filename);
			throw g2dImageSaveX();
		}
	}
}

	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename
	//	passed in.
	//
	//	If the file format is not known the function will throw
	//	g2dUnknownImageFileTypeX.
	//------------------------------------------------------------------------
	void g2dImageSaveVk::Save(const fsLocator& i_FileName, g2dWindow* i_pWin)
	{
		if (i_pWin)
		{
			g2dImage* image = NULL;
			g2dScreenCaptureUtil::CaptureWindowToImage(*i_pWin, image);
			std::unique_ptr<g2dImage> owned(image);
			if (image)
			{
				Save(i_FileName, image);
			}
		}
	}

	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename
	//	passed in.
	//
	//	If the file format is not known the function will throw
	//	g2dUnknownImageFileTypeX.
	//------------------------------------------------------------------------
	void g2dImageSaveVk::Save(const fsLocator& i_FileName, const g2dImage* i_pImage)
	{
		const g2dImageVk* pImageVk = dynamic_cast<const g2dImageVk*>(i_pImage);
		DBG_ASSERT(pImageVk != 0, "Not a valid image pointer");

		FileType type = pick_format( i_FileName );

		if ( type == e_EXR ) {
			g2dScanlineOpenEXRUtil::WriteToScanlineEXR(i_FileName, i_pImage);
		}else if (type == e_HDR)
		{
			g2dHDRUtil::WriteToHDR(i_FileName, i_pImage);
		}else if (type == e_TGA)
		{
			g2dTGAUtil::WriteToTGA(i_FileName, i_pImage);
		}else if (type == e_PNG)
		{
			save_png(i_FileName, i_pImage);
		} else {
			//	DX11 saves these through WIC (BMP, JPG, TIFF, DDS; PPM, DIB
			//	and PFM throw g2dUnknownImageFileTypeX there too)
			GVK_NOT_YET("g2dImageSaveVk::Save (formats DX11 saves through WIC, other than PNG)");
			throw g2dImageSaveX();
		}
	}
}
