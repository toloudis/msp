/********************************************************************************************\
**  dynControlDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef DYN_CONTROLDATAPARSER_HPP
#error dynControlDataParser.hpp multiply included
#endif
#define DYN_CONTROLDATAPARSER_HPP

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

class chReader;
class chWriter;

namespace dynControlDataParser
{
	//========================================================================
	//   ReadControlData
	//========================================================================
	void ReadControlData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<dynControlData>& o_Controls );

	//========================================================================
	//   WriteControlData
	//========================================================================
	void WriteControlData(	chWriter& o_Writer,
					const std::vector<dynControlData>& i_Controls );

}

