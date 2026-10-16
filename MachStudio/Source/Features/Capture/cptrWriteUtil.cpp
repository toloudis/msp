/****************************************************************************\
**  cptrWriteUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteUtil.hpp"

#include "Core/ch/chBinWriter.hpp"
#include "Core/ma/maIntRect.hpp"
#include "Graphics/g2d/g2dImage.hpp"

//
//	FileStream
//

FileStream::FileStream(chBinWriter &i_Writer)
:	m_pBinWriter(&i_Writer), m_pFileStream(NULL)
{
}

FileStream::FileStream(fsFileStream &i_Stream)
:	m_pBinWriter(NULL), m_pFileStream(&i_Stream)
{
}

void FileStream::Write(envType::Int64 i_NumBytes, const void* i_Data)
{
	if (m_pBinWriter) 
		m_pBinWriter->Write(i_Data, i_NumBytes);
	else if (m_pFileStream) 
		m_pFileStream->Write(i_NumBytes, i_Data);
}

namespace cptrWriteUtil
{


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fill_bmp_header(g2dBMPFormat::InfoHeader &i_Header, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_DestImageSize)
{
	//--------------------------------------------------------------//
	// BitmapInfoHeader
	//
	i_Header.m_Size				= sizeof( g2dBMPFormat::InfoHeader );
	i_Header.m_Width			= i_Width;
	i_Header.m_Height			= i_Height;
	i_Header.m_Planes			= 1;
	i_Header.m_BitCount			= (unsigned short)i_DestBitsPerPixel;
	i_Header.m_Compression		= g2dBMPFormat::c_CompressionRGB;
	i_Header.m_SizeImage		= i_DestImageSize;
	i_Header.m_XPelsPerMeter	= i_Width;
	i_Header.m_YPelsPerMeter	= i_Height;
	i_Header.m_ClrUsed			= 0;
	i_Header.m_ClrImportant		= 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_header(FileStream &i_File, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_PaletteSize,
					  int i_DestImageSize)
{
	//--------------------------------------------------------------//
	// BitmapFileHeader
	//
	g2dBMPFormat::InfoHeader	bih;
	g2dBMPFormat::FileHeader	bfh;

	bfh.m_Type		= g2dBMPFormat::c_FileType;
	bfh.m_Size		= sizeof( g2dBMPFormat::FileHeader ) + sizeof( bih ) +
					  i_PaletteSize + i_DestImageSize;
	bfh.m_Reserved1	= 0;
	bfh.m_Reserved2	= 0;
	bfh.m_OffBits	= sizeof( g2dBMPFormat::FileHeader ) + sizeof( bih ) +
					  i_PaletteSize;

	i_File.Write( sizeof( bfh ), &bfh );

	//--------------------------------------------------------------//
	// BitmapInfoHeader
	//
	cptrWriteUtil::fill_bmp_header(bih, i_Width, i_Height, i_DestBitsPerPixel, i_DestImageSize);

	i_File.Write( sizeof( bih ), &bih );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_header(	FileStream &i_File,
						g2dImage* i_pSurface,
						bool	i_bGreyscale /*= false*/,
						int		i_Sampling /*= 1*/,
						const maIntRect*	i_pRect /*= NULL*/ )
{
	//--------------------------------------------------------------//
	// Get width and height from rect or surface
	//
	int Width	= (i_pRect) ? i_pRect->GetWidth() : i_pSurface->GetWidth();
	int Height	= (i_pRect) ? i_pRect->GetHeight() : i_pSurface->GetHeight();

	if (i_Sampling > 1)
	{
		Width /= i_Sampling;
		Height /= i_Sampling;
	}

	if (i_bGreyscale)
	{
		//--------------------------------------------------------------//
		// 8-bit format
		//
		int DestBitsPerPixel = 8;
		int PaletteSize = 256;
		int DestImageSize	= Width * Height;

		write_bmp_header(i_File, Width, Height,
					DestBitsPerPixel, PaletteSize, DestImageSize);

		//--------------------------------------------------------------//
		// Palette
		//
		for ( int loop = 0; loop < 256; ++loop )
		{
			// grey: blue, green, red, reserved
			envType::UInt8 TempColor[4] = { (envType::UInt8) loop, (envType::UInt8) loop, (envType::UInt8) loop, 0 };
			i_File.Write( sizeof(TempColor), TempColor );
		}
	}
	else
	{
		//--------------------------------------------------------------//
		// 24-bit format
		//
		int DestBitsPerPixel = 24;
		int PaletteSize = 0;
		int DestImageSize	= 3 * Width * Height;

		write_bmp_header(i_File, Width, Height,
					DestBitsPerPixel, PaletteSize, DestImageSize);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data(FileStream &i_File,
					g2dImage* i_pSurface,
					bool	i_bGreyscale /*= false*/,
					int		i_Sampling /*= 1*/,
					const maIntRect* i_pRect /*= NULL*/)
{
	int wid=0,ht=0,bpp=0,bufSize=0;
	envType::UInt8* pBuffer = NULL;
	i_pSurface->GetPixels(
		&pBuffer, &bufSize, &wid, &ht, &bpp, 
		i_Sampling, i_bGreyscale, i_pRect);

	if (pBuffer)
	{
		i_File.Write( bufSize, pBuffer );
	}

	delete [] pBuffer;
	//DBG_LOG("Time to write: " << appTime::GetTime() - begin_time);
}
} // end namespace