/********************************************************************************************\
**  lyrsLayersDataParser.hpp
**
**		Layers data parsing
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef LYRS_LAYERSDATAPARSER_HPP
#error lyrsLayerDataParser.hpp multiply included
#endif
#define LYRS_LAYERSDATAPARSER_HPP

#ifndef LYRS_DATA_HPP
#include "Systems/Layers/Data/lyrsData.hpp"
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
class lyrsLayersDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadLayerData
	//------------------------------------------------------------------------
	static void ReadLayerData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							lyrsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteLayerData
	//------------------------------------------------------------------------
	static void WriteLayerData(	chWriter& o_Writer,
							const lyrsData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					lyrsLayersData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const lyrsLayersData& i_Data );

};

