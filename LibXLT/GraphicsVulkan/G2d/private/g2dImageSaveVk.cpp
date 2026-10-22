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
#ifndef GVK_G2DIMAGEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageVk.hpp"
#endif
#ifndef GVK_MATIMAGEENCODE_HPP
#include "GraphicsVulkan/Mat/private/matImageEncode.hpp"
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

	//------------------------------------------------------------------------
	//	DX11's get_d3d_file_format: the formats DX11 saves through WIC; the
	//	rest throw (DDS is written only by texture saving)
	//------------------------------------------------------------------------
	enum Codec
	{
		e_CodecBMP,
		e_CodecJPEG,
		e_CodecPNG,
		e_CodecTIFF
	};

	Codec get_codec( FileType i_Type )
	{
		switch( i_Type )
		{
			case e_BMP:
				return e_CodecBMP;
			case e_JPG:
				return e_CodecJPEG;
			case e_PNG:
				return e_CodecPNG;
			case e_TIFF:
				return e_CodecTIFF;
			case e_DDS:
			case e_TGA:
			case e_PPM:
			case e_DIB:
			case e_HDR:
			case e_PFM:
				throw g2dUnknownImageFileTypeX();
		}

		throw g2dUnknownImageFileTypeX();
	}

	//------------------------------------------------------------------------
	//	DX11's SaveToFile (DirectXTex's SaveToWICFile), through matImageEncode
	//------------------------------------------------------------------------
	void SaveToFile(const fsLocator& i_FileName, const g2dImageVk& i_Image, Codec i_Codec)
	{
		const matEncodeImage image = { i_Image.GetWidth(), i_Image.GetHeight(), i_Image.GetStride(),
									   i_Image.GetDxgiFormat(), i_Image.Pixels() };
		std::vector<uint8_t> bytes;
		switch (i_Codec)
		{
			case e_CodecBMP:	bytes = matEncodeBMP(image); break;
			case e_CodecJPEG:	bytes = matEncodeJPG(image); break;
			case e_CodecPNG:	bytes = matEncodePNG(image); break;
			case e_CodecTIFF:	bytes = matEncodeTIFF(image); break;
		}
		matEncodeWriteFile(i_FileName, bytes);
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
			//	DX11's capture_back_buffer
			g2dImage* image = NULL;
			g2dScreenCaptureUtil::CaptureWindowToImage(*i_pWin, image);
			std::unique_ptr<g2dImage> owned(image);
			const g2dImageVk* pImageVk = dynamic_cast<const g2dImageVk*>(image);
			DBG_ASSERT(pImageVk != 0, "Not a valid image pointer");

			FileType type = pick_format( i_FileName );

			SaveToFile( i_FileName, *pImageVk, get_codec( type ) );
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
		} else {
			SaveToFile( i_FileName, *pImageVk, get_codec( type ) );
		}
	}
}