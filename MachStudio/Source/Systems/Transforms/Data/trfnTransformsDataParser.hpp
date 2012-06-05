/********************************************************************************************\
**  trfnTransformsDataParser.hpp
**
**		Transforms data parsing
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TRFN_TRANSFORMSDATAPARSER_HPP
#error trfnTransformDataParser.hpp multiply included
#endif
#define TRFN_TRANSFORMSDATAPARSER_HPP

#ifndef TRFN_SCRIPTDATA_HPP
#include "Systems/Transforms/Data/trfnScriptData.hpp"
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
class trfnTransformsDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadTransformData
	//------------------------------------------------------------------------
	static void ReadTransformData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							trfnScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteTransformData
	//------------------------------------------------------------------------
	static void WriteTransformData(	chWriter& o_Writer,
							const trfnScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					trfnTransformsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const trfnTransformsData& i_Data );

};

