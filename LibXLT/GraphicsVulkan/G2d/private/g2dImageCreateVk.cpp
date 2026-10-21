/*****************************************************************************
**  g2dImageCreateVk.cpp
**
**      See g2dImageCreateVk.hpp. GraphicsDX11's g2dImageCreateDX11.cpp,
**	with the parts of g2dSurfaceLoaderDX11.cpp it uses (LoadPNG, LoadBMP,
**	LoadBMPData, CopyImageData). DX11 reads PNG files with libpng; here
**	matImageDecode (stb_image) decodes them and the result is turned into
**	the rows libpng gives DX11's loader: 8-bit RGBA if the PNG has an
**	alpha channel, else 8-bit RGB (16-bit channels keep their high byte,
**	as png_set_strip_16 does).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DIMAGECREATEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageCreateVk.hpp"
#endif
#ifndef GVK_G2DIMAGEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/Fs/fsResourceTracker.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/Gf/gfFileBin.hpp"
#endif
#ifndef GF_FILETRANSLATIONMGR_HPP
#include "Core/Gf/gfFileTranslationMgr.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif

#include <cstdint>
#include <memory>
#include <vector>

namespace gvk
{
namespace
{
	enum FileType
	{
		e_BMP,
		e_PNG
	};

	FileType pick_format(const fsLocator& i_Locator)
	{
		//	decide what kind of file we have
		//
		gfFileBin test_file_stream(i_Locator, fsFileStream::e_ReadOnly);

		//	The BMP header begins with 0x42 0x4d 'BM'
		unsigned char header[8];
		test_file_stream.Read(2, header);
		if( (header[0] == 0x42) && (header[1] == 0x4d) )
			return e_BMP;
		else
		{
			test_file_stream.Read(6, header+2);

			if( (header[0] == 137)	&&
				(header[1] == 80)	&&
				(header[2] == 78)	&&
				(header[3] == 71)	&&
				(header[4] == 13)	&&
				(header[5] == 10)	&&
				(header[6] == 26)	&&
				(header[7] == 10)	)
			{
				return e_PNG;
			}
			else
				throw g2dUnknownImageFileTypeX();
		}
	}

	//	The Windows bitmap headers DX11's LoadBMPData reads (wingdi.h's
	//	BITMAPFILEHEADER and BITMAPINFOHEADER, which GraphicsVulkan's
	//	portable code doesn't include)
#pragma pack(push, 2)
	struct BitmapFileHeader
	{
		uint16_t	bfType;
		uint32_t	bfSize;
		uint16_t	bfReserved1;
		uint16_t	bfReserved2;
		uint32_t	bfOffBits;
	};
#pragma pack(pop)
	struct BitmapInfoHeader
	{
		uint32_t	biSize;
		int32_t		biWidth;
		int32_t		biHeight;
		uint16_t	biPlanes;
		uint16_t	biBitCount;
		uint32_t	biCompression;
		uint32_t	biSizeImage;
		int32_t		biXPelsPerMeter;
		int32_t		biYPelsPerMeter;
		uint32_t	biClrUsed;
		uint32_t	biClrImportant;
	};
	static_assert(sizeof(BitmapFileHeader) == 14, "BITMAPFILEHEADER is 14 bytes");
	static_assert(sizeof(BitmapInfoHeader) == 40, "BITMAPINFOHEADER is 40 bytes");
	const uint32_t c_BitmapCoreHeaderSize = 12;		// sizeof(BITMAPCOREHEADER)
	const uint32_t c_BI_RGB = 0;

	//	The rows, freed when it goes
	struct Rows
	{
		std::vector<uint8_t*> m_Rows;
		~Rows()
		{
			for (uint8_t* row : m_Rows)
				delete [] row;
		}
	};

	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  The format
	//	for this function's row data is always RGB.
	//------------------------------------------------------------------------
	void LoadBMPData(	const fsLocator& i_Locator,
						std::vector<uint8_t*>& o_Rows,
						int& o_Width, bool& o_HasAlpha)
	{
		gfFileBin read_stream(i_Locator, fsFileStream::e_ReadOnly);

		BitmapFileHeader file_header;
		read_stream.Read(sizeof(file_header), &file_header);

		if( file_header.bfType != 0x4d42 )	// 'BM'
			throw g2dUnknownImageFileTypeX();

		BitmapInfoHeader info_header;
		read_stream.Read(sizeof(info_header), &info_header);

		// don't deal with CORE headers, whatever they are
		if( info_header.biSize == c_BitmapCoreHeaderSize )
			throw g2dUnknownImageFileTypeX();

		//	let's try loading only 24 bit bmps
		if ( ( info_header.biBitCount != 24 ) &&
			 ( info_header.biBitCount != 32 ))
			throw g2dUnknownImageFileTypeX();

		//	we don't want to deal with compression
		if( info_header.biCompression != c_BI_RGB )
			throw g2dUnknownImageFileTypeX();

		o_HasAlpha = (info_header.biBitCount == 32);

		o_Width = info_header.biWidth;
		int height = info_header.biHeight;

		bool top_down = false;
		if( height < 0 )
		{
			top_down = true;
			height = -height;
		}

		fsFileStream::FilePosType bitmap_data_pos = file_header.bfOffBits;
		read_stream.SetFilePos(bitmap_data_pos);

		int pixelsize = info_header.biBitCount / 8;

		//	24bpp
		int i;
		o_Rows.resize(height);
		for( i = 0 ; i < height ; ++i )
			o_Rows[i] = new uint8_t[o_Width * pixelsize];

		int file_y;
		if( top_down )
		{
			for( file_y = 0 ; file_y < height ; ++file_y )
				read_stream.Read(o_Width * pixelsize, o_Rows[file_y]);
		}
		else
		{
			for( file_y = height - 1 ; file_y >= 0 ; --file_y )
				read_stream.Read(o_Width * pixelsize, o_Rows[file_y]);
		}

		//	we must reverse the R and B values to have the proper output format
		for( file_y = 0 ; file_y < height ; ++file_y )
		{
			int x_pixel;
			uint8_t* row_base = o_Rows[file_y];

			for( x_pixel = 0 ; x_pixel < o_Width ; ++x_pixel )
			{
				int byte_number = x_pixel * pixelsize;
				uint8_t temp = row_base[byte_number];
				row_base[byte_number] = row_base[byte_number + 2];
				row_base[byte_number + 2] = temp;
			}
		}
	}

	//------------------------------------------------------------------------
	//	DX11's load_png_data (libpng) as rows: RGBA if the PNG's colour type
	//	has an alpha channel, else RGB; 8 bits a channel
	//------------------------------------------------------------------------
	void LoadPNGData(	const fsLocator& i_Locator,
						std::vector<uint8_t*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha)
	{
		//	the colour type, from the IHDR chunk (byte 25 of the file)
		uint8_t header[26];
		{
			gfFileBin read_stream(i_Locator, fsFileStream::e_ReadOnly);
			if (read_stream.Read(sizeof(header), header) != (int)sizeof(header))
				throw g2dUnknownImageFileTypeX();
		}
		const uint8_t color_type = header[25];
		//	does it have an alpha channel? (PNG_COLOR_MASK_ALPHA)
		o_HasAlpha = (color_type & 4) != 0;

		matDecodedImage decoded;
		itString error;
		if (!matDecodeImage(i_Locator, matImageFileType::e_PNG, decoded, &error))
		{
			DBG_ERROR("Couldn't read PNG file: " << error);
			throw g2dUnknownImageFileTypeX();
		}

		//	matImageDecode gives grey PNGs one channel and the others four,
		//	of 8 or 16 bits
		uint32_t channels = 4, bytes = 1;
		switch (decoded.m_DxgiFormat)
		{
			case g2dVkGlobal::e_DXGI_R8G8B8A8_UNORM:
			case g2dVkGlobal::e_DXGI_R8G8B8A8_UNORM_SRGB:	channels = 4; bytes = 1; break;
			case g2dVkGlobal::e_DXGI_R16G16B16A16_UNORM:	channels = 4; bytes = 2; break;
			case 61 /* DXGI_FORMAT_R8_UNORM */:				channels = 1; bytes = 1; break;
			case 56 /* DXGI_FORMAT_R16_UNORM */:			channels = 1; bytes = 2; break;
			default:
				DBG_ERROR("Unexpected PNG pixel format " << decoded.m_DxgiFormat);
				throw g2dUnknownImageFileTypeX();
		}

		const int out_channels = o_HasAlpha ? 4 : 3;
		const int width = (int)decoded.m_Width;
		const int height = (int)decoded.m_Height;
		const size_t row_pitch = decoded.m_Subs[0].m_RowPitch;
		o_Width = width;
		o_Rows.resize(height);
		for (int y = 0; y < height; y++)
		{
			const uint8_t* src = decoded.m_Bytes.data() + decoded.m_Subs[0].m_Offset + (size_t)y * row_pitch;
			uint8_t* row = new uint8_t[(size_t)width * out_channels];
			o_Rows[y] = row;
			for (int x = 0; x < width; x++)
			{
				for (int c = 0; c < out_channels; c++)
				{
					//	grey: the one channel for red, green and blue (alpha 255)
					const uint32_t src_channel = channels == 1 ? 0 : (uint32_t)c;
					uint8_t value;
					if (channels == 1 && c == 3)
						value = 255;
					else if (bytes == 2)
						value = (uint8_t)(reinterpret_cast<const uint16_t*>(src)[(size_t)x * channels + src_channel] >> 8);
					else
						value = src[(size_t)x * channels + src_channel];
					row[(size_t)x * out_channels + c] = value;
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//	This function expects the source image data to be in the same format
	//	as that returned by the LoadPNGData and LoadBMPData functions
	//	(RGB bytes or RGBA bytes).
	//------------------------------------------------------------------------
	void CopyImageData(	void* i_DestBits,
						int i_DestStride,
						int i_DestWidth,
						const g2dPFD& i_DestPFD,
						const std::vector<uint8_t*>& i_SourceRows,
						bool i_SourceHasAlpha)
	{
		//	reformat
		//
		int cur_width, cur_height;
		int height = (int)i_SourceRows.size();

		uint8_t* cur_image_line = reinterpret_cast<uint8_t*>(i_DestBits);

		if ( i_SourceHasAlpha )
		{
			// alpha version
			//
			for( cur_height = 0 ; cur_height < height ; cur_height++ )
			{
				uint8_t*	cur_src_pixel = i_SourceRows[cur_height];
				uint8_t* cur_image_pixel = cur_image_line;

				for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
				{
					uint8_t red = *cur_src_pixel++;
					uint8_t green = *cur_src_pixel++;
					uint8_t blue = *cur_src_pixel++;
					uint8_t alpha = *cur_src_pixel++;
					uint32_t pixel = i_DestPFD.MakePixel(red, green, blue, alpha);

					if( i_DestPFD.BitsPerPixel() == 16 )
					{
						uint16_t* word_pixel = reinterpret_cast<uint16_t*>(cur_image_pixel);
						*word_pixel = uint16_t(pixel);
						cur_image_pixel += 2;
					}
					else if( i_DestPFD.BitsPerPixel() == 24 )
					{
						DBG_ASSERT(false, "Not Implemented");
					}
					else if( i_DestPFD.BitsPerPixel() == 32 )
					{
						uint32_t* dword_pixel = reinterpret_cast<uint32_t*>(cur_image_pixel);
						*dword_pixel = pixel;
						cur_image_pixel += 4;
					}
				}

				cur_image_line += i_DestStride;
			}
		}
		else
		{
			// non-alpha version
			for( cur_height = 0 ; cur_height < height ; cur_height++ )
			{
				uint8_t*	cur_src_pixel = i_SourceRows[cur_height];
				uint8_t* cur_image_pixel = cur_image_line;

				for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
				{
					uint8_t red = *cur_src_pixel++;
					uint8_t green = *cur_src_pixel++;
					uint8_t blue = *cur_src_pixel++;
					uint32_t pixel = i_DestPFD.MakePixel(red, green, blue);

					if( i_DestPFD.BitsPerPixel() == 16 )
					{
						uint16_t* word_pixel = reinterpret_cast<uint16_t*>(cur_image_pixel);
						*word_pixel = uint16_t(pixel);
						cur_image_pixel += 2;
					}
					else if( i_DestPFD.BitsPerPixel() == 24 )
					{
						DBG_ASSERT(false, "Not Implemented");
					}
					else if( i_DestPFD.BitsPerPixel() == 32 )
					{
						uint32_t* dword_pixel = reinterpret_cast<uint32_t*>(cur_image_pixel);
						*dword_pixel = pixel;
						cur_image_pixel += 4;
					}
				}

				cur_image_line += i_DestStride;
			}
		}
	}

	//	g2dDX11Global::g_ScreenPixelFormat: the R8G8B8A8 screen format's
	g2dPFD screen_pixel_format()
	{
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(g2dVkGlobal::e_DXGI_R8G8B8A8_UNORM, pfd);
		return pfd;
	}

	//	g2dSurfaceLoader::LoadPNG and LoadBMP with the screen pixel format
	void load(g2dImageVk& io_Image, const fsLocator& i_Locator, FileType i_Type, g2dImage::CreateLoc i_CreateLoc)
	{
		fsResourceTracker::MarkBegin(i_Locator);

		Rows rows;
		int width;
		bool alpha;
		if (i_Type == e_PNG)
			LoadPNGData(i_Locator, rows.m_Rows, width, alpha);
		else
			LoadBMPData(i_Locator, rows.m_Rows, width, alpha);

		const g2dPFD image_pfd = screen_pixel_format();
		io_Image.Make( width, (int)rows.m_Rows.size(), image_pfd, i_CreateLoc );

		void* image_data = io_Image.Lock();
		int stride = io_Image.GetStride();

		CopyImageData(	image_data,
						stride,
						width,
						image_pfd,
						rows.m_Rows,
						alpha);

		io_Image.Release();

		fsResourceTracker::MarkEnd(i_Locator);
	}
}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dGeneralX.
	//--------------------------------------------------------------------
	g2dImage* g2dImageCreateVk::Make( int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc )
	{
		return new g2dImageVk(i_Width, i_Height, i_PFD, i_CreateLoc );
	}

	//------------------------------------------------------------------------
	//	This Load will attempt to use whatever platform-specific facilities
	//	are available to load the given file from disk.  If the file does
	//	not seem to represent a useable image type, it will throw a
	//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
	//	will throw a fsFileDoesntExistX (see the fs package).
	//------------------------------------------------------------------------
	g2dImage* g2dImageCreateVk::Load( const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc )
	{
		if( !gfFileTranslationMgr::FileExists(i_Locator) )
			throw fsFileDoesntExistX(i_Locator);

		FileType file_type = pick_format(i_Locator);

		std::unique_ptr<g2dImageVk> image(new g2dImageVk);
		load(*image, i_Locator, file_type, i_CreateLoc);
		image->SetLocator(i_Locator);
		return image.release();
	}
}
