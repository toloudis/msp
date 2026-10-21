/*****************************************************************************
**  matImageEncode.hpp
**
**      Image files as GraphicsDX11 writes them, without WIC or DirectXTex
**	(Plan 6 decision 3). DX11 saves images and textures through
**	DirectXTex's SaveToWICFile (BMP, JPG, PNG, TIFF) and SaveToDDSFile;
**	each encoder here writes the layout those give for the same pixels
**	(measured, Plan 6 Task 2; the table is in the plan's ledger):
**
**	- the WIC pixel format DirectXTex picks for the DXGI format, then the
**	  one WIC's encoder takes in its place, and WIC's conversion between
**	  them: floats to 16-bit integers through the sRGB curve (alpha
**	  linear, both clamped), 16-bit integers and floats to BMP's 64-bit
**	  fixed point (s2.13, linear), 8-bit grey to an 8-bit palette (and
**	  WIC's lookup into it, which reduces each grey to 4 bits first).
**	  WIC's sRGB curves are approximations: where the result isn't an
**	  8-bit value, it can differ from the exact curve computed here by
**	  2/65535 (float to 16-bit) and 1/8192 (16-bit to fixed point)
**	- BMP: WIC's headers byte for byte (a V5 header for 32-bit BGRA)
**	- PNG: DirectXTex's gAMA chunk (gamma 1.0; sRGB for _SRGB formats) and
**	  WIC's pHYs (72 dpi); no tEXt chunk naming DirectXTex
**	- TIFF: LZW, horizontal prediction for 8-bit samples, unassociated
**	  alpha, 72 dpi, WIC's strip size; no Software or XMP tags
**	- JPG: stb_image_write at WIC's default quality (0.9 -> 90, 4:2:0);
**	  grey images become three-component JPEGs (stb_image_write writes
**	  only YCbCr), where WIC writes one component
**	- DDS: DirectXTex's EncodeDDSHeader (ported from DirectXTexDDS.cpp and
**	  DDS.h, MIT licence, Copyright (c) Microsoft Corporation; the
**	  September 2024 release GraphicsDX11 links) for one 2D image, and its
**	  rows; uncompressed formats only
**
**	Source formats: R8G8B8A8, B8G8R8A8, B8G8R8X8 (and their _SRGB forms),
**	R8, R16G16B16A16_UNORM, R16G16B16A16_FLOAT, R32G32B32A32_FLOAT,
**	R16_FLOAT, R32_FLOAT (and D32_FLOAT): what GraphicsVulkan's images and
**	textures have and DirectXTex can write through WIC. DDS takes any
**	uncompressed format. Each encoder throws g2dImageSaveX for a format
**	it can't write (DX11's SaveToWICFile fails for those too, but also
**	writes a few formats GraphicsVulkan never has: 10-bit, 565, ...).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATIMAGEENCODE_HPP
#error matImageEncode.hpp multiply included
#endif
#define GVK_MATIMAGEENCODE_HPP

#include <cstdint>
#include <vector>

class fsLocator;

namespace gvk
{
	struct matEncodeImage
	{
		int				m_Width;
		int				m_Height;
		int				m_Stride;			// bytes per row
		uint32_t		m_DxgiFormat;		// a DXGI_FORMAT value, as vkdFormat uses
		const uint8_t*	m_pPixels;			// top row first
	};

	std::vector<uint8_t> matEncodeBMP(const matEncodeImage& i_Image);
	std::vector<uint8_t> matEncodeJPG(const matEncodeImage& i_Image);
	std::vector<uint8_t> matEncodePNG(const matEncodeImage& i_Image);
	std::vector<uint8_t> matEncodeTIFF(const matEncodeImage& i_Image);
	std::vector<uint8_t> matEncodeDDS(const matEncodeImage& i_Image);

	//	Writes the bytes to the file (replacing it); throws g2dImageSaveX
	void matEncodeWriteFile(const fsLocator& i_File, const std::vector<uint8_t>& i_Bytes);
}
