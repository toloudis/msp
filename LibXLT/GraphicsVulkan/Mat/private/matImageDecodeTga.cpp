/*****************************************************************************
**  matImageDecodeTga.cpp
**
**      TGA files as DirectXTex's LoadFromTGAFile reads them (a port of
**	DirectXTexTGA.cpp, MIT licence, Copyright (c) Microsoft Corporation;
**	the September 2024 release GraphicsDX11 links):
**
**	- 8-bit grey (plain or RLE) -> R8
**	- 16-bit -> B5G5R5A1
**	- 24-bit and 32-bit (plain or RLE), and 8-bit colour-mapped with a
**	  24-bit map -> R8G8B8A8 (24-bit: alpha 1)
**	- an alpha channel that is 0 everywhere is made opaque
**	- a TGA 2.0 extension area with gamma 2.2 or 2.4 makes it _SRGB
**	- RLE packets may not cross the end of a row; any read past the end
**	  of the data fails the load
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif

#include <algorithm>
#include <cmath>

namespace gvk
{
	namespace matDecode
	{
		namespace
		{
			enum ImageType : uint8_t
			{
				e_NoImage = 0, e_ColorMapped = 1, e_TrueColor = 2, e_BlackAndWhite = 3,
				e_ColorMappedRle = 9, e_TrueColorRle = 10, e_BlackAndWhiteRle = 11,
			};

			const uint8_t c_InvertX = 0x10, c_InvertY = 0x20, c_Interleaved = 0x40 | 0x80;
			const size_t c_HeaderLength = 18, c_FooterLength = 26, c_ExtensionLength = 495;
			const char c_Signature[] = "TRUEVISION-XFILE.";		// 18 bytes with the terminating 0

			struct Header
			{
				uint8_t		m_IdLength, m_ColorMapType, m_ImageType;
				uint16_t	m_ColorMapFirst, m_ColorMapLength;
				uint8_t		m_ColorMapSize;
				uint16_t	m_Width, m_Height;
				uint8_t		m_Bits, m_Descriptor;
			};

			Header read_header(const uint8_t* i_p)
			{
				Header h;
				h.m_IdLength = i_p[0];
				h.m_ColorMapType = i_p[1];
				h.m_ImageType = i_p[2];
				h.m_ColorMapFirst = Read16(i_p + 3);
				h.m_ColorMapLength = Read16(i_p + 5);
				h.m_ColorMapSize = i_p[7];
				h.m_Width = Read16(i_p + 12);
				h.m_Height = Read16(i_p + 14);
				h.m_Bits = i_p[16];
				h.m_Descriptor = i_p[17];
				return h;
			}

			//	Output pixels: one of R8 (1 byte), B5G5R5A1 (2), R8G8B8A8 (4)
			struct Writer
			{
				uint8_t*	m_pPixels;
				size_t		m_PixelBytes;
				uint32_t	m_Width, m_Height;
				bool		m_bInvertX, m_bInvertY;

				//	Where pixel x of the i_Row-th row in the file goes
				uint8_t* At(uint32_t i_Row, uint32_t i_X) const
				{
					const uint32_t y = m_bInvertY ? i_Row : m_Height - i_Row - 1;
					const uint32_t x = m_bInvertX ? m_Width - 1 - i_X : i_X;
					return m_pPixels + ((size_t)y * m_Width + x) * m_PixelBytes;
				}
			};

			//	One file pixel (i_Bits) to the output format
			void put_pixel(uint8_t* o_pOut, const uint8_t* i_pIn, uint8_t i_Bits)
			{
				switch (i_Bits)
				{
					case 8:		o_pOut[0] = i_pIn[0]; break;
					case 16:	o_pOut[0] = i_pIn[0]; o_pOut[1] = i_pIn[1]; break;
					case 24:	o_pOut[0] = i_pIn[2]; o_pOut[1] = i_pIn[1]; o_pOut[2] = i_pIn[0]; o_pOut[3] = 255; break;
					default:	o_pOut[0] = i_pIn[2]; o_pOut[1] = i_pIn[1]; o_pOut[2] = i_pIn[0]; o_pOut[3] = i_pIn[3]; break;
				}
			}

			uint32_t alpha_of(const uint8_t* i_pIn, uint8_t i_Bits)
			{
				switch (i_Bits)
				{
					case 16:	return (Read16(i_pIn) & 0x8000) ? 255 : 0;
					case 32:	return i_pIn[3];
					default:	return 255;
				}
			}
		}

		bool DecodeTga(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error)
		{
			if (i_Size < c_HeaderLength)
			{
				o_Error = "TGA header is truncated";
				return false;
			}
			const Header h = read_header(i_pData);
			if (h.m_Descriptor & c_Interleaved)
			{
				o_Error = "interleaved TGA files are not supported";
				return false;
			}
			if (!h.m_Width || !h.m_Height)
			{
				o_Error = "TGA has no pixels";
				return false;
			}
			bool rle = false, mapped = false;
			uint32_t dxgi = 0;
			size_t bytes = 0;
			switch (h.m_ImageType)
			{
				case e_ColorMapped:
					if (h.m_ColorMapType != 1 || h.m_ColorMapLength == 0 || h.m_Bits != 8 || h.m_ColorMapSize != 24)
					{
						o_Error = "only 8-bit TGA colour maps of 24-bit colours are supported";
						return false;
					}
					mapped = true;
					dxgi = e_R8G8B8A8_UNORM;
					bytes = 4;
					break;

				case e_TrueColor:
				case e_TrueColorRle:
					if (h.m_ColorMapType != 0 || h.m_ColorMapLength != 0)
					{
						o_Error = "TGA true colour image with a colour map";
						return false;
					}
					switch (h.m_Bits)
					{
						case 16:	dxgi = e_B5G5R5A1_UNORM; bytes = 2; break;
						case 24:
						case 32:	dxgi = e_R8G8B8A8_UNORM; bytes = 4; break;
						default:
							o_Error = "unsupported TGA bit depth";
							return false;
					}
					rle = h.m_ImageType == e_TrueColorRle;
					break;

				case e_BlackAndWhite:
				case e_BlackAndWhiteRle:
					if (h.m_ColorMapType != 0 || h.m_ColorMapLength != 0 || h.m_Bits != 8)
					{
						o_Error = "unsupported TGA greyscale image";
						return false;
					}
					dxgi = e_R8_UNORM;
					bytes = 1;
					rle = h.m_ImageType == e_BlackAndWhiteRle;
					break;

				case e_NoImage:
				case e_ColorMappedRle:
					o_Error = "unsupported TGA image type";
					return false;

				default:
					o_Error = "not a TGA file";
					return false;
			}

			size_t offset = c_HeaderLength + h.m_IdLength;
			if (offset > i_Size)
			{
				o_Error = "TGA file is truncated";
				return false;
			}
			uint8_t palette[256 * 4] = {};
			if (mapped)
			{
				const size_t last = (size_t)h.m_ColorMapFirst + h.m_ColorMapLength;
				const size_t mapBytes = (size_t)h.m_ColorMapLength * 3;
				if (h.m_ColorMapLength > 256 || last > 256)
				{
					o_Error = "TGA colour map is too large";
					return false;
				}
				if (offset == i_Size || mapBytes > i_Size - offset)
				{
					o_Error = "TGA colour map is truncated";
					return false;
				}
				const uint8_t* map = i_pData + offset;
				for (size_t i = h.m_ColorMapFirst; i < last; i++, map += 3)
				{
					palette[i * 4 + 0] = map[2];
					palette[i * 4 + 1] = map[1];
					palette[i * 4 + 2] = map[0];
					palette[i * 4 + 3] = 255;
				}
				offset += mapBytes;
			}
			if (offset >= i_Size)
			{
				o_Error = "TGA file has no pixel data";
				return false;
			}

			Writer out;
			out.m_pPixels = Make2D(o_Image, dxgi, h.m_Width, h.m_Height, bytes);
			out.m_PixelBytes = bytes;
			out.m_Width = h.m_Width;
			out.m_Height = h.m_Height;
			out.m_bInvertX = (h.m_Descriptor & c_InvertX) != 0;
			out.m_bInvertY = (h.m_Descriptor & c_InvertY) != 0;

			const uint8_t* src = i_pData + offset;
			const uint8_t* end = i_pData + i_Size;
			const size_t inBytes = mapped ? 1 : (size_t)(h.m_Bits / 8);
			uint32_t maxAlpha = 0;
			for (uint32_t row = 0; row < h.m_Height; row++)
			{
				for (uint32_t x = 0; x < h.m_Width; )
				{
					uint32_t count = 1;
					bool repeat = false;
					if (rle)
					{
						if (src >= end)
						{
							o_Error = "TGA pixel data is truncated";
							return false;
						}
						repeat = (*src & 0x80) != 0;
						count = (uint32_t)(*src & 0x7F) + 1;
						src++;
					}
					const size_t need = repeat ? inBytes : inBytes * count;
					if ((size_t)(end - src) < need)
					{
						o_Error = "TGA pixel data is truncated";
						return false;
					}
					for (uint32_t j = 0; j < count; j++, x++)
					{
						if (x >= h.m_Width)
						{
							o_Error = "TGA run crosses the end of a row";
							return false;
						}
						const uint8_t* in = repeat ? src : src + j * inBytes;
						uint8_t* dst = out.At(row, x);
						if (mapped)
						{
							memcpy(dst, &palette[in[0] * 4], 4);
						}
						else
						{
							put_pixel(dst, in, h.m_Bits);
							maxAlpha = std::max(maxAlpha, alpha_of(in, h.m_Bits));
						}
					}
					src += need;
				}
			}

			//	An alpha channel that is 0 everywhere is taken as no alpha
			if (!mapped && maxAlpha == 0 && (h.m_Bits == 16 || h.m_Bits == 32))
			{
				for (size_t i = 0; i < o_Image.m_Bytes.size(); i += bytes)
				{
					if (bytes == 2)
					{
						o_Image.m_Bytes[i + 1] |= 0x80;
					}
					else
					{
						o_Image.m_Bytes[i + 3] = 255;
					}
				}
			}

			//	The TGA 2.0 extension area's gamma
			if (i_Size >= c_FooterLength)
			{
				const uint8_t* footer = i_pData + i_Size - c_FooterLength;
				if (memcmp(footer + 8, c_Signature, sizeof(c_Signature)) == 0)
				{
					const uint32_t ext = Read32(footer);
					if (ext != 0 && (uint64_t)ext + c_ExtensionLength <= i_Size && Read16(i_pData + ext) == c_ExtensionLength)
					{
						const uint16_t numerator = Read16(i_pData + ext + 478), denominator = Read16(i_pData + ext + 480);
						if (denominator != 0)
						{
							const float gamma = (float)numerator / (float)denominator;
							if (std::fabs(gamma - 2.2f) < 0.01f || std::fabs(gamma - 2.4f) < 0.01f)
							{
								o_Image.m_DxgiFormat = MakeSrgb(o_Image.m_DxgiFormat);
							}
						}
					}
				}
			}
			return true;
		}
	}
}
