/********************************************************************************************\
**  giGIDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/

#ifdef GI_GIDATAPARSER_HPP
#error giGIDataParser.hpp multiply included
#endif
#define GI_GIDATAPARSER_HPP

#ifndef GI_SCRIPTDATA_HPP
#include "Systems/GlobalIllumination/Data/giScriptData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

class giGIDataParser
{
public:
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					giScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const giScriptData& i_Data );

private:
	//========================================================================
	//   ReadGIData
	//========================================================================
	static void ReadGIData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					giGIData& o_Data );

	//========================================================================
	//   WriteGIData
	//========================================================================
	static void WriteGIData(	chWriter& o_Writer,
					const giGIData& i_Data );


};

