/********************************************************************************************\
**  billDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef BILL_DATAPARSER_HPP
#error billDataParser.hpp multiply included
#endif
#define BILL_DATAPARSER_HPP

#ifndef BILL_SCRIPTDATA_HPP
#include "Systems/Billboard/Data/billScriptData.hpp"
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
class billDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadBillboard
	//------------------------------------------------------------------------
	static void ReadBillboardChunk(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									billScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteBillboard
	//------------------------------------------------------------------------
	static void WriteBillboardChunk(chWriter& o_Writer,
									const billScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							billListData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
							const billListData& i_Data );



};

