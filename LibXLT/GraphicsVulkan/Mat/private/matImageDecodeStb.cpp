/*****************************************************************************
**  matImageDecodeStb.cpp
**
**      PNG, JPEG, BMP and GIF through stb_image (this file holds its
**	implementation). stb_image decodes the pixels; the code here reads
**	what WIC (through DirectXTex's LoadFromWICFile) uses to choose the
**	format, and so what a shader samples:
**
**	- PNG: grey stays one channel (R8, or R16 for 16-bit); grey+alpha,
**	  RGB, RGBA and palettes become RGBA (16-bit: R16G16B16A16); an sRGB
**	  chunk, or a gAMA chunk of 1/2.2 without one, makes 8-bit RGBA _SRGB
**	- JPEG: grey is R8, colour RGBA8; EXIF ColorSpace 1 makes it _SRGB
**	- BMP: RGBA8; alpha only where the header has an alpha mask (WIC reads
**	  other 32-bit BMPs as BGRX). Uncompressed 24-bit and 32-bit BMPs are
**	  read here, from their bit masks (stb_image misplaces the pixels of
**	  the BITMAPV5HEADER files WIC writes); the rest go to stb_image
**	- GIF: the first frame, RGBA8
**
**	stb_image reads zeros past the end of a truncated BMP, GIF or JPEG
**	rather than failing, so their structure is checked here first.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif

// Keep this format-restricted implementation separate from ImportExport's copy.
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#define STBI_NO_STDIO
#include <stb_image.h>

#include <climits>
#include <memory>

namespace gvk
{
	namespace matDecode
	{
		namespace
		{
			struct StbFree
			{
				void operator()(void* i_p) const	{ stbi_image_free(i_p); }
			};

			//----------------------------------------------------------------
			//	PNG: IHDR, and the colour space chunks before the image data
			//----------------------------------------------------------------
			struct PngInfo
			{
				uint32_t	m_Width = 0, m_Height = 0;
				int			m_Depth = 0, m_ColorType = -1;
				bool		m_bSrgbChunk = false;
				bool		m_bGama = false;
				uint32_t	m_Gama = 0;
			};

			bool read_png_info(const uint8_t* i_pData, size_t i_Size, PngInfo& o_Info, std::string& o_Error)
			{
				static const uint8_t c_Signature[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
				if (i_Size < 8 || memcmp(i_pData, c_Signature, 8) != 0)
				{
					o_Error = "not a PNG file";
					return false;
				}
				size_t at = 8;
				bool header = false;
				while (at + 8 <= i_Size)
				{
					const uint32_t length = Read32BE(i_pData + at);
					const uint8_t* type = i_pData + at + 4;
					if (length > i_Size - at - 8)
					{
						o_Error = "PNG chunk runs past the end of the file";
						return false;
					}
					const uint8_t* body = i_pData + at + 8;
					if (!memcmp(type, "IHDR", 4) && length >= 13)
					{
						o_Info.m_Width = Read32BE(body);
						o_Info.m_Height = Read32BE(body + 4);
						o_Info.m_Depth = body[8];
						o_Info.m_ColorType = body[9];
						header = true;
					}
					else if (!memcmp(type, "sRGB", 4) && length >= 1)
					{
						o_Info.m_bSrgbChunk = true;
					}
					else if (!memcmp(type, "gAMA", 4) && length >= 4)
					{
						o_Info.m_bGama = true;
						o_Info.m_Gama = Read32BE(body);
					}
					else if (!memcmp(type, "IDAT", 4) || !memcmp(type, "IEND", 4))
					{
						break;		// colour space chunks come before the image data
					}
					at += 12 + (size_t)length;
				}
				if (!header)
				{
					o_Error = "PNG file has no IHDR chunk";
					return false;
				}
				return true;
			}

			//----------------------------------------------------------------
			//	JPEG: the marker structure (an SOS, then an EOI), and EXIF's
			//	ColorSpace tag (40961), which WIC reads as System.Image.ColorSpace
			//----------------------------------------------------------------
			uint16_t tiff16(const uint8_t* i_p, bool i_bBig)	{ return i_bBig ? Read16BE(i_p) : Read16(i_p); }
			uint32_t tiff32(const uint8_t* i_p, bool i_bBig)	{ return i_bBig ? Read32BE(i_p) : Read32(i_p); }

			//	Finds i_Tag in the IFD at i_Ifd of the TIFF structure i_pTiff[0, i_Size);
			//	returns a pointer to its 12-byte entry, or NULL
			const uint8_t* find_ifd_entry(const uint8_t* i_pTiff, size_t i_Size, uint32_t i_Ifd, bool i_bBig, uint16_t i_Tag)
			{
				if (i_Ifd > i_Size || i_Size - i_Ifd < 2)
				{
					return nullptr;
				}
				const uint32_t count = tiff16(i_pTiff + i_Ifd, i_bBig);
				for (uint32_t i = 0; i < count; i++)
				{
					const size_t entry = (size_t)i_Ifd + 2 + (size_t)i * 12;
					if (entry + 12 > i_Size)
					{
						return nullptr;
					}
					if (tiff16(i_pTiff + entry, i_bBig) == i_Tag)
					{
						return i_pTiff + entry;
					}
				}
				return nullptr;
			}

			//	ColorSpace from an APP1 Exif segment's body; 0 if absent
			uint32_t exif_color_space(const uint8_t* i_pBody, size_t i_Size)
			{
				if (i_Size < 14 || memcmp(i_pBody, "Exif\0\0", 6) != 0)
				{
					return 0;
				}
				const uint8_t* tiff = i_pBody + 6;
				const size_t size = i_Size - 6;
				bool big;
				if (tiff[0] == 'I' && tiff[1] == 'I')
				{
					big = false;
				}
				else if (tiff[0] == 'M' && tiff[1] == 'M')
				{
					big = true;
				}
				else
				{
					return 0;
				}
				const uint8_t* exif = find_ifd_entry(tiff, size, tiff32(tiff + 4, big), big, 0x8769);
				if (!exif)
				{
					return 0;
				}
				const uint8_t* space = find_ifd_entry(tiff, size, tiff32(exif + 8, big), big, 0xA001);
				return space ? tiff16(space + 8, big) : 0;
			}

			bool check_jpeg(const uint8_t* i_pData, size_t i_Size, bool& o_bSrgb, std::string& o_Error)
			{
				o_bSrgb = false;
				if (i_Size < 4 || i_pData[0] != 0xFF || i_pData[1] != 0xD8)
				{
					o_Error = "not a JPEG file";
					return false;
				}
				size_t at = 2;
				while (at + 4 <= i_Size)
				{
					if (i_pData[at] != 0xFF)
					{
						o_Error = "JPEG marker expected";
						return false;
					}
					const uint8_t marker = i_pData[at + 1];
					if (marker == 0xFF)
					{
						at++;			// fill byte
						continue;
					}
					const size_t length = Read16BE(i_pData + at + 2);
					if (length < 2 || at + 2 + length > i_Size)
					{
						o_Error = "JPEG segment runs past the end of the file";
						return false;
					}
					if (marker == 0xE1)
					{
						o_bSrgb = o_bSrgb || exif_color_space(i_pData + at + 4, length - 2) == 1;
					}
					if (marker == 0xDA)
					{
						//	Entropy-coded data follows; it never holds FF D9 (FF is
						//	stuffed with 00), so an EOI after it means the scan is whole
						for (size_t i = at + 2 + length; i + 1 < i_Size; i++)
						{
							if (i_pData[i] == 0xFF && i_pData[i + 1] == 0xD9)
							{
								return true;
							}
						}
						o_Error = "JPEG file is truncated (no end of image marker)";
						return false;
					}
					at += 2 + length;
				}
				o_Error = "JPEG file has no image data";
				return false;
			}

			//----------------------------------------------------------------
			//	BMP: the pixel data must all be there; is there an alpha mask?
			//----------------------------------------------------------------
			bool check_bmp(const uint8_t* i_pData, size_t i_Size, bool& o_bAlpha, std::string& o_Error)
			{
				o_bAlpha = false;
				if (i_Size < 26 || i_pData[0] != 'B' || i_pData[1] != 'M')
				{
					o_Error = "not a BMP file";
					return false;
				}
				const uint32_t offset = Read32(i_pData + 10);
				const uint32_t header = Read32(i_pData + 14);
				int64_t width, height;
				uint32_t bpp, compression = 0;
				if (header == 12)
				{
					width = Read16(i_pData + 18);
					height = Read16(i_pData + 20);
					bpp = Read16(i_pData + 24);
				}
				else
				{
					if (i_Size < 14 + 40 || header < 40)
					{
						o_Error = "BMP header is truncated";
						return false;
					}
					width = (int32_t)Read32(i_pData + 18);
					height = (int32_t)Read32(i_pData + 22);
					bpp = Read16(i_pData + 28);
					compression = Read32(i_pData + 30);
					if (bpp == 32)
					{
						if (header >= 56 && i_Size >= 14 + 56)
						{
							o_bAlpha = Read32(i_pData + 14 + 52) != 0;
						}
						else if (compression == 6 && i_Size >= 14 + 40 + 16)		// BI_ALPHABITFIELDS
						{
							o_bAlpha = Read32(i_pData + 14 + 40 + 12) != 0;
						}
					}
				}
				if (height < 0)
				{
					height = -height;
				}
				if (width <= 0 || height <= 0 || bpp == 0)
				{
					o_Error = "BMP has no pixels";
					return false;
				}
				if (compression == 0 || compression == 3 || compression == 6)
				{
					const uint64_t row = ((uint64_t)width * bpp + 31) / 32 * 4;
					const uint64_t last = ((uint64_t)width * bpp + 7) / 8;
					if ((uint64_t)offset + row * (uint64_t)(height - 1) + last > i_Size)
					{
						o_Error = "BMP pixel data is truncated";
						return false;
					}
				}
				return true;
			}

			//	Uncompressed 24-bit and 32-bit BMPs, read here (stb_image
			//	misplaces the pixels of a BITMAPV5HEADER file with masks after
			//	the header, as WIC writes them): RGBA8, alpha from the alpha
			//	mask, else 1. Returns false with o_bHandled false for the
			//	formats left to stb_image.
			uint32_t mask_value(uint32_t i_Pixel, uint32_t i_Mask)
			{
				if (!i_Mask)
				{
					return 0;
				}
				uint32_t shift = 0;
				while (!((i_Mask >> shift) & 1))
				{
					shift++;
				}
				const uint32_t max = i_Mask >> shift;
				const uint32_t v = (i_Pixel & i_Mask) >> shift;
				return max == 255 ? v : (uint32_t)((uint64_t)v * 255 * 2 + max) / (2 * max);
			}

			bool decode_bmp_direct(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, bool& o_bHandled)
			{
				o_bHandled = false;
				const uint32_t header = Read32(i_pData + 14);
				if (header < 40 || i_Size < 14 + 40)
				{
					return false;
				}
				const int32_t width = (int32_t)Read32(i_pData + 18), height = (int32_t)Read32(i_pData + 22);
				const uint32_t bpp = Read16(i_pData + 28), compression = Read32(i_pData + 30);
				if ((bpp != 24 && bpp != 32) || (compression != 0 && compression != 3 && compression != 6) ||
					(bpp == 24 && compression != 0))
				{
					return false;
				}
				o_bHandled = true;
				uint32_t r = 0x00ff0000, g = 0x0000ff00, b = 0x000000ff, a = 0;
				if (compression != 0)
				{
					//	in the header from BITMAPV2INFOHEADER on, else after it
					const size_t at = 14 + 40;
					if (i_Size < at + 12)
					{
						return false;
					}
					r = Read32(i_pData + at);
					g = Read32(i_pData + at + 4);
					b = Read32(i_pData + at + 8);
					if ((header >= 56 || compression == 6) && i_Size >= at + 16)
					{
						a = Read32(i_pData + at + 12);
					}
				}
				else if (header >= 56)
				{
					a = Read32(i_pData + 14 + 52);
				}
				const uint32_t w = (uint32_t)width, h = (uint32_t)(height < 0 ? -(int64_t)height : height);
				const uint32_t offset = Read32(i_pData + 10);
				const size_t row = ((size_t)w * bpp + 31) / 32 * 4;
				uint8_t* out = Make2D(o_Image, e_R8G8B8A8_UNORM, w, h, 4);
				for (uint32_t y = 0; y < h; y++)
				{
					//	bottom-up unless the height is negative
					const uint8_t* src = i_pData + offset + (size_t)(height < 0 ? y : h - 1 - y) * row;
					uint8_t* dst = out + (size_t)y * w * 4;
					for (uint32_t x = 0; x < w; x++, dst += 4)
					{
						const uint8_t* p = src + (size_t)x * (bpp / 8);
						const uint32_t px = bpp == 32 ? Read32(p) : (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
						dst[0] = (uint8_t)mask_value(px, r);
						dst[1] = (uint8_t)mask_value(px, g);
						dst[2] = (uint8_t)mask_value(px, b);
						dst[3] = a ? (uint8_t)mask_value(px, a) : 255;
					}
				}
				return true;
			}

			//----------------------------------------------------------------
			//	GIF: the block structure up to the end of the first image
			//----------------------------------------------------------------
			bool skip_sub_blocks(const uint8_t* i_pData, size_t i_Size, size_t& io_At)
			{
				for (;;)
				{
					if (io_At >= i_Size)
					{
						return false;
					}
					const size_t length = i_pData[io_At++];
					if (length == 0)
					{
						return true;
					}
					if (length > i_Size - io_At)
					{
						return false;
					}
					io_At += length;
				}
			}

			bool check_gif(const uint8_t* i_pData, size_t i_Size, std::string& o_Error)
			{
				if (i_Size < 13 || (memcmp(i_pData, "GIF87a", 6) != 0 && memcmp(i_pData, "GIF89a", 6) != 0))
				{
					o_Error = "not a GIF file";
					return false;
				}
				size_t at = 13;
				if (i_pData[10] & 0x80)
				{
					at += 3 * ((size_t)2 << (i_pData[10] & 7));
				}
				while (at < i_Size)
				{
					const uint8_t block = i_pData[at++];
					if (block == 0x21)
					{
						if (at >= i_Size)
						{
							break;
						}
						at++;		// label
						if (!skip_sub_blocks(i_pData, i_Size, at))
						{
							break;
						}
					}
					else if (block == 0x2C)
					{
						if (at + 9 > i_Size)
						{
							break;
						}
						const uint8_t flags = i_pData[at + 8];
						at += 9;
						if (flags & 0x80)
						{
							at += 3 * ((size_t)2 << (flags & 7));
						}
						at++;		// LZW minimum code size
						if (at > i_Size || !skip_sub_blocks(i_pData, i_Size, at))
						{
							break;
						}
						return true;
					}
					else
					{
						o_Error = block == 0x3B ? "GIF file has no image" : "GIF block expected";
						return false;
					}
				}
				o_Error = "GIF file is truncated";
				return false;
			}

			//	stb_image's 8-bit pixels with i_Channels channels into a new
			//	2D image of i_Dxgi with i_OutChannels channels per pixel
			bool load8(const uint8_t* i_pData, size_t i_Size, int i_Channels, uint32_t i_Dxgi, matDecodedImage& o_Image,
					   std::string& o_Error)
			{
				int w = 0, h = 0, n = 0;
				std::unique_ptr<stbi_uc, StbFree> pixels(stbi_load_from_memory(i_pData, (int)i_Size, &w, &h, &n, i_Channels));
				if (!pixels)
				{
					o_Error = std::string("stb_image: ") + stbi_failure_reason();
					return false;
				}
				uint8_t* out = Make2D(o_Image, i_Dxgi, (uint32_t)w, (uint32_t)h, (size_t)i_Channels);
				memcpy(out, pixels.get(), (size_t)w * h * i_Channels);
				return true;
			}

			bool load16(const uint8_t* i_pData, size_t i_Size, int i_Channels, uint32_t i_Dxgi, matDecodedImage& o_Image,
						std::string& o_Error)
			{
				int w = 0, h = 0, n = 0;
				std::unique_ptr<stbi_us, StbFree> pixels(stbi_load_16_from_memory(i_pData, (int)i_Size, &w, &h, &n, i_Channels));
				if (!pixels)
				{
					o_Error = std::string("stb_image: ") + stbi_failure_reason();
					return false;
				}
				uint8_t* out = Make2D(o_Image, i_Dxgi, (uint32_t)w, (uint32_t)h, (size_t)i_Channels * 2);
				memcpy(out, pixels.get(), (size_t)w * h * i_Channels * 2);
				return true;
			}

			bool decode_png(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error)
			{
				PngInfo info;
				if (!read_png_info(i_pData, i_Size, info, o_Error))
				{
					return false;
				}
				//	DirectXTex: an sRGB chunk, else gAMA of 45455 (1/2.2), makes it sRGB
				const bool srgb = info.m_bSrgbChunk || (info.m_bGama && info.m_Gama == 45455);
				const bool wide = info.m_Depth == 16;
				bool ok;
				if (info.m_ColorType == 0)
				{
					ok = wide ? load16(i_pData, i_Size, 1, e_R16_UNORM, o_Image, o_Error)
							  : load8(i_pData, i_Size, 1, e_R8_UNORM, o_Image, o_Error);
				}
				else
				{
					ok = wide ? load16(i_pData, i_Size, 4, e_R16G16B16A16_UNORM, o_Image, o_Error)
							  : load8(i_pData, i_Size, 4, srgb ? e_R8G8B8A8_UNORM_SRGB : e_R8G8B8A8_UNORM, o_Image, o_Error);
				}
				return ok;
			}
		}

		bool DecodeStb(const uint8_t* i_pData, size_t i_Size, matImageFileType i_Type, matDecodedImage& o_Image, std::string& o_Error)
		{
			if (i_Size > (size_t)INT_MAX)
			{
				o_Error = "the file is too large";
				return false;
			}
			switch (i_Type)
			{
				case matImageFileType::e_PNG:
					return decode_png(i_pData, i_Size, o_Image, o_Error);

				case matImageFileType::e_JPG:
				{
					bool srgb;
					if (!check_jpeg(i_pData, i_Size, srgb, o_Error))
					{
						return false;
					}
					int w, h, n;
					if (!stbi_info_from_memory(i_pData, (int)i_Size, &w, &h, &n))
					{
						o_Error = std::string("stb_image: ") + stbi_failure_reason();
						return false;
					}
					return n == 1 ? load8(i_pData, i_Size, 1, e_R8_UNORM, o_Image, o_Error)
								  : load8(i_pData, i_Size, 4, srgb ? e_R8G8B8A8_UNORM_SRGB : e_R8G8B8A8_UNORM, o_Image, o_Error);
				}

				case matImageFileType::e_BMP:
				{
					bool alpha, handled;
					if (!check_bmp(i_pData, i_Size, alpha, o_Error))
					{
						return false;
					}
					const bool direct = decode_bmp_direct(i_pData, i_Size, o_Image, handled);
					if (handled)
					{
						if (!direct)
						{
							o_Error = "BMP bit masks are truncated";
						}
						return direct;
					}
					if (!load8(i_pData, i_Size, 4, e_R8G8B8A8_UNORM, o_Image, o_Error))
					{
						return false;
					}
					if (!alpha)
					{
						for (size_t i = 3; i < o_Image.m_Bytes.size(); i += 4)
						{
							o_Image.m_Bytes[i] = 255;
						}
					}
					return true;
				}

				case matImageFileType::e_GIF:
					return check_gif(i_pData, i_Size, o_Error) && load8(i_pData, i_Size, 4, e_R8G8B8A8_UNORM, o_Image, o_Error);

				default:
					o_Error = "not an stb_image file type";
					return false;
			}
		}
	}
}
