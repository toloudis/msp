/********************************************************************************************\
**  chtrExpressionDefParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005-8 - All Rights Reserved
\********************************************************************************************/

#ifdef CHTR_EXPRESSIONDEFPARSER_HPP
#error chtrExpressionDefParser.hpp multiply included
#endif
#define CHTR_EXPRESSIONDEFPARSER_HPP

#ifndef CHTR_EXPRESSIONDATA_HPP
#include "Systems/Character/Data/chtrExpressionData.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace chtrExpressionDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name GetChunkName();

	//------------------------------------------------------------------------
	//   ReadExpression1
	//------------------------------------------------------------------------
	void ReadExpression1(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						chtrExpressionData& o_Data );

	//------------------------------------------------------------------------
	//   ReadExpression2
	//------------------------------------------------------------------------
	void ReadExpression2(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						chtrExpressionData& o_Data );

	//------------------------------------------------------------------------
	//   ReadExpression4
	//------------------------------------------------------------------------
	void ReadExpression4(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						chtrExpressionData& o_Data );

	//------------------------------------------------------------------------
	//   WriteExpression1
	//------------------------------------------------------------------------
	void WriteExpression1(chWriter& o_Writer,
						 const chtrExpressionData& i_Data );

	//------------------------------------------------------------------------
	//   WriteExpression2
	//------------------------------------------------------------------------
	void WriteExpression2(chWriter& o_Writer,
						 const chtrExpressionData& i_Data );

	//------------------------------------------------------------------------
	//   WriteExpression4
	//------------------------------------------------------------------------
	void WriteExpression4(chWriter& o_Writer,
						 const chtrExpressionData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chtrExpressionsData& o_Data );

	//------------------------------------------------------------------------
	// ReadData
	//------------------------------------------------------------------------
	void ReadData(	const fsLocator &i_Locator,
					chtrExpressionsData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const chtrExpressionsData& i_Data );

	//------------------------------------------------------------------------
	// WriteData
	//------------------------------------------------------------------------
	void WriteData( const fsLocator &i_Locator,
					const chtrExpressionsData& i_Data );
}

