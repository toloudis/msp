/*****************************************************************************
**  matImageDecodeDds.cpp
**
**      DDS files as GraphicsDX11 reads them (matTextureLoaderDDSWin):
**	DirectXTex's LoadFromDDSFile with DDS_FLAGS_NONE, ported here (from
**	DirectXTexDDS.cpp, DirectXTexConvert.cpp and DDS.h, MIT licence,
**	Copyright (c) Microsoft Corporation; the September 2024 release
**	GraphicsDX11 links), and GraphicsDX11's own conversion of D3D9's CxV8U8
**	normal maps to R8G8B8A8.
**
**	- "DX10" headers give the DXGI format, array size, cube flag and
**	  dimension directly
**	- D3D9-era headers go through DirectXTex's legacy format table
**	  (g_LegacyDDSMap below): formats D3D11 has are kept (A8R8G8B8 as
**	  B8G8R8A8, L8 as R8, V8U8 as R8G8_SNORM, ...), the rest are converted
**	  as DirectXTex converts them (R8G8B8 and palettes to R8G8B8A8,
**	  R3G3B2 to B5G6R5, A4L4 to B4G4R4A4, X formats with alpha set, the
**	  red/blue swap of D3DX's A2R10G10B10, ...)
**	- block-compressed data is copied as it is
**	- a file whose data is shorter than its mips, faces and slices need
**	  fails, as in DirectXTex
**
**	The decoded image keeps DirectXTex's format number. B8G8R8X8 images
**	get alpha 1 written into the X byte, so the B8G8R8A8 format Vulkan
**	uses for them (vkdFormat) samples as D3D samples X8. Formats with no
**	Vulkan equivalent (YUY2, R8G8_B8G8, ...) fail the decode.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif

#include <algorithm>
#include <cmath>

namespace gvk
{
	namespace matDecode
	{
		namespace
		{
			constexpr uint32_t fourcc(char a, char b, char c, char d)
			{
				return (uint32_t)(uint8_t)a | ((uint32_t)(uint8_t)b << 8) | ((uint32_t)(uint8_t)c << 16) | ((uint32_t)(uint8_t)d << 24);
			}

			//	DDS.h
			const uint32_t c_Magic = 0x20534444;		// "DDS "
			const size_t c_HeaderSize = 124, c_PixelFormatSize = 32, c_Dx10Size = 20;
			const size_t c_MinHeader = 4 + c_HeaderSize, c_Dx10Header = c_MinHeader + c_Dx10Size;

			const uint32_t DDS_FOURCC = 0x4, DDS_RGB = 0x40, DDS_RGBA = 0x41, DDS_LUMINANCE = 0x20000,
						   DDS_LUMINANCEA = 0x20001, DDS_ALPHAPIXELS = 0x1, DDS_ALPHA = 0x2, DDS_PAL8 = 0x20,
						   DDS_PAL8A = 0x21, DDS_BUMPLUMINANCE = 0x40000, DDS_BUMPDUDV = 0x80000, DDS_BUMPDUDVA = 0x80001;
			const uint32_t DDS_HEIGHT = 0x2, DDS_HEADER_FLAGS_VOLUME = 0x00800000, DDS_CUBEMAP = 0x200,
						   DDS_CUBEMAP_ALLFACES = 0x600 | 0xa00 | 0x1200 | 0x2200 | 0x4200 | 0x8200;
			const uint32_t DDS_DIMENSION_TEXTURE1D = 2, DDS_DIMENSION_TEXTURE2D = 3, DDS_DIMENSION_TEXTURE3D = 4;
			const uint32_t DDS_RESOURCE_MISC_TEXTURECUBE = 0x4;
			const uint32_t D3DFMT_CxV8U8 = 117;

			struct PixelFormat
			{
				uint32_t m_Size, m_Flags, m_FourCC, m_Bits, m_R, m_G, m_B, m_A;
			};

			//	DirectXTexDDS.cpp's CONVERSION_FLAGS
			enum Conversion : uint32_t
			{
				CONV_FLAGS_NONE = 0x0, CONV_FLAGS_EXPAND = 0x1, CONV_FLAGS_NOALPHA = 0x2, CONV_FLAGS_SWIZZLE = 0x4,
				CONV_FLAGS_PAL8 = 0x8, CONV_FLAGS_888 = 0x10, CONV_FLAGS_565 = 0x20, CONV_FLAGS_5551 = 0x40,
				CONV_FLAGS_4444 = 0x80, CONV_FLAGS_44 = 0x100, CONV_FLAGS_332 = 0x200, CONV_FLAGS_8332 = 0x400,
				CONV_FLAGS_A8P8 = 0x800, CONV_FLAGS_DX10 = 0x10000, CONV_FLAGS_PMALPHA = 0x20000,
				CONV_FLAGS_L6V5U5 = 0x200000, CONV_FLAGS_L8U8V8 = 0x400000, CONV_FLAGS_WUV10 = 0x800000,
			};

			//	DXGI numbers the table uses
			enum : uint32_t
			{
				BC1 = 71, BC2 = 74, BC3 = 77, BC4U = 80, BC4S = 81, BC5U = 83, BC5S = 84, BC6HU = 95, BC7 = 98,
				R8G8_B8G8 = 68, G8R8_G8B8 = 69, BGRA8 = 87, BGRX8 = 88, RGBA8 = 28, RG16 = 35, RGB10A2 = 24,
				B5G6R5 = 85, B5G5R5A1 = 86, R8 = 61, R16 = 56, RG8 = 49, A8 = 65, RGBA16 = 11, RGBA16S = 13,
				R16F = 54, RG16F = 34, RGBA16F = 10, R32F = 41, RG32F = 16, RGBA32F = 2, B4G4R4A4 = 115,
				YUY2 = 107, RG8S = 51, RGBA8S = 31, RG16S = 37,
			};

			struct Legacy
			{
				uint32_t	m_Format;
				uint32_t	m_Conversion;
				PixelFormat	m_Pf;
			};

			#define PF(flags, cc, bits, r, g, b, a) { 32, flags, cc, bits, r, g, b, a }
			#define FOURCC_PF(cc) PF(DDS_FOURCC, cc, 0, 0, 0, 0, 0)

			//	g_LegacyDDSMap, in its order
			const Legacy c_Legacy[] =
			{
				{ BC1, CONV_FLAGS_NONE, FOURCC_PF(fourcc('D', 'X', 'T', '1')) },
				{ BC2, CONV_FLAGS_NONE, FOURCC_PF(fourcc('D', 'X', 'T', '3')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('D', 'X', 'T', '5')) },
				{ BC2, CONV_FLAGS_PMALPHA, FOURCC_PF(fourcc('D', 'X', 'T', '2')) },
				{ BC3, CONV_FLAGS_PMALPHA, FOURCC_PF(fourcc('D', 'X', 'T', '4')) },
				//	DXT5 variants with swizzled channels, returned as BC3
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('A', '2', 'D', '5')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('x', 'G', 'B', 'R')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('R', 'x', 'B', 'G')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('R', 'B', 'x', 'G')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('x', 'R', 'B', 'G')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('R', 'G', 'x', 'B')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('x', 'G', 'x', 'R')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('G', 'X', 'R', 'B')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('G', 'R', 'X', 'B')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('R', 'X', 'G', 'B')) },
				{ BC3, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'R', 'G', 'X')) },

				{ BC4U, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '4', 'U')) },
				{ BC4S, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '4', 'S')) },
				{ BC5U, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '5', 'U')) },
				{ BC5S, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '5', 'S')) },

				{ BC4U, CONV_FLAGS_NONE, FOURCC_PF(fourcc('A', 'T', 'I', '1')) },
				{ BC5U, CONV_FLAGS_NONE, FOURCC_PF(fourcc('A', 'T', 'I', '2')) },
				{ BC5U, CONV_FLAGS_NONE, FOURCC_PF(fourcc('A', '2', 'X', 'Y')) },

				{ BC6HU, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '6', 'H')) },
				{ BC7, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '7', 'L')) },
				{ BC7, CONV_FLAGS_NONE, FOURCC_PF(fourcc('B', 'C', '7', '\0')) },

				{ R8G8_B8G8, CONV_FLAGS_NONE, FOURCC_PF(fourcc('R', 'G', 'B', 'G')) },
				{ G8R8_G8B8, CONV_FLAGS_NONE, FOURCC_PF(fourcc('G', 'R', 'G', 'B')) },

				{ BGRA8, CONV_FLAGS_NONE, PF(DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000) },		// A8R8G8B8
				{ BGRX8, CONV_FLAGS_NONE, PF(DDS_RGB, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0) },					// X8R8G8B8
				{ RGBA8, CONV_FLAGS_NONE, PF(DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000) },		// A8B8G8R8
				{ RGBA8, CONV_FLAGS_NOALPHA, PF(DDS_RGB, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0) },				// X8B8G8R8
				{ RG16, CONV_FLAGS_NONE, PF(DDS_RGB, 0, 32, 0x0000ffff, 0xffff0000, 0, 0) },							// G16R16

				{ RGB10A2, CONV_FLAGS_SWIZZLE, PF(DDS_RGBA, 0, 32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) },	// A2R10G10B10 (D3DX reversal)
				{ RGB10A2, CONV_FLAGS_NONE, PF(DDS_RGBA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) },		// A2B10G10R10 (D3DX reversal)

				{ RGBA8, CONV_FLAGS_EXPAND | CONV_FLAGS_NOALPHA | CONV_FLAGS_888, PF(DDS_RGB, 0, 24, 0xff0000, 0x00ff00, 0x0000ff, 0) },	// R8G8B8

				{ B5G6R5, CONV_FLAGS_565, PF(DDS_RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0) },								// R5G6B5
				{ B5G5R5A1, CONV_FLAGS_5551, PF(DDS_RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000) },					// A1R5G5B5
				{ B5G5R5A1, CONV_FLAGS_5551 | CONV_FLAGS_NOALPHA, PF(DDS_RGB, 0, 16, 0x7c00, 0x03e0, 0x001f, 0) },		// X1R5G5B5

				{ RGBA8, CONV_FLAGS_EXPAND | CONV_FLAGS_8332, PF(DDS_RGBA, 0, 16, 0x00e0, 0x001c, 0x0003, 0xff00) },	// A8R3G3B2
				{ B5G6R5, CONV_FLAGS_EXPAND | CONV_FLAGS_332, PF(DDS_RGB, 0, 8, 0xe0, 0x1c, 0x03, 0) },				// R3G3B2

				{ R8, CONV_FLAGS_NONE, PF(DDS_LUMINANCE, 0, 8, 0xff, 0, 0, 0) },										// L8
				{ R16, CONV_FLAGS_NONE, PF(DDS_LUMINANCE, 0, 16, 0xffff, 0, 0, 0) },									// L16
				{ RG8, CONV_FLAGS_NONE, PF(DDS_LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00) },								// A8L8
				{ RG8, CONV_FLAGS_NONE, PF(DDS_LUMINANCEA, 0, 8, 0x00ff, 0, 0, 0xff00) },								// A8L8 (alternative bit count)

				//	NVTT v1 wrote these with RGB instead of LUMINANCE
				{ R8, CONV_FLAGS_NONE, PF(DDS_RGB, 0, 8, 0xff, 0, 0, 0) },
				{ R16, CONV_FLAGS_NONE, PF(DDS_RGB, 0, 16, 0xffff, 0, 0, 0) },
				{ RG8, CONV_FLAGS_NONE, PF(DDS_RGBA, 0, 16, 0x00ff, 0, 0, 0xff00) },

				{ A8, CONV_FLAGS_NONE, PF(DDS_ALPHA, 0, 8, 0, 0, 0, 0xff) },											// A8

				{ RGBA16, CONV_FLAGS_NONE, FOURCC_PF(36) },			// A16B16G16R16
				{ RGBA16S, CONV_FLAGS_NONE, FOURCC_PF(110) },		// Q16W16V16U16
				{ R16F, CONV_FLAGS_NONE, FOURCC_PF(111) },			// R16F
				{ RG16F, CONV_FLAGS_NONE, FOURCC_PF(112) },			// G16R16F
				{ RGBA16F, CONV_FLAGS_NONE, FOURCC_PF(113) },		// A16B16G16R16F
				{ R32F, CONV_FLAGS_NONE, FOURCC_PF(114) },			// R32F
				{ RG32F, CONV_FLAGS_NONE, FOURCC_PF(115) },			// G32R32F
				{ RGBA32F, CONV_FLAGS_NONE, FOURCC_PF(116) },		// A32B32G32R32F

				{ R32F, CONV_FLAGS_NONE, PF(DDS_RGB, 0, 32, 0xffffffff, 0, 0, 0) },									// R32F (D3DX uses FourCC 114 instead)

				{ RGBA8, CONV_FLAGS_EXPAND | CONV_FLAGS_PAL8 | CONV_FLAGS_A8P8, PF(DDS_PAL8A, 0, 16, 0, 0, 0, 0xff00) },	// A8P8
				{ RGBA8, CONV_FLAGS_EXPAND | CONV_FLAGS_PAL8, PF(DDS_PAL8, 0, 8, 0, 0, 0, 0) },								// P8

				{ B4G4R4A4, CONV_FLAGS_4444, PF(DDS_RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000) },					// A4R4G4B4
				{ B4G4R4A4, CONV_FLAGS_NOALPHA | CONV_FLAGS_4444, PF(DDS_RGB, 0, 16, 0x0f00, 0x00f0, 0x000f, 0) },		// X4R4G4B4
				{ B4G4R4A4, CONV_FLAGS_EXPAND | CONV_FLAGS_44, PF(DDS_LUMINANCEA, 0, 8, 0x0f, 0, 0, 0xf0) },			// A4L4

				{ YUY2, CONV_FLAGS_NONE, FOURCC_PF(fourcc('Y', 'U', 'Y', '2')) },
				{ YUY2, CONV_FLAGS_SWIZZLE, FOURCC_PF(fourcc('U', 'Y', 'V', 'Y')) },

				{ RG8S, CONV_FLAGS_NONE, PF(DDS_BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0, 0) },								// V8U8
				{ RGBA8S, CONV_FLAGS_NONE, PF(DDS_BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000) },	// Q8W8V8U8
				{ RG16S, CONV_FLAGS_NONE, PF(DDS_BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0, 0) },						// V16U16

				{ RGBA8, CONV_FLAGS_L6V5U5 | CONV_FLAGS_EXPAND, PF(DDS_BUMPLUMINANCE, 0, 16, 0x001f, 0x03e0, 0xfc00, 0) },	// L6V5U5
				{ RGBA8, CONV_FLAGS_L8U8V8, PF(DDS_BUMPLUMINANCE, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0) },		// X8L8V8U8
				{ RGB10A2, CONV_FLAGS_WUV10, PF(DDS_BUMPDUDVA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) },	// A2W10V10U10
			};

			#undef PF
			#undef FOURCC_PF

			//	GetDXGIFormat (DDS_FLAGS_NONE); 0 if the pixel format is unknown
			uint32_t legacy_format(const PixelFormat& i_Pf, uint32_t i_Reserved9, uint32_t& o_Conversion)
			{
				uint32_t flags = i_Pf.m_Flags;
				if (i_Reserved9 == fourcc('N', 'V', 'T', 'T'))
				{
					flags &= ~0xC0000000;		// non-standard nVidia flags (DDPF_SRGB | DDPF_NORMAL)
				}
				const Legacy* found = nullptr;
				for (const Legacy& entry : c_Legacy)
				{
					const PixelFormat& e = entry.m_Pf;
					if (i_Pf.m_Size == 0 && i_Pf.m_Flags == 0 && i_Pf.m_FourCC != 0)
					{
						//	DDS files whose pixel format is mostly zero
						if ((e.m_Flags & DDS_FOURCC) && i_Pf.m_FourCC == e.m_FourCC)
						{
							found = &entry;
							break;
						}
						continue;
					}
					if ((flags & DDS_FOURCC) && (e.m_Flags & DDS_FOURCC))
					{
						if (i_Pf.m_FourCC == e.m_FourCC)
						{
							found = &entry;
							break;
						}
					}
					else if (flags == e.m_Flags && i_Pf.m_Bits == e.m_Bits)
					{
						bool match;
						if (e.m_Flags & DDS_PAL8)
						{
							match = true;
						}
						else if (e.m_Flags & DDS_ALPHA)
						{
							match = i_Pf.m_A == e.m_A;
						}
						else if (e.m_Flags & (DDS_LUMINANCE | DDS_BUMPDUDV))
						{
							match = i_Pf.m_R == e.m_R && (!(e.m_Flags & DDS_ALPHAPIXELS) || i_Pf.m_A == e.m_A);
						}
						else if (e.m_Flags & DDS_ALPHAPIXELS)
						{
							match = i_Pf.m_R == e.m_R && i_Pf.m_G == e.m_G && i_Pf.m_B == e.m_B && i_Pf.m_A == e.m_A;
						}
						else
						{
							match = i_Pf.m_R == e.m_R && i_Pf.m_G == e.m_G && i_Pf.m_B == e.m_B;
						}
						if (match)
						{
							found = &entry;
							break;
						}
					}
				}
				if (!found)
				{
					return 0;
				}
				o_Conversion = found->m_Conversion;
				uint32_t format = found->m_Format;
				if (i_Reserved9 == fourcc('N', 'V', 'T', 'T') && (i_Pf.m_Flags & 0x40000000))
				{
					format = MakeSrgb(format);
				}
				return format;
			}

			uint32_t mip_size(uint32_t i_Size, uint32_t i_Mip)
			{
				return std::max(1u, i_Size >> i_Mip);
			}

			//	CalculateMipLevels(3D): the most mips the size allows
			uint32_t max_mips(uint32_t i_W, uint32_t i_H, uint32_t i_D)
			{
				uint32_t mips = 1;
				while (i_W > 1 || i_H > 1 || i_D > 1)
				{
					i_W = std::max(1u, i_W >> 1);
					i_H = std::max(1u, i_H >> 1);
					i_D = std::max(1u, i_D >> 1);
					mips++;
				}
				return mips;
			}

			//	Rows and bytes per row of one subresource in the file
			//	(ComputePitch with the source bit count the conversion implies)
			struct Pitch { size_t m_Row; uint32_t m_Rows; };

			Pitch source_pitch(const vkdFormatInfo& i_Info, uint32_t i_SourceBits, uint32_t i_W, uint32_t i_H)
			{
				if (i_Info.m_bCompressed)
				{
					return { (size_t)std::max(1u, (i_W + 3) / 4) * i_Info.m_BlockBytes, std::max(1u, (i_H + 3) / 4) };
				}
				return { ((size_t)i_W * i_SourceBits + 7) / 8, i_H };
			}

			//	LegacyExpandScanline / LegacyConvertScanline / SwizzleScanline /
			//	CopyScanline, for the conversions the legacy table uses
			void convert_row(uint8_t* o_pDst, size_t i_DstBytes, const uint8_t* i_pSrc, size_t i_SrcBytes, uint32_t i_Format,
							 uint32_t i_Conversion, const uint32_t* i_pPalette)
			{
				const bool setAlpha = (i_Conversion & CONV_FLAGS_NOALPHA) != 0;
				auto put32 = [&](size_t i, uint32_t v) { memcpy(o_pDst + i * 4, &v, 4); };
				auto put16 = [&](size_t i, uint16_t v) { memcpy(o_pDst + i * 2, &v, 2); };
				if (i_Conversion & CONV_FLAGS_EXPAND)
				{
					if (i_Conversion & CONV_FLAGS_PAL8)
					{
						if (i_Conversion & CONV_FLAGS_A8P8)
						{
							for (size_t i = 0; i * 2 + 1 < i_SrcBytes && i * 4 + 3 < i_DstBytes; i++)
							{
								const uint16_t t = Read16(i_pSrc + i * 2);
								const uint32_t ta = setAlpha ? 0xff000000 : (uint32_t)((t & 0xff00) << 16);
								put32(i, i_pPalette[t & 0xff] | ta);
							}
						}
						else
						{
							for (size_t i = 0; i < i_SrcBytes && i * 4 + 3 < i_DstBytes; i++)
							{
								put32(i, i_pPalette[i_pSrc[i]]);
							}
						}
					}
					else if (i_Conversion & CONV_FLAGS_888)
					{
						//	24bpp Direct3D 9 files are BGR
						for (size_t i = 0; i * 3 + 2 < i_SrcBytes && i * 4 + 3 < i_DstBytes; i++)
						{
							const uint8_t* s = i_pSrc + i * 3;
							put32(i, ((uint32_t)s[0] << 16) | ((uint32_t)s[1] << 8) | s[2] | 0xff000000);
						}
					}
					else if (i_Conversion & CONV_FLAGS_332)
					{
						//	R3G3B2 -> B5G6R5
						for (size_t i = 0; i < i_SrcBytes && i * 2 + 1 < i_DstBytes; i++)
						{
							const unsigned t = i_pSrc[i];
							const unsigned t1 = ((t & 0xe0u) << 8) | ((t & 0xc0u) << 5);
							const unsigned t2 = ((t & 0x1cu) << 6) | ((t & 0x1cu) << 3);
							const unsigned t3 = ((t & 0x03u) << 3) | ((t & 0x03u) << 1) | ((t & 0x02) >> 1);
							put16(i, (uint16_t)(t1 | t2 | t3));
						}
					}
					else if (i_Conversion & CONV_FLAGS_8332)
					{
						for (size_t i = 0; i * 2 + 1 < i_SrcBytes && i * 4 + 3 < i_DstBytes; i++)
						{
							const uint16_t t = Read16(i_pSrc + i * 2);
							const uint32_t t1 = (uint32_t)((t & 0x00e0) | ((t & 0x00e0) >> 3) | ((t & 0x00c0) >> 6));
							const uint32_t t2 = (uint32_t)(((t & 0x001c) << 11) | ((t & 0x001c) << 8) | ((t & 0x0018) << 5));
							const uint32_t t3 = (uint32_t)(((t & 0x0003) << 22) | ((t & 0x0003) << 20) | ((t & 0x0003) << 18) | ((t & 0x0003) << 16));
							const uint32_t ta = setAlpha ? 0xff000000 : (uint32_t)((t & 0xff00) << 16);
							put32(i, t1 | t2 | t3 | ta);
						}
					}
					else if (i_Conversion & CONV_FLAGS_44)
					{
						//	A4L4 -> B4G4R4A4
						for (size_t i = 0; i < i_SrcBytes && i * 2 + 1 < i_DstBytes; i++)
						{
							const unsigned t = i_pSrc[i];
							const unsigned t1 = (t & 0x0fu);
							const unsigned ta = setAlpha ? 0xf000u : ((t & 0xf0u) << 8);
							put16(i, (uint16_t)(t1 | (t1 << 4) | (t1 << 8) | ta));
						}
					}
					else if (i_Conversion & CONV_FLAGS_L6V5U5)
					{
						for (size_t i = 0; i * 2 + 1 < i_SrcBytes && i * 4 + 3 < i_DstBytes; i++)
						{
							const uint16_t t = Read16(i_pSrc + i * 2);
							const uint32_t t1 = ((t & 0xFC00) >> 8) | ((t & 0xC000) >> 14);
							const int m = 1 << 4;
							const int8_t v = (int8_t)(((((t >> 5) & 0x1f) ^ m) - m) + 16);
							const int8_t u = (int8_t)((((t & 0x1f) ^ m) - m) + 16);
							const uint32_t t2 = (uint32_t)(u << 3 | u >> 2);
							const uint32_t t3 = (uint32_t)(v << 3 | v >> 2);
							put32(i, t1 | (t2 << 8) | (t3 << 16) | 0xff000000);
						}
					}
					return;
				}
				const size_t count = std::min(i_DstBytes, i_SrcBytes) / 4;
				if (i_Conversion & CONV_FLAGS_SWIZZLE)
				{
					if (i_Format == RGB10A2)
					{
						//	Red and blue swapped (D3DFMT_A2R10G10B10)
						for (size_t i = 0; i < count; i++)
						{
							const uint32_t t = Read32(i_pSrc + i * 4);
							const uint32_t ta = setAlpha ? 0xC0000000 : (t & 0xC0000000);
							put32(i, ((t & 0x3ff00000) >> 20) | ((t & 0x000003ff) << 20) | (t & 0x000ffc00) | ta);
						}
						return;
					}
				}
				else if (i_Conversion & CONV_FLAGS_L8U8V8)
				{
					for (size_t i = 0; i < count; i++)
					{
						const uint32_t t = Read32(i_pSrc + i * 4);
						const uint32_t t1 = (t >> 16) & 0xff;
						const int m = 1 << 7;
						const uint32_t v = (uint32_t)(((((t >> 8) & 0xff) ^ m) - m) + 128);
						const uint32_t u = (uint32_t)((((t & 0xff) ^ m) - m) + 128);
						put32(i, t1 | (u << 8) | (v << 16) | 0xff000000);
					}
					return;
				}
				else if (i_Conversion & CONV_FLAGS_WUV10)
				{
					for (size_t i = 0; i < count; i++)
					{
						const uint32_t t = Read32(i_pSrc + i * 4);
						const int m = 1 << 9;
						const uint32_t w = (uint32_t)(((((t >> 20) & 0x3ff) ^ m) - m) + 512);
						const uint32_t v = (uint32_t)(((((t >> 10) & 0x3ff) ^ m) - m) + 512);
						const uint32_t u = (uint32_t)((((t & 0x3ff) ^ m) - m) + 512);
						const uint32_t ta = setAlpha ? 0xC0000000 : (t & 0xC0000000);
						put32(i, u | (v << 10) | (w << 20) | ta);
					}
					return;
				}
				//	CopyScanline: a copy, setting alpha for the X formats
				memcpy(o_pDst, i_pSrc, std::min(i_DstBytes, i_SrcBytes));
				if (setAlpha)
				{
					switch (i_Format)
					{
						case RGBA8:
							for (size_t i = 0; i < count; i++)
							{
								o_pDst[i * 4 + 3] = 0xff;
							}
							break;
						case B5G5R5A1:
						case B4G4R4A4:
						{
							const uint16_t alpha = (i_Format == B4G4R4A4) ? 0xF000 : 0x8000;
							for (size_t i = 0; i * 2 + 1 < std::min(i_DstBytes, i_SrcBytes); i++)
							{
								put16(i, (uint16_t)(Read16(o_pDst + i * 2) | alpha));
							}
							break;
						}
						default:
							break;
					}
				}
			}

			//	GraphicsDX11's CxV8U8 conversion (CreateTextureFromDDS): x and y
			//	from the signed bytes, z = sqrt(1 - x^2 - y^2), to R8G8B8A8
			bool decode_cxv8u8(const uint8_t* i_pData, size_t i_Size, uint32_t i_Width, uint32_t i_Height, uint32_t i_Mips,
							   matDecodedImage& o_Image, std::string& o_Error)
			{
				if (i_Mips == 0)
				{
					i_Mips = 1;
				}
				if (i_Mips > 15 || i_Width == 0 || i_Height == 0 || i_Width > 16384 || i_Height > 16384 ||
					i_Mips > max_mips(i_Width, i_Height, 1))
				{
					o_Error = "CxV8U8 DDS has an unsupported size";
					return false;
				}
				size_t need = 0;
				for (uint32_t m = 0; m < i_Mips; m++)
				{
					need += (size_t)mip_size(i_Width, m) * mip_size(i_Height, m) * 2;
				}
				if (need > i_Size - c_MinHeader)
				{
					o_Error = "CxV8U8 DDS pixel data is truncated";
					return false;
				}
				o_Image = matDecodedImage();
				o_Image.m_DxgiFormat = RGBA8;
				o_Image.m_Width = i_Width;
				o_Image.m_Height = i_Height;
				o_Image.m_Mips = i_Mips;
				const char* src = reinterpret_cast<const char*>(i_pData + c_MinHeader);
				for (uint32_t m = 0; m < i_Mips; m++)
				{
					const uint32_t w = mip_size(i_Width, m), h = mip_size(i_Height, m);
					const size_t offset = o_Image.m_Bytes.size();
					o_Image.m_Subs.push_back({ offset, (size_t)w * 4, (size_t)w * 4 * h });
					o_Image.m_Bytes.resize(offset + (size_t)w * h * 4);
					uint8_t* dst = o_Image.m_Bytes.data() + offset;
					for (uint32_t y = 0; y < h; y++)
					{
						for (uint32_t x = 0; x < w; x++)
						{
							const char rc = src[y * w * 2 + x * 2 + 0];
							const char gc = src[y * w * 2 + x * 2 + 1];
							const float r = rc / 127.0f * 2.0f - 1.0f;
							const float g = gc / 127.0f * 2.0f - 1.0f;
							const float b = std::sqrt(std::max(0.0f, 1.0f - r * r - g * g));
							uint8_t* d = dst + (y * w + x) * 4;
							d[0] = (uint8_t)((r + 1.0f) * 0.5f * 255);
							d[1] = (uint8_t)((g + 1.0f) * 0.5f * 255);
							d[2] = (uint8_t)((b + 1.0f) * 0.5f * 255);
							d[3] = 255;
						}
					}
					src += (size_t)w * h * 2;
				}
				return true;
			}
		}

		bool DecodeDds(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error)
		{
			if (i_Size < c_MinHeader)
			{
				o_Error = "DDS header is truncated";
				return false;
			}
			if (Read32(i_pData) != c_Magic)
			{
				o_Error = "not a DDS file";
				return false;
			}
			const uint8_t* h = i_pData + 4;
			if (Read32(h) != c_HeaderSize || Read32(h + 72) != c_PixelFormatSize)
			{
				o_Error = "DDS header has the wrong size";
				return false;
			}
			const uint32_t headerFlags = Read32(h + 4);
			const uint32_t height = Read32(h + 8), width = Read32(h + 12), depth = Read32(h + 20);
			uint32_t mips = Read32(h + 24);
			const uint32_t reserved9 = Read32(h + 28 + 9 * 4);
			PixelFormat pf;
			pf.m_Size = Read32(h + 72);
			pf.m_Flags = Read32(h + 76);
			pf.m_FourCC = Read32(h + 80);
			pf.m_Bits = Read32(h + 84);
			pf.m_R = Read32(h + 88);
			pf.m_G = Read32(h + 92);
			pf.m_B = Read32(h + 96);
			pf.m_A = Read32(h + 100);
			const uint32_t caps2 = Read32(h + 108);

			if ((pf.m_Flags & DDS_FOURCC) && pf.m_FourCC == D3DFMT_CxV8U8)
			{
				return decode_cxv8u8(i_pData, i_Size, width, height, mips, o_Image, o_Error);
			}

			if (mips == 0)
			{
				mips = 1;
			}
			uint32_t format = 0, conversion = 0, layers = 1, w = width, ht = height, d = 1;
			bool cube = false, volume = false;
			size_t offset = c_MinHeader;
			if ((pf.m_Flags & DDS_FOURCC) && pf.m_FourCC == fourcc('D', 'X', '1', '0'))
			{
				if (i_Size < c_Dx10Header)
				{
					o_Error = "DDS DX10 header is truncated";
					return false;
				}
				const uint8_t* x = i_pData + c_MinHeader;
				conversion |= CONV_FLAGS_DX10;
				format = Read32(x);
				const uint32_t dimension = Read32(x + 4), misc = Read32(x + 8);
				layers = std::max(1u, Read32(x + 12));
				offset = c_Dx10Header;
				switch (dimension)
				{
					case DDS_DIMENSION_TEXTURE1D:
						if ((headerFlags & DDS_HEIGHT) && height != 1)
						{
							o_Error = "DDS 1D texture with a height";
							return false;
						}
						ht = 1;
						break;
					case DDS_DIMENSION_TEXTURE2D:
						if (misc & DDS_RESOURCE_MISC_TEXTURECUBE)
						{
							cube = true;
							layers *= 6;
						}
						break;
					case DDS_DIMENSION_TEXTURE3D:
						if (!(headerFlags & DDS_HEADER_FLAGS_VOLUME) || layers > 1)
						{
							o_Error = "DDS volume texture header is invalid";
							return false;
						}
						volume = true;
						d = depth;
						break;
					default:
						o_Error = "DDS resource dimension is invalid";
						return false;
				}
			}
			else
			{
				if (headerFlags & DDS_HEADER_FLAGS_VOLUME)
				{
					volume = true;
					d = depth;
				}
				else if (caps2 & DDS_CUBEMAP)
				{
					if ((caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
					{
						o_Error = "DDS cube map without all six faces";
						return false;
					}
					cube = true;
					layers = 6;
				}
				format = legacy_format(pf, reserved9, conversion);
				if (format == 0)
				{
					o_Error = "DDS pixel format is not supported";
					return false;
				}
			}

			const VkFormat vk = vkdFormatFromDxgi(format);
			if (vk == VK_FORMAT_UNDEFINED)
			{
				o_Error = "DDS format " + std::to_string(format) + " has no Vulkan equivalent";
				return false;
			}
			if (w == 0 || ht == 0 || d == 0 || w > 16384 || ht > 16384 || mips > 15 || layers > 2048 || d > 2048 ||
				mips > max_mips(w, ht, d))
			{
				o_Error = "DDS size is not supported";
				return false;
			}

			const uint32_t* palette = nullptr;
			uint32_t paletteBuffer[256];
			if (conversion & CONV_FLAGS_PAL8)
			{
				if (i_Size - offset < 256 * 4)
				{
					o_Error = "DDS palette is truncated";
					return false;
				}
				memcpy(paletteBuffer, i_pData + offset, sizeof(paletteBuffer));
				palette = paletteBuffer;
				offset += 256 * 4;
			}

			//	The bits per pixel in the file
			const vkdFormatInfo info = vkdFormatInfoOf(vk);
			uint32_t sourceBits = info.m_BlockBytes * 8;
			if (conversion & CONV_FLAGS_EXPAND)
			{
				if (conversion & CONV_FLAGS_888)
				{
					sourceBits = 24;
				}
				else if (conversion & (CONV_FLAGS_565 | CONV_FLAGS_5551 | CONV_FLAGS_4444 | CONV_FLAGS_8332 | CONV_FLAGS_A8P8 | CONV_FLAGS_L6V5U5))
				{
					sourceBits = 16;
				}
				else if (conversion & (CONV_FLAGS_44 | CONV_FLAGS_332 | CONV_FLAGS_PAL8))
				{
					sourceBits = 8;
				}
			}

			//	Every subresource must be in the file
			size_t need = 0;
			for (uint32_t layer = 0; layer < layers; layer++)
			{
				for (uint32_t m = 0; m < mips; m++)
				{
					const Pitch p = source_pitch(info, sourceBits, mip_size(w, m), mip_size(ht, m));
					need += p.m_Row * p.m_Rows * mip_size(d, m);
				}
			}
			if (need > i_Size - offset)
			{
				o_Error = "DDS pixel data is truncated";
				return false;
			}

			o_Image = matDecodedImage();
			o_Image.m_DxgiFormat = format;
			o_Image.m_Width = w;
			o_Image.m_Height = ht;
			o_Image.m_Depth = d;
			o_Image.m_Mips = mips;
			o_Image.m_Layers = layers;
			o_Image.m_bCube = cube;
			(void)volume;

			//	The file holds the subresources in the same order as m_Subs
			const uint8_t* src = i_pData + offset;
			for (uint32_t layer = 0; layer < layers; layer++)
			{
				for (uint32_t m = 0; m < mips; m++)
				{
					const uint32_t mw = mip_size(w, m), mh = mip_size(ht, m), md = mip_size(d, m);
					const Pitch in = source_pitch(info, sourceBits, mw, mh);
					const size_t outRow = (size_t)(mw + info.m_BlockWidth - 1) / info.m_BlockWidth * info.m_BlockBytes;
					const uint32_t outRows = (mh + info.m_BlockHeight - 1) / info.m_BlockHeight;
					const size_t at = o_Image.m_Bytes.size();
					o_Image.m_Subs.push_back({ at, outRow, outRow * outRows });
					o_Image.m_Bytes.resize(at + outRow * outRows * md);
					uint8_t* dst = o_Image.m_Bytes.data() + at;
					for (uint32_t slice = 0; slice < md; slice++)
					{
						for (uint32_t row = 0; row < in.m_Rows; row++)
						{
							if (info.m_bCompressed)
							{
								memcpy(dst, src, std::min(in.m_Row, outRow));
							}
							else
							{
								convert_row(dst, outRow, src, in.m_Row, format, conversion, palette);
							}
							src += in.m_Row;
							dst += outRow;
						}
					}
				}
			}

			//	X8 formats: alpha 1, as D3D reads them (Vulkan uses the A8 format)
			if (format == BGRX8 || format == 93)
			{
				for (size_t i = 3; i < o_Image.m_Bytes.size(); i += 4)
				{
					o_Image.m_Bytes[i] = 0xff;
				}
			}
			return true;
		}
	}
}
