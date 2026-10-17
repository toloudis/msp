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

namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//const chDefs::Name c_REFL = chDefs::MakeName('T', 'H', 'M', 'B');
	const int l_BitmapSize = 128;

	// The pixels are preceded by 54 bytes that used to hold a BMP file
	// header.  They are ignored, and kept so the chunk layout doesn't change.
	const int c_ReservedHeaderSize = 54;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void matThumbnailParser::Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						uint8_t* &o_PixelBuffer, 
						int &o_BufferSize,
						int &o_BitmapSize) 
{
	int image_offset = c_ReservedHeaderSize;
	int image_size = i_Size - image_offset;

	char* header_info = new char[image_offset];
	o_PixelBuffer = new uint8_t[image_size];
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
						uint8_t* i_PixelBuffer, int i_BufferSize )
{
	//static const chDefs::Version s_Version = 0;
	//write chunk header
	//o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	//write the reserved header bytes
	const uint8_t reserved[c_ReservedHeaderSize] = {0};
	o_Writer.Write( reserved, sizeof( reserved ) );

	//write image data
	o_Writer.Write(i_PixelBuffer, i_BufferSize);

	//o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
//uint8_t* matThumbnailParser::Create() const
//{
//	uint8_t* pBuffer = new uint8_t();
//	return pBuffer;
//}

//--------------------------------------------------------------------
// Return the chunk name used for thumbnails
//--------------------------------------------------------------------
//chDefs::Name matThumbnailParser::GetChunkName()
//{
//	return c_REFL;
//}
