/********************************************************************************************\
**  envtEnvironmentsDataParser.hpp
**
**		Environments data parsing
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef ENVT_ENVIRONMENTSDATAPARSER_HPP
#error envtEnvironmentDataParser.hpp multiply included
#endif
#define ENVT_ENVIRONMENTSDATAPARSER_HPP

#ifndef ENVT_SCRIPTDATA_HPP
#include "Systems/Environments/Data/envtScriptData.hpp"
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
class envtEnvironmentsDataParser
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
					envtEnvironmentsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
					const envtEnvironmentsData& i_Data );

	//------------------------------------------------------------------------
	//   ReadEnvironmentData
	//------------------------------------------------------------------------
	static void ReadEnvironmentData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							envtScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteEnvironmentData
	//------------------------------------------------------------------------
	static void WriteEnvironmentData(	chWriter& o_Writer,
							const envtScriptData& i_Data );
};

