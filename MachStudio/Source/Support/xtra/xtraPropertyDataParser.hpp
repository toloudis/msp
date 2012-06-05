/********************************************************************************************\
**  xtraPropertyDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef XTRA_PROPERTYDATAPARSER_HPP
#error xtraPropertyDataParser.hpp multiply included
#endif
#define XTRA_PROPERTYDATAPARSER_HPP

#ifndef XTRA_PROPERTYDATA_HPP
#include "Support/xtra/xtraPropertyData.hpp"
#endif 

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

class chReader;
class chWriter;

namespace xtraPropertyDataParser
{
	//========================================================================
	//   ReadCustomPropertyData
	//========================================================================
	void ReadCustomPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<shared_ptr<xtraPropertyData>>& o_Properties );

	//========================================================================
	//   WriteCustomPropertyData
	//========================================================================
	void WriteCustomPropertyData(	chWriter& o_Writer,
							const std::vector<shared_ptr<xtraPropertyData>>& i_Properties );

}

