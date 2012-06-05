/********************************************************************************************\
**  grupGroupsDataParser.hpp
**
**		Groups data parsing
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef GRUP_GROUPSDATAPARSER_HPP
#error grupGroupDataParser.hpp multiply included
#endif
#define GRUP_GROUPSDATAPARSER_HPP

#ifndef GRUP_DATA_HPP
#include "Systems/Groups/Data/grupData.hpp"
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
class grupGroupsDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadGroupData
	//------------------------------------------------------------------------
	static void ReadGroupData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							grupData& o_Data );

	//------------------------------------------------------------------------
	//   WriteGroupData
	//------------------------------------------------------------------------
	static void WriteGroupData(	chWriter& o_Writer,
							const grupData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					grupGroupsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData( chWriter& o_Writer,
						   const grupGroupsData& i_Data );

};

