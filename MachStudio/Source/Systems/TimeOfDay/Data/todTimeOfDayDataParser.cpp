/********************************************************************************************\
**  todTimeOfDayDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "todTimeOfDayDataParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"

namespace todTimeOfDayDataParser
{

namespace
{
const chDefs::Name c_TIMEOFDAYD = chDefs::MakeName('T', 'O', 'D', 'D');

const int l_TODLIST_VERSION	= 0;
}

//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  GetChunkName()
{
	return c_TIMEOFDAYD;
}

//========================================================================
//   ReadTimeOfDayData
//========================================================================
void ReadTimeOfDayData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						todTimeOfDayData& o_Data )
{
	o_Data.m_bAutoRotation.Read(i_Reader);
	o_Data.m_bEnableAutoRotation.Read(i_Reader);
	o_Data.m_fLengthOfDay.Read(i_Reader);
	o_Data.m_fStartTimeOfDay.Read(i_Reader);
}

//========================================================================
//   WriteTimeOfDayData
//========================================================================
void WriteTimeOfDayData(	chWriter& o_Writer,
							const todTimeOfDayData& i_Data )
{

	o_Writer.WriteChunkHeader( c_TIMEOFDAYD, l_TODLIST_VERSION, false );

	i_Data.m_bAutoRotation.Write(o_Writer);
	i_Data.m_bEnableAutoRotation.Write(o_Writer);
	i_Data.m_fLengthOfDay.Write(o_Writer);
	i_Data.m_fStartTimeOfDay.Write(o_Writer);

	o_Writer.FinishChunk();
}

}	// end of namespace
