/********************************************************************************************\
**  setsDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef SETS_DATAPARSER_HPP
#error setsDataParser.hpp multiply included
#endif
#define SETS_DATAPARSER_HPP

#ifndef SETS_SCRIPTDATA_HPP
#include "Systems/Sets/Data/setsScriptData.hpp"
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
namespace setsDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadSetItem
	//------------------------------------------------------------------------
	void ReadSetItem(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						setsScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteSetItem
	//------------------------------------------------------------------------
	void WriteSetItem(	chWriter& o_Writer,
						const setsScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					setsListData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const setsListData& i_Data );
}

