/********************************************************************************************\
**  propDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PROP_DATAPARSER_HPP
#error propDataParser.hpp multiply included
#endif
#define PROP_DATAPARSER_HPP

#ifndef PROP_SCRIPTDATA_HPP
#include "Systems/Props/Data/propScriptData.hpp"
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
class propDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadPropChunk
	//------------------------------------------------------------------------
	static void ReadPropChunk(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								propScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WritePropChunk
	//------------------------------------------------------------------------
	static void WritePropChunk(	chWriter& o_Writer,
								const propScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							propPropsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const propPropsData& i_Data );

};

