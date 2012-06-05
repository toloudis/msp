/********************************************************************************************\
**  ProjectSetupDataParser.hpp
**
**		data parser for the project file
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PROJECTSETUPDATAPARSER_HPP
#error ProjectSetupDataParser.hpp multiply included
#endif
#define PROJECTSETUPDATAPARSER_HPP

#ifndef PROJECTSETUPDATA_HPP
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
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
namespace ProjectSetupDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					ProjectSetupData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const ProjectSetupData& i_Data );
}

