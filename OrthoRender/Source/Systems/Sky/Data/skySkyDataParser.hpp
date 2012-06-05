/********************************************************************************************\
**  skySkyDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef SKY_SKYDATAPARSER_HPP
#error skySkyDataParser.hpp multiply included
#endif
#define SKY_SKYDATAPARSER_HPP

#ifndef DAY_SKYDATA_HPP
#include "daySkyData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

namespace skySkyDataParser
{
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	chDefs::Name  GetChunkName();

	//========================================================================
	//   ReadSkyData
	//========================================================================
	void ReadSkyData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						daySkyLayerList& o_Data );

	//========================================================================
	//   WriteSkyData
	//========================================================================
	void WriteSkyData(	chWriter& o_Writer,
						const daySkyLayerList& i_Data );


}

