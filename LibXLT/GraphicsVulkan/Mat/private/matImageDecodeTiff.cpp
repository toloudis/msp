/*****************************************************************************
**  matImageDecodeTiff.cpp
**
**      TIFF files through libtiff, read from memory, in the formats WIC's
**	TIFF decoder gives DirectXTex:
**
**	- grey: R8, R16, or R32_FLOAT (32-bit float samples)
**	- grey + alpha: R8G8B8A8 / R16G16B16A16 with the grey in red, green, blue
**	- RGB, RGBA: R8G8B8A8, R16G16B16A16_UNORM, R32G32B32A32_FLOAT (alpha 1
**	  where the file has none)
**	- anything else (palettes, YCbCr, 1-bit, tiles, planar...): libtiff's
**	  RGBA reader, R8G8B8A8
**	- EXIF ColorSpace 1 makes 8-bit RGBA _SRGB
**
**	libtiff's errors and warnings go to the decode error, not to stderr.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif

#include <tiffio.h>

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <vector>

namespace gvk
{
	namespace matDecode
	{
		namespace
		{
			struct Memory
			{
				const uint8_t*	m_pData;
				uint64_t		m_Size;
				uint64_t		m_At;
				std::string*	m_pError;
			};

			tmsize_t read_proc(thandle_t i_Handle, void* o_pData, tmsize_t i_Size)
			{
				Memory* m = static_cast<Memory*>(i_Handle);
				if (i_Size < 0 || m->m_At > m->m_Size)
				{
					return 0;
				}
				const uint64_t n = std::min<uint64_t>((uint64_t)i_Size, m->m_Size - m->m_At);
				memcpy(o_pData, m->m_pData + m->m_At, (size_t)n);
				m->m_At += n;
				return (tmsize_t)n;
			}

			tmsize_t write_proc(thandle_t, void*, tmsize_t)
			{
				return 0;
			}

			toff_t seek_proc(thandle_t i_Handle, toff_t i_Offset, int i_Whence)
			{
				Memory* m = static_cast<Memory*>(i_Handle);
				uint64_t base = 0;
				switch (i_Whence)
				{
					case SEEK_SET:	base = 0; break;
					case SEEK_CUR:	base = m->m_At; break;
					case SEEK_END:	base = m->m_Size; break;
					default:		return (toff_t)-1;
				}
				m->m_At = base + i_Offset;
				return m->m_At;
			}

			int close_proc(thandle_t)
			{
				return 0;
			}

			toff_t size_proc(thandle_t i_Handle)
			{
				return static_cast<Memory*>(i_Handle)->m_Size;
			}

			int map_proc(thandle_t, void**, toff_t*)
			{
				return 0;
			}

			void unmap_proc(thandle_t, void*, toff_t)
			{
			}

			int error_handler(TIFF*, void* i_pUser, const char* i_pModule, const char* i_pFormat, va_list i_Args)
			{
				std::string* error = static_cast<std::string*>(i_pUser);
				if (error->empty())
				{
					char text[512];
					vsnprintf(text, sizeof(text), i_pFormat, i_Args);
					*error = std::string("libtiff: ") + (i_pModule ? std::string(i_pModule) + ": " : std::string()) + text;
				}
				return 1;
			}

			int warning_handler(TIFF*, void*, const char*, const char*, va_list)
			{
				return 1;		// warnings don't fail the decode
			}

			struct Closer
			{
				void operator()(TIFF* i_pTiff) const	{ TIFFClose(i_pTiff); }
			};

			//	EXIF ColorSpace (40961) == 1; reads the EXIF directory, so call it last
			bool exif_srgb(TIFF* i_pTiff)
			{
				toff_t exif = 0;
				if (!TIFFGetField(i_pTiff, TIFFTAG_EXIFIFD, &exif) || !TIFFReadEXIFDirectory(i_pTiff, exif))
				{
					return false;
				}
				uint16_t space = 0;
				return TIFFGetField(i_pTiff, EXIFTAG_COLORSPACE, &space) && space == 1;
			}
		}

		bool DecodeTiff(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error)
		{
			std::string error;
			Memory memory = { i_pData, i_Size, 0, &error };
			TIFFOpenOptions* options = TIFFOpenOptionsAlloc();
			TIFFOpenOptionsSetErrorHandlerExtR(options, error_handler, &error);
			TIFFOpenOptionsSetWarningHandlerExtR(options, warning_handler, nullptr);
			TIFFOpenOptionsSetMaxSingleMemAlloc(options, (tmsize_t)1 << 31);
			std::unique_ptr<TIFF, Closer> tiff(TIFFClientOpenExt("memory", "rm", &memory, read_proc, write_proc, seek_proc,
																 close_proc, size_proc, map_proc, unmap_proc, options));
			TIFFOpenOptionsFree(options);
			if (!tiff)
			{
				o_Error = error.empty() ? "not a TIFF file" : error;
				return false;
			}
			TIFF* t = tiff.get();
			uint32_t width = 0, height = 0;
			uint16_t samples = 1, bits = 1, format = SAMPLEFORMAT_UINT, photometric = PHOTOMETRIC_MINISBLACK,
					 planar = PLANARCONFIG_CONTIG;
			TIFFGetField(t, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(t, TIFFTAG_IMAGELENGTH, &height);
			TIFFGetFieldDefaulted(t, TIFFTAG_SAMPLESPERPIXEL, &samples);
			TIFFGetFieldDefaulted(t, TIFFTAG_BITSPERSAMPLE, &bits);
			TIFFGetFieldDefaulted(t, TIFFTAG_SAMPLEFORMAT, &format);
			TIFFGetFieldDefaulted(t, TIFFTAG_PLANARCONFIG, &planar);
			TIFFGetField(t, TIFFTAG_PHOTOMETRIC, &photometric);
			if (width == 0 || height == 0 || width > 16384 || height > 16384)
			{
				o_Error = "TIFF size is not supported";
				return false;
			}

			const bool grey = photometric == PHOTOMETRIC_MINISBLACK || photometric == PHOTOMETRIC_MINISWHITE;
			const bool simple = !TIFFIsTiled(t) && planar == PLANARCONFIG_CONTIG && samples >= 1 && samples <= 4 &&
				(grey ? samples <= 2 : (photometric == PHOTOMETRIC_RGB && samples >= 3)) &&
				((format == SAMPLEFORMAT_UINT && (bits == 8 || bits == 16)) || (format == SAMPLEFORMAT_IEEEFP && bits == 32));
			if (!simple)
			{
				//	libtiff converts everything else to RGBA8
				uint32_t* pixels = reinterpret_cast<uint32_t*>(Make2D(o_Image, e_R8G8B8A8_UNORM, width, height, 4));
				if (!TIFFReadRGBAImageOriented(t, width, height, pixels, ORIENTATION_TOPLEFT, 0) || !error.empty())
				{
					o_Error = error.empty() ? "TIFF image could not be read" : error;
					return false;
				}
				return true;
			}

			const size_t sampleBytes = bits / 8;
			const bool wide = bits == 16, real = format == SAMPLEFORMAT_IEEEFP;
			uint32_t dxgi;
			uint32_t channels = 4;
			if (samples == 1)
			{
				channels = 1;
				dxgi = real ? e_R32_FLOAT : (wide ? e_R16_UNORM : e_R8_UNORM);
			}
			else
			{
				dxgi = real ? e_R32G32B32A32_FLOAT : (wide ? e_R16G16B16A16_UNORM : e_R8G8B8A8_UNORM);
			}
			if (real && samples == 2)
			{
				o_Error = "TIFF grey + alpha float images are not supported";
				return false;
			}
			uint8_t* out = Make2D(o_Image, dxgi, width, height, channels * sampleBytes);
			const tmsize_t lineSize = TIFFScanlineSize(t);
			if (lineSize < (tmsize_t)(width * samples * sampleBytes))
			{
				o_Error = "TIFF scanline size is wrong";
				return false;
			}
			std::vector<uint8_t> line((size_t)lineSize);
			for (uint32_t y = 0; y < height; y++)
			{
				if (TIFFReadScanline(t, line.data(), y, 0) != 1 || !error.empty())
				{
					o_Error = error.empty() ? "TIFF pixel data could not be read" : error;
					return false;
				}
				uint8_t* dst = out + (size_t)y * width * channels * sampleBytes;
				for (uint32_t x = 0; x < width; x++)
				{
					const uint8_t* s = line.data() + (size_t)x * samples * sampleBytes;
					uint8_t* d = dst + (size_t)x * channels * sampleBytes;
					if (samples == 1)
					{
						memcpy(d, s, sampleBytes);
					}
					else if (samples == 2)
					{
						//	grey, grey, grey, alpha
						memcpy(d, s, sampleBytes);
						memcpy(d + sampleBytes, s, sampleBytes);
						memcpy(d + 2 * sampleBytes, s, sampleBytes);
						memcpy(d + 3 * sampleBytes, s + sampleBytes, sampleBytes);
					}
					else
					{
						memcpy(d, s, 3 * sampleBytes);
						if (samples == 4)
						{
							memcpy(d + 3 * sampleBytes, s + 3 * sampleBytes, sampleBytes);
						}
						else if (real)
						{
							const float one = 1.0f;
							memcpy(d + 3 * sampleBytes, &one, 4);
						}
						else
						{
							memset(d + 3 * sampleBytes, 0xff, sampleBytes);
						}
					}
				}
			}
			if (photometric == PHOTOMETRIC_MINISWHITE && !real)
			{
				//	white is 0: invert the grey (not alpha)
				for (size_t p = 0; p < (size_t)width * height; p++)
				{
					uint8_t* d = out + p * channels * sampleBytes;
					for (size_t b = 0; b < (channels == 1 ? 1 : 3) * sampleBytes; b++)
					{
						d[b] = (uint8_t)~d[b];
					}
				}
			}
			if (dxgi == e_R8G8B8A8_UNORM && exif_srgb(t))
			{
				o_Image.m_DxgiFormat = e_R8G8B8A8_UNORM_SRGB;
			}
			return true;
		}
	}
}
