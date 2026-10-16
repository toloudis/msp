/*****************************************************************************
**	g2dBMPFormat.hpp
**
**		Headers of the BMP image file format.  These have the same layout
**	as the Windows BITMAPFILEHEADER and BITMAPINFOHEADER structures, so
**	files written with them are ordinary .bmp files.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_BMPFORMAT_HPP
#error g2dBMPFormat.hpp multiply included
#endif
#define G2D_BMPFORMAT_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
//============================================================================
namespace g2dBMPFormat
{
	// "BM", stored little-endian
	const envType::UInt16 c_FileType = 0x4D42;

	// uncompressed pixels
	const envType::UInt32 c_CompressionRGB = 0;

#pragma pack(push, 2)
	//------------------------------------------------------------------------
	//	FileHeader starts the file.
	//------------------------------------------------------------------------
	struct FileHeader
	{
		envType::UInt16	m_Type;			// c_FileType
		envType::UInt32	m_Size;			// size of the whole file in bytes
		envType::UInt16	m_Reserved1;
		envType::UInt16	m_Reserved2;
		envType::UInt32	m_OffBits;		// offset from the file start to the pixels
	};
#pragma pack(pop)

	//------------------------------------------------------------------------
	//	InfoHeader follows the FileHeader and describes the image.
	//------------------------------------------------------------------------
	struct InfoHeader
	{
		envType::UInt32	m_Size;			// sizeof(InfoHeader)
		envType::Int32	m_Width;
		envType::Int32	m_Height;		// positive means bottom-up rows
		envType::UInt16	m_Planes;		// always 1
		envType::UInt16	m_BitCount;		// bits per pixel
		envType::UInt32	m_Compression;	// c_CompressionRGB
		envType::UInt32	m_SizeImage;	// size of the pixels in bytes
		envType::Int32	m_XPelsPerMeter;
		envType::Int32	m_YPelsPerMeter;
		envType::UInt32	m_ClrUsed;
		envType::UInt32	m_ClrImportant;
	};

	static_assert(sizeof(FileHeader) == 14, "BMP file header must be 14 bytes");
	static_assert(sizeof(InfoHeader) == 40, "BMP info header must be 40 bytes");
}
