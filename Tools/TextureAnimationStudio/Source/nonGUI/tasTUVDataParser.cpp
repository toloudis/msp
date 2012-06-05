/********************************************************************************************\
**  tasTUVDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "tasTUVDataParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileStream.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace tasTUVDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_TUVD = chDefs::MakeName('T', 'U', 'V', 'D');	// TUV data
const chDefs::Name c_FNML = chDefs::MakeName('F', 'N', 'M', 'L');	//	+-- Item List
const chDefs::Name c_FNFO = chDefs::MakeName('F', 'N', 'F', 'O');	//	  +-- item Info


//------------------------------------------------------------------------
//   ReadTUVData
//------------------------------------------------------------------------
void ReadTUVData(	chReader& io_Reader,
					tasTUVDataItem& o_Data )
{
	std::string savefilename;
	chChunkParserUtil::Read( io_Reader, savefilename );
	o_Data.m_Filename = savefilename;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadTUVListData( chReader& io_Reader,
						tasTUVData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	int i = 0;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_FNML )
		{
			int entries;
			chChunkParserUtil::Read( io_Reader, entries );
			o_Data.m_Filenames.resize( entries );

			//DBG_LOG1( "Reading %d entries", entries );

			while( io_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_FNFO )
				{
					//DBG_LOG1( "TUV list item %d", i );

					ReadTUVData( io_Reader, o_Data.m_Filenames[i] );
					i++;
				}
				io_Reader.FinishChunk();
			}
		}
		io_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteTUVData
//------------------------------------------------------------------------
void WriteTUVData(	chWriter& o_Writer,
						const tasTUVDataItem& i_Data )
{
	const int l_cFNFO_VERSION = 0;
	o_Writer.WriteChunkHeader( c_FNFO, l_cFNFO_VERSION, false );

	chChunkParserUtil::Write( o_Writer, i_Data.m_Filename );

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteTUVListData( chWriter& io_Writer,
						const tasTUVData& i_Data )
{
	//	Write List
	const int l_cFNML_VERSION = 0;
	io_Writer.WriteChunkHeader( c_FNML, l_cFNML_VERSION, true );

	chChunkParserUtil::Write( io_Writer, i_Data.m_Filenames.size() );

	DBG_LOG1( "Writing %d items", i_Data.m_Filenames.size() );

	//
	for (int i=0; i<i_Data.m_Filenames.size(); i++)
	{
		WriteTUVData(io_Writer, i_Data.m_Filenames[i]);
	}

	io_Writer.FinishChunk();
}

}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_TUVD;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				tasTUVData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	//DBG_LOG0( "Reading the data" );

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		//DBG_LOG2( "reading chunk %x %x", name, c_TUVD );

		if ( name == c_TUVD )
		{
			//DBG_LOG0( "TUV data" );

			ReadTUVListData( i_Reader, o_Data );
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const tasTUVData& i_Data )
{
	const int l_cTUVD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_TUVD, l_cTUVD_VERSION, true );

	WriteTUVListData( o_Writer, i_Data );

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Read(const fsLocator& i_Locator, tasTUVData& o_Data)
{
	std::auto_ptr<gfFileBin> data_file(new gfFileBin(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian));
	gfFileBin::Header header;
	data_file->ReadHeader(&header);

	chBinReader reader(*data_file);
	tasTUVDataParser::ReadData(reader, o_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Write(const fsLocator& i_Locator, const tasTUVData& i_Data)
{
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);

	fsFileUtil::CreateFile(i_Locator);

	gfFileBin data_file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	const int c_Major = 1; const int c_Minor = 0; const int c_Rev = 0;
	data_file.WriteHeader(c_Major,c_Minor,c_Rev);

	chBinWriter writer(data_file);
	tasTUVDataParser::WriteData(writer, i_Data);
}

}	// end of namespace

