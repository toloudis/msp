/********************************************************************************************\
**  mnmBaseDataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Support/mnm/mnmBaseDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Support/tmln/tmlnParser.hpp"


//============================================================================
//============================================================================
namespace mnmBaseDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_BASE	= chDefs::MakeName('B', 'A', 'S', 'E');	// base data main chunk
const chDefs::Name c_BASD	= chDefs::MakeName('B', 'A', 'S', 'D');	// base data chunk
const chDefs::Name c_DRVS	= chDefs::MakeName('D', 'R', 'V', 'S');	// base drivers chunk


//------------------------------------------------------------------------
//   read_base_data
//------------------------------------------------------------------------
void read_base_data(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mnmBaseData& o_Data )
{
	nameUID uid;
	chChunkParserUtil::Read(i_Reader, uid);
	o_Data.m_Name.SetUID( uid );

	std::string name;
	chChunkParserUtil::Read(i_Reader, name);
	o_Data.m_Name.SetString( name );

	chChunkParserUtil::Read(i_Reader, o_Data.m_Filename);
	chChunkParserUtil::Read(i_Reader, o_Data.m_Position);
	chChunkParserUtil::Read(i_Reader, o_Data.m_Orientation);
	chChunkParserUtil::Read(i_Reader, o_Data.m_bEditorVisible);

	//DBG_LOG1("base-read filename-%s", itStringUtil::GetStdString(o_Data.m_Filename).c_str() );
	//DBG_LOG3("base-read pos(%6.3f,%6.3f,%6.3f)", o_Data.m_Position.GetX(), o_Data.m_Position.GetY(), o_Data.m_Position.GetZ() );
}

//------------------------------------------------------------------------
//   write_base_data
//------------------------------------------------------------------------
void write_base_data(chWriter& o_Writer,
					 const mnmBaseData& i_Data )
{
	// Write base info
	const int l_BASD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_BASD, l_BASD_VERSION, false );

	chChunkParserUtil::Write(o_Writer, i_Data.m_Name.GetUID() );
	chChunkParserUtil::Write(o_Writer, i_Data.m_Name.GetString() );
	chChunkParserUtil::Write(o_Writer, i_Data.m_Filename);
	chChunkParserUtil::Write(o_Writer, i_Data.m_Position);
	chChunkParserUtil::Write(o_Writer, i_Data.m_Orientation);
	chChunkParserUtil::Write(o_Writer, i_Data.m_bEditorVisible);

	o_Writer.FinishChunk();

	//DBG_LOG1("base-write filename-%s", itStringUtil::GetStdString(i_Data.m_Filename).c_str() );
	//DBG_LOG3("base-read pos(%6.3f,%6.3f,%6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );
}

}



//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_BASE;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Name i_SystemCode,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				mnmBaseData& o_Data )
{
	chDefs::Name	name;
	chDefs::Version version;
	chDefs::Size	size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == c_BASD )
		{
			read_base_data(i_Reader, version, size, o_Data);
		}
		else if ( name == c_DRVS )
		{
			// FIX: [rjk] this is temporary until all scene files are converted!!!
			tmlnParser::ReadDrivers(i_Reader, i_SystemCode, o_Data.m_Drivers);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const mnmBaseData& i_Data )
{
	const int l_BASE_VERSION = 0;
	o_Writer.WriteChunkHeader( c_BASE, l_BASE_VERSION, true );

	write_base_data(o_Writer, i_Data);

	o_Writer.FinishChunk();
}

}	// end of namespace

