/********************************************************************************************\
**  tasUVADataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TAS_UVADATAPARSER_HPP
#error tasUVADataParser.hpp multiply included
#endif
#define TAS_UVADATAPARSER_HPP

#ifndef TAS_UVADATA_HPP
#include "tasUVAData.hpp"
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
namespace tasUVADataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Read(const fsLocator& i_Locator, tasUVAData& o_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Write(const fsLocator& i_Locator, const tasUVAData& i_Data);
}

