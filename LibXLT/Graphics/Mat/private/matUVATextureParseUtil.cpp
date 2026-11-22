/****************************************************************************\
**	matUVATextureParseUtil.cpp
**
**		matUVATextureParseUtil is a namespace that reads and writes .tuv files
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matUVATextureParseUtil.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsLocator.hpp"


//============================================================================
//	UVA Texture file format
//
//	The entire UVA (UV Anim) texture specification is contained in a
//	TUVA chunk.  This chunk could be located in a stand-alone file
//	or as part of another file.
//
//	The TUVA (Terawatt UV Anim) chunk can contain two other chunk
//	types:
//
//		UDAT	(Uva DATa)
//				short num_frames (total number of frames in the UVA)
//				short num_width_frames (number of frames across a page)
//				short num_height_frames (number of frames down a page)
//				float frame_rate	(frames per second)
//				char looping (1 = looping, 0 = once through)
//				char reversing (1 = reversing, 0 = no reverse)
//		TPAG	(Texture PAGe)
//				null terminated string texture_name
//					(textures are usually assumed to be in the same
//					directory or pak as the uva)
//
//
//	The TUVA chunk can contain one UDAT and one or more TPAG chunks.
//	The subchunks can be in any order, except that the TPAG chunks
//	should appear in the order that they should be in the UVA.
//
//
//============================================================================
namespace matUVATextureParseUtil
{

namespace
{
	//--------------------------------------------------------------------
	// PARSING DATA AND METHODS
	//--------------------------------------------------------------------
	const chDefs::Name c_TUVA = chDefs::MakeName('T', 'U', 'V', 'A');
	const chDefs::Name c_UDAT = chDefs::MakeName('U', 'D', 'A', 'T');
	const chDefs::Name c_TPAG = chDefs::MakeName('T', 'P', 'A', 'G');

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void read_UDAT(	chReader& i_Reader,
					matUVATexture& o_Texture,
					chDefs::Version i_Version,
					chDefs::Size i_Size)
	{
		uint32_t num_frames;
		uint32_t num_width_frames;
		uint32_t num_height_frames;
		float frame_rate;
		uint8_t looping;
		uint8_t reversing;

		i_Reader.Read(num_frames);
		i_Reader.Read(num_width_frames);
		i_Reader.Read(num_height_frames);
		i_Reader.Read(frame_rate);
		i_Reader.Read(looping);
		i_Reader.Read(reversing);

		o_Texture.SetNumFrames(num_frames);
		o_Texture.SetNumWidthFrames(num_width_frames);
		o_Texture.SetNumHeightFrames(num_height_frames);
		o_Texture.SetFrameRate(frame_rate);
		o_Texture.SetLooping((looping == 1) ? true : false);
		o_Texture.SetReversing((reversing == 1) ? true : false);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void read_TPAG(	chReader& i_Reader,
					matUVATexture& o_Texture,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<std::string>& o_Names)
	{
		std::string texture_filename;

		i_Reader.Read(texture_filename);
		o_Names.push_back(texture_filename);
	}

	void write_UDAT(chWriter& o_Writer, const matUVATexture& i_Texture)
	{
		o_Writer.WriteChunkHeader(c_UDAT, 0, false);

		uint32_t num_frames = i_Texture.GetNumFrames();
		uint32_t num_width_frames = i_Texture.GetNumWidthFrames();
		uint32_t num_height_frames = i_Texture.GetNumHeightFrames();
		float frame_rate = i_Texture.GetFrameRate();
		uint8_t looping = i_Texture.GetLooping();
		uint8_t reversing = i_Texture.GetReversing();

		o_Writer.Write(num_frames);
		o_Writer.Write(num_width_frames);
		o_Writer.Write(num_height_frames);
		o_Writer.Write(frame_rate);
		o_Writer.Write(looping);
		o_Writer.Write(reversing);

		o_Writer.FinishChunk();
	}

	void write_TPAG(chWriter& o_Writer, const std::string* i_Textures, int i_Num)
	{
		o_Writer.WriteChunkHeader(c_TPAG, 0, false);

		//	We must make a single-byte string out of the itString
		//	This only works for ANSI strings
		//
		const std::string& it_string = i_Textures[i_Num];
		std::string single_byte_string;

		int i;
		int num = it_string.size();
		for( i = 0 ; i < num ; i++ )
			single_byte_string += char(it_string.c_str()[i]);

		o_Writer.Write(single_byte_string.c_str());

		o_Writer.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	ReadUVATexture reads the given UVA using the given chReader.
//--------------------------------------------------------------------
matUVATexture* ReadUVATexture(	chReader& i_Reader,
								std::vector<std::string>& o_Names)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	i_Reader.ReadChunkHeader(name, version, size);

	DBG_ASSERT(name == c_TUVA, "Expected TUVA chunk");
	if( name != c_TUVA )
		throw chInvalidChunkX();

	matUVATexture* uva_texture = new matUVATexture;

	try
	{
		while (i_Reader.ReadChunkHeader(name, version, size))
		{
			if( name == c_UDAT )
				read_UDAT(i_Reader, *uva_texture, version, size);
			else if( name == c_TPAG )
				read_TPAG(i_Reader, *uva_texture, version, size, o_Names);

			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX&)
	{
		DBG_ERROR("Invalid chunk in matTextureMgr.cpp - read_TUVA");
		delete uva_texture;
		uva_texture = NULL;
		throw;
	}
	catch( ... )
	{
		DBG_ERROR("Unknown exception in TUV");
		delete uva_texture;
		uva_texture = NULL;
		throw;
	}


	return uva_texture;
}

//--------------------------------------------------------------------
//	WriteUVATexture writes the given UVA using the given chWriter.
//--------------------------------------------------------------------
void WriteUVATexture(chWriter& i_Writer, const matUVATexture& i_Texture, const std::string* i_Textures)
{
	i_Writer.WriteChunkHeader(c_TUVA, 0, true);

	write_UDAT(i_Writer, i_Texture);

	int i;
	int num = i_Texture.GetNumPages();
	for( i = 0 ; i < num ; i++ )
		write_TPAG(i_Writer, i_Textures, i);

	i_Writer.FinishChunk();
}

}