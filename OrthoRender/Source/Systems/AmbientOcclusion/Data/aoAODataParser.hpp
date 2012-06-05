/********************************************************************************************\
**  aoAODataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef AO_AODATAPARSER_HPP
#error aoAODataParser.hpp multiply included
#endif
#define AO_AODATAPARSER_HPP

#ifndef AO_SCRIPTDATA_HPP
#include "Systems/AmbientOcclusion/Data/aoScriptData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

class aoAODataParser
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
					aoScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const aoScriptData& i_Data );

private:
	//========================================================================
	//   ReadAOData
	//========================================================================
	static void ReadAOData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					aoAOData& o_Data );

	//========================================================================
	//   WriteAOData
	//========================================================================
	static void WriteAOData(	chWriter& o_Writer,
					const aoAOData& i_Data );


};

