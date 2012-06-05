/********************************************************************************************\
**  tasUVADataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#include "tasUVADataParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
//#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace tasUVADataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_UVAD = chDefs::MakeName('U', 'V', 'A', 'D');	// UVA data
const chDefs::Name c_ANFO = chDefs::MakeName('A', 'N', 'F', 'O');	//  +-- Animation Info
const chDefs::Name c_TLST = chDefs::MakeName('T', 'L', 'S', 'T');	//	+-- Texture Page List
const chDefs::Name c_TNFO = chDefs::MakeName('T', 'N', 'F', 'O');	//	  +-- Texture Page Info


//------------------------------------------------------------------------
//   ReadTexturePageData
//------------------------------------------------------------------------
void ReadTexturePageData(	chReader& io_Reader,
							tasUVADataItem& o_Data )
{
	fsLocator filename;
	std::string savefilename;
	chChunkParserUtil::Read( io_Reader, savefilename );
	o_Data.m_Filename.Clear();
	fsFileUtil::ANSIFilenameToLocator( savefilename, filename );
	o_Data.m_Filename = filename;

	chChunkParserUtil::Read( io_Reader, o_Data.m_FramesPerPage );
	int flsize; 
	chChunkParserUtil::Read( io_Reader, flsize );
	o_Data.m_FrameLocs.resize(flsize);

	for (int i=0; i < o_Data.m_FrameLocs.size(); ++i)
	{
		chChunkParserUtil::Read( io_Reader, o_Data.m_FrameLocs[i] );
	}
}

//------------------------------------------------------------------------
//   WriteTexturePageData
//------------------------------------------------------------------------
void WriteTexturePageData(	chWriter& o_Writer,
							const tasUVADataItem& i_Data )
{
	const int l_cTNFO_VERSION = 0;
	o_Writer.WriteChunkHeader( c_TNFO, l_cTNFO_VERSION, false );

	std::string savefilename;
	fsFileUtil::LocatorToANSIFilename( i_Data.m_Filename, savefilename );
	chChunkParserUtil::Write( o_Writer, savefilename );

	chChunkParserUtil::Write( o_Writer, i_Data.m_FramesPerPage );
	chChunkParserUtil::Write( o_Writer, i_Data.m_FrameLocs.size() );

	for (int i=0; i < i_Data.m_FrameLocs.size(); ++i)
	{
		chChunkParserUtil::Write( o_Writer, i_Data.m_FrameLocs[i] );
	}

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadUVAInfoData(	chReader& io_Reader,
						tasUVAData& o_Data )
{
	//	Read List
	chChunkParserUtil::Read( io_Reader, o_Data.m_NumberOfFrames );
	chChunkParserUtil::Read( io_Reader, o_Data.m_NumberOfTexturePages );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteUVAInfoData(	chWriter& io_Writer,
						const tasUVAData& i_Data )
{
	//	Write List
	const int l_cANFO_VERSION = 0;
	io_Writer.WriteChunkHeader( c_ANFO, l_cANFO_VERSION, true );

	chChunkParserUtil::Write( io_Writer, i_Data.m_NumberOfFrames );
	chChunkParserUtil::Write( io_Writer, i_Data.m_NumberOfTexturePages );

	io_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadUVAListData(	chReader& io_Reader,
						tasUVAData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	int i = 0;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_TLST )
		{
			int entries;
			chChunkParserUtil::Read( io_Reader, entries );
			o_Data.m_TexturePages.resize( entries );

			//DBG_LOG1( "Reading %d entries", entries );

			while( io_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_TNFO )
				{
					//DBG_LOG1( "UVA list item %d", i );

					ReadTexturePageData( io_Reader, o_Data.m_TexturePages[i] );
					i++;
				}
				io_Reader.FinishChunk();
			}
		}
		io_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteUVAListData(	chWriter& io_Writer,
						const tasUVAData& i_Data )
{
	//	Write List
	const int l_cTLST_VERSION = 0;
	io_Writer.WriteChunkHeader( c_TLST, l_cTLST_VERSION, true );

	chChunkParserUtil::Write( io_Writer, i_Data.m_TexturePages.size() );

	DBG_LOG1( "Writing %d items", i_Data.m_TexturePages.size() );

	//
	for (int i=0; i<i_Data.m_TexturePages.size(); i++)
	{
		WriteTexturePageData(io_Writer, i_Data.m_TexturePages[i]);
	}

	io_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				tasUVAData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	//DBG_LOG0( "Reading the data" );
	i_Reader.ReadChunkHeader(name, version, size);
	//DBG_LOG2( "reading chunk %x %x", name, c_UVAD );

	if ( name == c_UVAD )
	{
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_TLST)
			{
				ReadUVAListData( i_Reader, o_Data );
			}
			else if (name == c_ANFO)
			{
				ReadUVAInfoData( i_Reader, o_Data );
			}
		}
	}
	i_Reader.FinishChunk();
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const tasUVAData& i_Data )
{
	const int l_cUVAD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_UVAD, l_cUVAD_VERSION, true );

	WriteUVAInfoData( o_Writer, i_Data );
	WriteUVAListData( o_Writer, i_Data );

	o_Writer.FinishChunk();
}

}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_UVAD;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Read(const fsLocator& i_Locator, tasUVAData& o_Data)
{
	std::auto_ptr<gfFileBin> data_file(new gfFileBin(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian));
	gfFileBin::Header header;
	data_file->ReadHeader(&header);

	chBinReader reader(*data_file);

	tasUVADataParser::ReadData(reader, o_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Write(const fsLocator& i_Locator, const tasUVAData& i_Data)
{
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);

	fsFileUtil::CreateFile(i_Locator);

	gfFileBin data_file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	const int c_Major = 1; const int c_Minor = 0; const int c_Rev = 0;
	data_file.WriteHeader(c_Major,c_Minor,c_Rev);
	chBinWriter writer(data_file);

	tasUVADataParser::WriteData(writer, i_Data);
}

}	// end of namespace

