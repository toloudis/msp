/********************************************************************************************\
**  SceneSetupDataParser.hpp
**
**		Parse the SceneSetup data.
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef SCENESETUPDATAPARSER_HPP
#error SceneSetupDataParser.hpp multiply included
#endif
#define SCENESETUPDATAPARSER_HPP

#ifndef SCENESETUPDATA_HPP
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
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
namespace SceneSetupDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					SceneSetupData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const SceneSetupData& i_Data );

}

