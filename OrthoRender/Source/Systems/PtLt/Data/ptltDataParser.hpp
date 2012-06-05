/********************************************************************************************\
**  ptltDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef PTLT_DATAPARSER_HPP
#error ptltDataParser.hpp multiply included
#endif
#define PTLT_DATAPARSER_HPP

#ifndef PTLT_SCRIPTDATA_HPP
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#endif
#ifndef PTLT_DATA_HPP
#include "Systems/PtLt/Data/ptltData.hpp"
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
class ptltDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadPointLightData
	//------------------------------------------------------------------------
	static void ReadPointLightData(chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									ptltScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WritePointLightData
	//------------------------------------------------------------------------
	static void WritePointLightData(chWriter& o_Writer,
									const ptltScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							ptltPointLightsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const ptltPointLightsData& i_Data );
};

