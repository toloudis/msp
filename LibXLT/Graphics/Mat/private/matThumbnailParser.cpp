/*****************************************************************************
**	matThumbnailParser.cpp
**
**		matThumbnailParser handles the creation of raw image data types
**
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/Mat/matThumbnailParser.hpp"

#include "Graphics/g2d/g2dPFD.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include <wtypes.h>

namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//const chDefs::Name c_REFL = chDefs::MakeName('T', 'H', 'M', 'B');
	const int l_BitmapSize = 128;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void matThumbnailParser::Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						envType::UInt8* &o_PixelBuffer, 
						int &o_BufferSize,
						int &o_BitmapSize) 
{
	int image_offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	int image_size = i_Size - image_offset;

	char* header_info = new char[image_offset];
	o_PixelBuffer = new envType::UInt8[image_size];
	i_Reader.Read(header_info, image_offset);
	i_Reader.Read(o_PixelBuffer, image_size);
	delete [] header_info;
	
	o_BufferSize = image_size;
	o_BitmapSize = l_BitmapSize;
}


//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_PixelBuffer is the object being written.
//----------------------------------------------------------------------------
void matThumbnailParser::Write( chWriter& o_Writer,
						envType::UInt8* i_PixelBuffer, int i_BufferSize )
{
	//static const chDefs::Version s_Version = 0;
	//write chunk header
	//o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	//version 0
	// write BitmapFileHeader
	int width = l_BitmapSize;
	int height = l_BitmapSize;
	int bitsPerPixel = 24;

	BITMAPINFOHEADER bmpInfoHeader = {0};
    // Set the size
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Bit count
    bmpInfoHeader.biBitCount = bitsPerPixel;
    // Use all colors
    bmpInfoHeader.biClrImportant = 0;
    // Use as many colors according to bits per pixel
    bmpInfoHeader.biClrUsed = 0;
    // Store as un Compressed
    bmpInfoHeader.biCompression = BI_RGB;
    // Set the height in pixels
    bmpInfoHeader.biHeight = height;
    // Width of the Image in pixels
    bmpInfoHeader.biWidth = width;
    // Default number of planes
    bmpInfoHeader.biPlanes = 1;
    // Calculate the image size in bytes
    bmpInfoHeader.biSizeImage = width* height * (bitsPerPixel/8);

	BITMAPFILEHEADER bfh = {0};
    // This value should be values of BM letters i.e 0×4D42
    // 0x4D = M 0x42 = B storing in reverse order to match with endian
    bfh.bfType = 0x4D42;

    // Offset to the RGBQUAD
    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    // Total size of image including size of headers
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;

	//write bitmap header
	o_Writer.Write( &bfh, sizeof( bfh ) );

	//write bitmap info
	o_Writer.Write( &bmpInfoHeader, sizeof( bmpInfoHeader ) );

	//write image data
	o_Writer.Write(i_PixelBuffer, i_BufferSize);

	//o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
//envType::UInt8* matThumbnailParser::Create() const
//{
//	envType::UInt8* pBuffer = new envType::UInt8();
//	return pBuffer;
//}

//--------------------------------------------------------------------
// Return the chunk name used for thumbnails
//--------------------------------------------------------------------
//chDefs::Name matThumbnailParser::GetChunkName()
//{
//	return c_REFL;
//}
