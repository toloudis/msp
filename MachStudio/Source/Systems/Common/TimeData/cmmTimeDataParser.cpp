/********************************************************************************************\
**  cmmTimeDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Common/TimeData/cmmTimeDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Support/tmln/tmlnParser.hpp"


//============================================================================
//============================================================================
namespace cmmTimeDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_CTDP = chDefs::MakeName('C', 'T', 'D', 'P');


}	// end of namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_CTDP;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				cmmTimeData& o_Data )
{
	//TIME - reading time in seconds to preserve old file format
	envType::Float32 time;
	chChunkParserUtil::Read(i_Reader, time);
	o_Data.m_MaximumTime.SetSeconds( time );
	if (i_Version >= 1)
	{
		chChunkParserUtil::Read(i_Reader, time);
		o_Data.m_MinimumTime.SetSeconds( time );
	}

}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const cmmTimeData& i_Data )
{

	// Version 1 adds minimum time
	o_Writer.WriteChunkHeader( c_CTDP, 1, false );

	//TIME - reading time in seconds to preserve old file format
	envType::Float32 time = i_Data.m_MaximumTime.AsSeconds();
	chChunkParserUtil::Write(o_Writer, time);

	time = i_Data.m_MinimumTime.AsSeconds();
	chChunkParserUtil::Write(o_Writer, time);

	o_Writer.FinishChunk();
}

}	// end of namespace

