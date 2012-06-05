/********************************************************************************************\
**  cmmTimeDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef CMM_TIMEDATAPARSER_HPP
#error cmmTimeDataParser.hpp multiply included
#endif
#define CMM_TIMEDATAPARSER_HPP

#ifndef CMM_TIMEDATA_HPP
#include "Systems/Common/TimeData/cmmTimeData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

namespace cmmTimeDataParser
{
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	chDefs::Name  GetChunkName();

	//========================================================================
	//   ReadData
	//========================================================================
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					cmmTimeData& o_Data );

	//========================================================================
	//   WriteData
	//========================================================================
	void WriteData(	chWriter& o_Writer,
					const cmmTimeData& i_Data );


}

