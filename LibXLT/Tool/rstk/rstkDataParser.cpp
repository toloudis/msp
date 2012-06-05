/********************************************************************************************\
**	rstkDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Tool/rstk/rstkDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

#include <iomanip>


//============================================================================
//============================================================================
namespace rstkDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_RTRK = chDefs::MakeName('R', 'T', 'R', 'K');	// main data
const chDefs::Name c_RESL = chDefs::MakeName('R', 'E', 'S', 'L');	// +-- resource list data
const chDefs::Name c_RESD = chDefs::MakeName('R', 'E', 'S', 'D');	// +-- resource data


//------------------------------------------------------------------------
//   ReadResourceData
//------------------------------------------------------------------------
void ReadResourceData(	chReader& io_Reader,
						chDefs::Version i_Version,
						fsResourceTrackerFileData& o_Data )
{
	fsLocator filepath;
	std::string filename;
	chChunkParserUtil::Read( io_Reader, filename );
	fsFileUtil::ANSIFilenameToLocator( filename, filepath );
	o_Data.SetFilePath(filepath);
	int depth;
	chChunkParserUtil::Read( io_Reader, depth );
	o_Data.SetDepth(depth);
}

//------------------------------------------------------------------------
//   WriteResourceData
//------------------------------------------------------------------------
void WriteResourceData(	chWriter& o_Writer,
						const fsResourceTrackerFileData& i_Data )
{
	const int l_cRESD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_RESD, l_cRESD_VERSION, false );

	std::string filename;
	fsFileUtil::LocatorToANSIFilename( i_Data.GetFilePath(), filename );
	chChunkParserUtil::Write( o_Writer, filename );
	chChunkParserUtil::Write( o_Writer, i_Data.GetDepth() );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadResourceListData
//------------------------------------------------------------------------
void ReadResourceListData(	chReader& io_Reader,
							chDefs::Version i_Version,
							fsResourceTrackerData& o_Data )
{
	int num_resources;
	chChunkParserUtil::Read( io_Reader, num_resources );
	o_Data.m_Resources.resize(num_resources);
}

//------------------------------------------------------------------------
//   WriteResourceListData
//------------------------------------------------------------------------
void WriteResourceListData(	chWriter& o_Writer,
							const fsResourceTrackerData& i_Data )
{
	const int l_cRESL_VERSION = 0;
	o_Writer.WriteChunkHeader( c_RESL, l_cRESL_VERSION, false );

	int num_resources = i_Data.m_Resources.size();
	chChunkParserUtil::Write( o_Writer, num_resources );

	o_Writer.FinishChunk();
}

}	// local namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_RTRK;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				fsResourceTrackerData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	int i = 0;
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_RESD )
		{
			ReadResourceData( i_Reader, version, o_Data.m_Resources[i] );
			++i;
		}
		else if ( name == c_RESL )
		{
			ReadResourceListData( i_Reader, version, o_Data );
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const fsResourceTrackerData& i_Data )
{
	const int l_cRTRK_VERSION = 1;
	o_Writer.WriteChunkHeader( c_RTRK, l_cRTRK_VERSION, true );

	WriteResourceListData(o_Writer, i_Data);

	DBG_TRACE("---Writing Resource Data---");
	for (int i = 0; i < i_Data.m_Resources.size(); ++i)
	{
		//	DEBUG
		std::string filestr;
		fsFileUtil::LocatorToANSIFilename( i_Data.m_Resources[i].GetFilePath(), filestr );
		//DBG_LOG2("%02d. %s", i, filestr.c_str());
		DBG_TRACE(std::setw(2) << i << " " << filestr.c_str());

		//	write out one resource
		WriteResourceData( o_Writer, i_Data.m_Resources[i] );
	}

	o_Writer.FinishChunk();
}

}	// end of namespace

