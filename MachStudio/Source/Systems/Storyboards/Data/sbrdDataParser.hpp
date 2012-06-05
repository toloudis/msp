/********************************************************************************************\
**  sbrdDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef SBRD_DATAPARSER_HPP
#error sbrdDataParser.hpp multiply included
#endif
#define SBRD_DATAPARSER_HPP

#ifndef SBRD_SCRIPTDATA_HPP
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"
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
class sbrdListData;


//============================================================================
//============================================================================
class sbrdDataParser
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
							sbrdListData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const sbrdListData& i_Data );


private:
	//------------------------------------------------------------------------
	//   ReadObjectChunk
	//------------------------------------------------------------------------
	static void ReadObjectChunk(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									sbrdScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteObjectChunk
	//------------------------------------------------------------------------
	static void WriteObjectChunk(	chWriter& o_Writer,
									const sbrdScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadListChunk
	//------------------------------------------------------------------------
	static void ReadListChunk(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								sbrdListData& o_Data );

	//------------------------------------------------------------------------
	//   WriteListChunk
	//------------------------------------------------------------------------
	static void WriteListChunk(	chWriter& o_Writer,
								const sbrdListData& i_Data );
};

