/********************************************************************************************\
**  cmraDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef CMRA_DATAPARSER_HPP
#error cmraDataParser.hpp multiply included
#endif
#define CMRA_DATAPARSER_HPP

#ifndef CMRA_SCRIPTDATA_HPP
#include "Systems/Cameras/Data/cmraScriptData.hpp"
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
class cmraCamerasDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraCamerasData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const cmraCamerasData& i_Data );
};	// end of static class


