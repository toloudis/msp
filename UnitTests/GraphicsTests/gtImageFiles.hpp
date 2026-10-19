/*****************************************************************************
**  gtImageFiles.hpp
**
**      The texture files the decoder and texture tests read: one small file
**      per case GraphicsVulkan's decoders must handle the way DirectXTex
**      and WIC do (the Plan 4a review focus): PNGs of every colour type and
**      bit depth with and without sRGB/gAMA chunks, JPEGs (with and without
**      the EXIF sRGB tag), BMPs, a GIF, TGAs (plain, RLE, with an all-zero
**      alpha channel), TIFFs (8-bit, 16-bit, float), an HDR, and DDS files
**      of every BC format, cubes, volumes, arrays and the legacy D3D9
**      formats. Written into a directory by gtWriteImageFiles; the
**      reference image (what GraphicsDX11 gets) comes from
**      gtLoadReference.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <DirectXTex.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct gtImageFile
{
	enum Kind { e_DDS, e_TGA, e_HDR, e_BMP, e_JPG, e_PNG, e_TIFF, e_GIF };
	//	How GraphicsDX11 reads it
	enum Reference
	{
		e_RefWIC,		// DirectX::LoadFromWICFile (PNG, JPEG, BMP, GIF, TIFF)
		e_RefTGA,		// DirectX::LoadFromTGAFile
		e_RefDDS,		// DirectX::LoadFromDDSFile
		e_RefHDR,		// GraphicsDX11's RGBE reader, floats to halves (matTextureLoaderHDRWin)
		e_RefCxV8U8,	// GraphicsDX11's own CxV8U8 DDS conversion (matTextureLoaderDDSWin)
	};

	std::string				m_Name;			// "png_gray8"
	std::filesystem::path	m_Path;
	Kind					m_Kind;
	Reference				m_Reference;
	int						m_Tolerance;	// largest per-channel difference allowed (8-bit units); 0 = exact
	bool					m_bLegacyDds;	// a D3D9-era DDS pixel format DirectXTex converts
};

//	Writes every case into i_Dir; returns them in a fixed order
std::vector<gtImageFile> gtWriteImageFiles(const std::filesystem::path& i_Dir);

//	What GraphicsDX11 decodes from the file, before any resize or mips.
//	(WIC needs COM, which GraphicsTestsMain starts: gtPlatform::StartCom.)
bool gtLoadReference(const gtImageFile& i_File, DirectX::ScratchImage& o_Image, std::string& o_Error);

