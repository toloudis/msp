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
void fill_bmp_header(BITMAPINFOHEADER &i_Header, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_DestImageSize)
{
	//--------------------------------------------------------------//
	// BitmapInfoHeader
	//
	i_Header.biSize				= sizeof( BITMAPINFOHEADER );
	i_Header.biWidth			= i_Width;
	i_Header.biHeight			= i_Height;
	i_Header.biPlanes			= 1;
	i_Header.biBitCount			= (unsigned short)i_DestBitsPerPixel;
	i_Header.biCompression		= BI_RGB;
	i_Header.biSizeImage		= i_DestImageSize;
	i_Header.biXPelsPerMeter	= i_Width;
	i_Header.biYPelsPerMeter	= i_Height;
	i_Header.biClrUsed			= 0;
	i_Header.biClrImportant		= 0;
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
	BITMAPINFOHEADER	bih;
	BITMAPFILEHEADER	bfh;

	bfh.bfType		= 0x4d42;
	bfh.bfSize		= sizeof( BITMAPFILEHEADER ) + sizeof( bih ) +
					  i_PaletteSize + i_DestImageSize;
	bfh.bfReserved1	= 0;
	bfh.bfReserved2	= 0;
	bfh.bfOffBits	= sizeof( BITMAPFILEHEADER ) + sizeof( bih ) +
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
						RECT*	i_pRect /*= NULL*/ )
{
	//--------------------------------------------------------------//
	// Get width and height from rect or surface
	//
	int Width	= (i_pRect) ? (i_pRect->right - i_pRect->left) : i_pSurface->GetWidth();
	int Height	= (i_pRect) ? (i_pRect->bottom - i_pRect->top) : i_pSurface->GetHeight();

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
		PALETTEENTRY TempColor;
		for ( int loop = 0; loop < 256; ++loop )
		{
			TempColor.peBlue = (envType::UInt8) loop;
			TempColor.peRed = (envType::UInt8) loop;
			TempColor.peGreen = (envType::UInt8) loop;
			i_File.Write( sizeof(PALETTEENTRY), &TempColor );
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
					RECT* i_pRect /*= NULL*/)
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