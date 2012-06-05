/********************************************************************************************\
**  dirltDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef DIRLT_DATAPARSER_HPP
#error dirltDataParser.hpp multiply included
#endif
#define DIRLT_DATAPARSER_HPP

#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
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
class dirltDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadDirLightData
	//------------------------------------------------------------------------
	static void ReadDirLightData(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									dirltScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteDirLightData
	//------------------------------------------------------------------------
	static void WriteDirLightData(	chWriter& o_Writer,
									const dirltScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							dirltDirLightsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const dirltDirLightsData& i_Data );
};


