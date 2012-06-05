/********************************************************************************************\
**  chnlTimeDataParser.hpp
**
**		Parse the Time data.
**
**  Extra Large Technology
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#ifdef CHNL_TIMEDATAPARSER_HPP
#error chnlTimeDataParser.hpp multiply included
#endif
#define CHNL_TIMEDATAPARSER_HPP

#ifndef CHNL_TIMEDATA_HPP
#include "Features/Channels/Data/chnlTimeData.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace chnlTimeDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chnlTimeData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const chnlTimeData& i_Data );

}

