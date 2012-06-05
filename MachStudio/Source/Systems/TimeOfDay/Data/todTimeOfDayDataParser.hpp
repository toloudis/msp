/********************************************************************************************\
**  todTimeOfDayDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef TOD_TIMEOFDAYDATAPARSER_HPP
#error todTimeOfDayDataParser.hpp multiply included
#endif
#define TOD_TIMEOFDAYDATAPARSER_HPP

#ifndef TOD_TIMEOFDAYDATA_HPP
#include "todTimeOfDayData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

namespace todTimeOfDayDataParser
{
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	chDefs::Name  GetChunkName();

	//========================================================================
	//   ReadTimeOfDayData
	//========================================================================
	void ReadTimeOfDayData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					todTimeOfDayData& o_Data );

	//========================================================================
	//   WriteTimeOfDayData
	//========================================================================
	void WriteTimeOfDayData(	chWriter& o_Writer,
					const todTimeOfDayData& i_Data );


}

