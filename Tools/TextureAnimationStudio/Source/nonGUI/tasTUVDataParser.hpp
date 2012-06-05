/********************************************************************************************\
**  tasTUVDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TAS_TUVDATAPARSER_HPP
#error tasTUVDataParser.hpp multiply included
#endif
#define TAS_TUVDATAPARSER_HPP

#ifndef TAS_TUVDATA_HPP
#include "tasTUVData.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "chDefs.hpp"
#endif


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace tasTUVDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Read(const fsLocator& i_Locator, tasTUVData& o_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Write(const fsLocator& i_Locator, const tasTUVData& i_Data);
}

