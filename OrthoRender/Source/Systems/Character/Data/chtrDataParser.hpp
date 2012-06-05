/********************************************************************************************\
**  chtrDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef CHTR_DATAPARSER_HPP
#error chtrDataParser.hpp multiply included
#endif
#define CHTR_DATAPARSER_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
class chtrDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadCharacterChunk
	//------------------------------------------------------------------------
	static void ReadCharacterChunk(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									chtrScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteCharacterChunk
	//------------------------------------------------------------------------
	static void WriteCharacterChunk(chWriter& o_Writer,
									const chtrScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							chtrCharactersData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const chtrCharactersData& i_Data );
};


