/********************************************************************************************\
**  fogFogDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef FOG_FOGDATAPARSER_HPP
#error fogFogDataParser.hpp multiply included
#endif
#define FOG_FOGDATAPARSER_HPP

#ifndef FOG_SCRIPTDATA_HPP
#include "Systems/Fog/Data/fogScriptData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

class fogFogDataParser
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
					fogScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const fogScriptData& i_Data );

private:
	//========================================================================
	//   ReadFogData
	//========================================================================
	static void ReadFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data );

	//========================================================================
	//   ReadLegacyFogData
	//========================================================================
	static void ReadLegacyFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data );

	//========================================================================
	//   WriteFogData
	//========================================================================
	static void WriteFogData(	chWriter& o_Writer,
					const fogFogData& i_Data );


};

