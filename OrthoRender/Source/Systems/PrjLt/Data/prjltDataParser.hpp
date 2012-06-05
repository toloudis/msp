/********************************************************************************************\
**  prjltDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRJLT_DATAPARSER_HPP
#error prjltDataParser.hpp multiply included
#endif
#define PRJLT_DATAPARSER_HPP

#ifndef PRJLT_SCRIPTDATA_HPP
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
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
class prjltDataParser
{
public:
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	static chDefs::Name  GetChunkName();

	//========================================================================
	//   ReadProjectedLightData
	//========================================================================
	static void ReadProjectedLightData(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										prjltScriptData& o_Data );

	//========================================================================
	//   WriteProjectedLightData
	//========================================================================
	static void WriteProjectedLightData(chWriter& o_Writer,
										const prjltScriptData& i_Data );

	//========================================================================
	//   ReadData
	//========================================================================
	static void ReadData(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						prjltProjectLightsData& o_Data );

	//========================================================================
	//   WriteData
	//========================================================================
	static void WriteData(	chWriter& o_Writer,
							const prjltProjectLightsData& i_Data );


};


