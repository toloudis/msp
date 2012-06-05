/********************************************************************************************\
**  lsetLightSetsDataParser.hpp
**
**		LightSets data parsing
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef LSET_LIGHTSETSDATAPARSER_HPP
#error lsetLightSetDataParser.hpp multiply included
#endif
#define LSET_LIGHTSETSDATAPARSER_HPP

#ifndef LSET_SCRIPTDATA_HPP
#include "Systems/LightSets/Data/lsetScriptData.hpp"
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
class lsetLightSetsDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadLightSetData
	//------------------------------------------------------------------------
	static void ReadLightSetData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							lsetScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteLightSetData
	//------------------------------------------------------------------------
	static void WriteLightSetData(	chWriter& o_Writer,
							const lsetScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					lsetLightSetsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const lsetLightSetsData& i_Data );

};

