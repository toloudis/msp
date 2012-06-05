/********************************************************************************************\
**  skySkyDataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "skySkyDataParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"

#include "daySkyDataParser.hpp"


namespace skySkyDataParser
{

namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_SKYD = chDefs::MakeName('S', 'K', 'Y', 'D');
}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_SKYD;
}

//------------------------------------------------------------------------
//   ReadSkyData
//------------------------------------------------------------------------
void ReadSkyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					daySkyLayerList& o_Data )
{
	// read in the day package sky chunk
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	i_Reader.ReadChunkHeader(name, version, size);

	// Info divided into base chunk and driver list
	if ( name == daySkyDataParser::GetChunkName() )
	{
		daySkyDataParser::Read( i_Reader, i_Version, i_Size, o_Data );

		i_Reader.FinishChunk();

		//DBG_LOG1( "read camera (%s)", o_Data.m_Name.c_str() );
	}
	else
	{
		DBG_ERROR0( "invalid chunk while reading sky data" );
	}
}

//------------------------------------------------------------------------
//   WriteSkyData
//------------------------------------------------------------------------
void WriteSkyData(	chWriter& o_Writer,
					const daySkyLayerList& i_Data )
{
	o_Writer.WriteChunkHeader( c_SKYD, 0, false );

	daySkyDataParser::Write( o_Writer, i_Data );

	o_Writer.FinishChunk();
}


}	// end of namespace

