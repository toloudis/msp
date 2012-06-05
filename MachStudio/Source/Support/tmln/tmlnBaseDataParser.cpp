/********************************************************************************************\
**  tmlnBaseDataParser.cpp
**
**  TODO: phase out this class/file
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Support/tmln/tmlnBaseDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Support/tmln/tmlnParser.hpp"


//============================================================================
//============================================================================
namespace tmlnBaseDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_TBAS	= chDefs::MakeName('T', 'B', 'A', 'S');	// base data main chunk
const chDefs::Name c_DRVS	= chDefs::MakeName('D', 'R', 'V', 'S');	// base drivers chunk


//------------------------------------------------------------------------
//   write_base_data
//------------------------------------------------------------------------
void write_base_data(chWriter& o_Writer,
				     chDefs::Name i_SystemCode,
					 const tmlnBaseData& i_Data )
{
	// Write drivers' info
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );

	tmlnParser::WriteDrivers(o_Writer, i_SystemCode, i_Data.m_Drivers);

	o_Writer.FinishChunk();
}

}



//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_TBAS;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Name i_SystemCode,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				tmlnBaseData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, i_SystemCode, o_Data.m_Drivers);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				chDefs::Name i_SystemCode,
				const tmlnBaseData& i_Data )
{
	const int l_BASE_VERSION = 0;
	o_Writer.WriteChunkHeader( c_TBAS, l_BASE_VERSION, true );

	write_base_data(o_Writer, i_SystemCode, i_Data);

	o_Writer.FinishChunk();
}

}	// end of namespace

