/*****************************************************************************
**  matImageDecodeHdr.cpp
**
**      Radiance RGBE (.hdr) files as GraphicsDX11 reads them
**	(matTextureLoaderHDRWin, load_hdr_data): Graphics' RGBE reader
**	(Graphics/G2d/private/rgbe.cpp, after Bruce Walter's), copied here to
**	read from memory with every read checked against the end of the data,
**	then the floats as halves in R16G16B16A16_FLOAT with alpha 1, converted
**	as DirectXMath's XMConvertFloatToHalf does.
**
**	One difference from the copied reader: the "#?" magic token is
**	required (rgbe.cpp leaves that check commented out), so a file that
**	isn't Radiance at all doesn't decode as one.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif

#include <cmath>
#include <cstdio>
#include <vector>

namespace gvk
{
	namespace matDecode
	{
		namespace
		{
			//	The bytes still to read, as rgbe.cpp's FILE*
			struct Reader
			{
				const uint8_t*	m_pAt;
				const uint8_t*	m_pEnd;

				//	fgets with a 128-byte buffer: up to 127 characters, through
				//	the newline; false at the end of the data
				bool GetLine(char o_Line[128])
				{
					if (m_pAt >= m_pEnd)
					{
						return false;
					}
					size_t n = 0;
					while (n < 127 && m_pAt < m_pEnd)
					{
						const char c = (char)*m_pAt++;
						o_Line[n++] = c;
						if (c == '\n')
						{
							break;
						}
					}
					o_Line[n] = 0;
					return true;
				}

				bool Read(void* o_pData, size_t i_Size)
				{
					if ((size_t)(m_pEnd - m_pAt) < i_Size)
					{
						return false;
					}
					memcpy(o_pData, m_pAt, i_Size);
					m_pAt += i_Size;
					return true;
				}
			};

			//	rgbe.cpp's rgbe2float
			void rgbe2float(float* o_pRgb, const uint8_t i_Rgbe[4])
			{
				if (i_Rgbe[3])
				{
					const float f = (float)ldexp(1.0, i_Rgbe[3] - (int)(128 + 8));
					o_pRgb[0] = i_Rgbe[0] * f;
					o_pRgb[1] = i_Rgbe[1] * f;
					o_pRgb[2] = i_Rgbe[2] * f;
				}
				else
				{
					o_pRgb[0] = o_pRgb[1] = o_pRgb[2] = 0.0f;
				}
			}

			//	RGBE_ReadHeader
			bool read_header(Reader& io_In, int& o_Width, int& o_Height, std::string& o_Error)
			{
				char line[128];
				if (!io_In.GetLine(line))
				{
					o_Error = "HDR header is truncated";
					return false;
				}
				if (line[0] != '#' || line[1] != '?')
				{
					o_Error = "not a Radiance HDR file (no #? magic)";
					return false;
				}
				if (!io_In.GetLine(line))
				{
					o_Error = "HDR header is truncated";
					return false;
				}
				for (;;)
				{
					if (line[0] == 0)
					{
						o_Error = "HDR header has no FORMAT specifier";
						return false;
					}
					else if (strcmp(line, "\n") == 0)
					{
						break;
					}
					if (!io_In.GetLine(line))
					{
						o_Error = "HDR header is truncated";
						return false;
					}
				}
				if (!io_In.GetLine(line))
				{
					o_Error = "HDR header is truncated";
					return false;
				}
				if (sscanf(line, "-Y %d +X %d", &o_Height, &o_Width) < 2)
				{
					o_Error = "HDR header has no image size";
					return false;
				}
				return true;
			}

			//	RGBE_ReadPixels
			bool read_flat(Reader& io_In, float* o_pData, size_t i_Pixels)
			{
				uint8_t rgbe[4];
				for (size_t i = 0; i < i_Pixels; i++, o_pData += 3)
				{
					if (!io_In.Read(rgbe, 4))
					{
						return false;
					}
					rgbe2float(o_pData, rgbe);
				}
				return true;
			}

			//	RGBE_ReadPixels_RLE
			bool read_rle(Reader& io_In, float* o_pData, int i_Width, int i_Lines, std::string& o_Error)
			{
				if (i_Width < 8 || i_Width > 0x7fff)
				{
					//	run length encoding is not allowed, so read flat
					if (!read_flat(io_In, o_pData, (size_t)i_Width * i_Lines))
					{
						o_Error = "HDR pixel data is truncated";
						return false;
					}
					return true;
				}
				std::vector<uint8_t> scanline((size_t)4 * i_Width);
				for (int line = 0; line < i_Lines; line++)
				{
					uint8_t rgbe[4];
					if (!io_In.Read(rgbe, 4))
					{
						o_Error = "HDR pixel data is truncated";
						return false;
					}
					if (rgbe[0] != 2 || rgbe[1] != 2 || (rgbe[2] & 0x80))
					{
						//	this file is not run length encoded
						rgbe2float(o_pData, rgbe);
						if (!read_flat(io_In, o_pData + 3, (size_t)i_Width * (i_Lines - line) - 1))
						{
							o_Error = "HDR pixel data is truncated";
							return false;
						}
						return true;
					}
					if ((((int)rgbe[2]) << 8 | rgbe[3]) != i_Width)
					{
						o_Error = "HDR scanline has the wrong width";
						return false;
					}
					uint8_t* ptr = scanline.data();
					for (int i = 0; i < 4; i++)
					{
						uint8_t* ptr_end = scanline.data() + (size_t)(i + 1) * i_Width;
						while (ptr < ptr_end)
						{
							uint8_t buf[2];
							if (!io_In.Read(buf, 2))
							{
								o_Error = "HDR pixel data is truncated";
								return false;
							}
							int count;
							if (buf[0] > 128)
							{
								//	a run of the same value
								count = buf[0] - 128;
								if (count == 0 || count > ptr_end - ptr)
								{
									o_Error = "HDR scanline data is bad";
									return false;
								}
								while (count-- > 0)
								{
									*ptr++ = buf[1];
								}
							}
							else
							{
								//	a non-run
								count = buf[0];
								if (count == 0 || count > ptr_end - ptr)
								{
									o_Error = "HDR scanline data is bad";
									return false;
								}
								*ptr++ = buf[1];
								if (--count > 0)
								{
									if (!io_In.Read(ptr, (size_t)count))
									{
										o_Error = "HDR pixel data is truncated";
										return false;
									}
									ptr += count;
								}
							}
						}
					}
					for (int i = 0; i < i_Width; i++, o_pData += 3)
					{
						const uint8_t pixel[4] = { scanline[i], scanline[i + i_Width], scanline[i + 2 * i_Width], scanline[i + 3 * i_Width] };
						rgbe2float(o_pData, pixel);
					}
				}
				return true;
			}
		}

		bool DecodeHdr(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error)
		{
			Reader in = { i_pData, i_pData + i_Size };
			int width = 0, height = 0;
			if (!read_header(in, width, height, o_Error))
			{
				return false;
			}
			//	The least data the size needs: 4 bytes a pixel flat; RLE lines
			//	take a 4-byte header and, per channel, 2 bytes per 127 values
			const uint64_t lineMin = (width >= 8 && width <= 0x7fff) ? 4 + 8 * (((uint64_t)width + 126) / 127) : 4 * (uint64_t)width;
			if (width <= 0 || height <= 0 || width > 16384 || height > 16384 ||
				lineMin * (uint64_t)height > (uint64_t)(in.m_pEnd - in.m_pAt))
			{
				o_Error = "HDR image size is not plausible for the file";
				return false;
			}
			std::vector<float> rgb((size_t)3 * width * height);
			if (!read_rle(in, rgb.data(), width, height, o_Error))
			{
				return false;
			}
			uint16_t* out = reinterpret_cast<uint16_t*>(Make2D(o_Image, e_R16G16B16A16_FLOAT, (uint32_t)width, (uint32_t)height, 8));
			const uint16_t one = FloatToHalf(1.0f);
			for (size_t i = 0; i < (size_t)width * height; i++)
			{
				out[i * 4 + 0] = FloatToHalf(rgb[i * 3 + 0]);
				out[i * 4 + 1] = FloatToHalf(rgb[i * 3 + 1]);
				out[i * 4 + 2] = FloatToHalf(rgb[i * 3 + 2]);
				out[i * 4 + 3] = one;
			}
			return true;
		}
	}
}
