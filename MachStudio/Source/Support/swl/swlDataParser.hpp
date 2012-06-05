/********************************************************************************************\
**  swlDataParser.hpp
**
**	Writes overriden fragments to scene chunk.
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef SWL_DATAPARSER_HPP
#error swlDataParser.hpp multiply included
#endif
#define SWL_DATAPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

//============================================================================
//============================================================================
class chReader;
class chWriter;
class swlData;

//============================================================================
//============================================================================
namespace swlDataParser
{
	//------------------------------------------------------------------------
	//   ReadFragmentData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					swlData& o_data );

	//------------------------------------------------------------------------
	//   WriteFragmentData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const swlData& i_data );

}

