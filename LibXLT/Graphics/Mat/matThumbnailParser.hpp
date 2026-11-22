/*****************************************************************************
**	effReflDataParser.hpp
**
**		effReflDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_THUMBNAILPARSER_HPP
#error matThumbnailParser.hpp multiply included
#endif
#define MAT_THUMBNAILPARSER_HPP

#ifndef MAT_SHADERPARSER_HPP
#include "Graphics/mat/matShaderParser.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

//============================================================================
//	forward references
//============================================================================
struct g2dPixelR8G8B8;
class chReader;
class chWriter;

//============================================================================
//============================================================================
class matThumbnailParser
{
public:
	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	static void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						uint8_t* &o_PixelBuffer, 
						int &o_BufferSize,
						int &o_BitmapSize);

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Shader is the object being written.
	//--------------------------------------------------------------------
	static void Write( chWriter& i_Writer,
						uint8_t* i_PixelBuffer, int i_BufferSize );

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	//virtual uint8_t* Create() const;

	//--------------------------------------------------------------------
	// Return the chunk name used for thumbnails
	//--------------------------------------------------------------------
	//static chDefs::Name GetChunkName();
};
