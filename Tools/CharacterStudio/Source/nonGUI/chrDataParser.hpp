/********************************************************************************************\
**  chrDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef CHR_DATAPARSER_HPP
#error chrDataParser.hpp multiply included
#endif
#define CHR_DATAPARSER_HPP


#ifndef CHR_DATA_HPP
#include "nonGUI/chrData.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


class chReader;
class chWriter;
class fsLocator;

namespace chrDataParser
{
	//========================================================================
	//  returns chunk name for this data type
	//========================================================================
	chDefs::Name  GetChunkName();

	//========================================================================
	//   ReadExpression
	//========================================================================
	void ReadExpression(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chrExpression& o_Data );

	//========================================================================
	//   WriteExpression
	//========================================================================
	void WriteExpression(	chWriter& o_Writer,
					const chrExpression& i_Data );

	//========================================================================
	//   ReadData
	//========================================================================
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chrData& o_Data );

	//========================================================================
	// ReadData
	//========================================================================
	void ReadData(	const fsLocator &i_Locator,
					chrData& o_Data );


	//========================================================================
	//   WriteData
	//========================================================================
	void WriteData(	chWriter& o_Writer,
					const chrData& i_Data );

	//========================================================================
	// WriteData
	//========================================================================
	void WriteData( const fsLocator &i_Locator,
					const chrData& i_Data );
}

