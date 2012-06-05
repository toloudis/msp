/********************************************************************************************\
**  tmlnDriverInfoParser.hpp
**
**  reads and writes common driver information, like begin time,
**		end time and name
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERINFOPARSER_HPP
#error tmlnDriverInfoParser.hpp multiply included
#endif
#define TMLN_DRIVERINFOPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <string>

class tmlnDriverInfo;
class chReader;
class chWriter;
class fsLocator;


//========================================================================
//========================================================================
namespace tmlnDriverInfoParser
{
	//========================================================================
	// Chunk name used for driver base information
	//========================================================================
	chDefs::Name GetChunkName();

	//========================================================================
	//   ReadDriverInfo - read common driver information, like begin time,
	//		end time and name
	//========================================================================
	void ReadDriverInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverInfo& o_Driver );

	//========================================================================
	//   WriteDriverInfo
	//========================================================================
	void WriteDriverInfo(	chWriter& o_Writer,
							const tmlnDriverInfo& i_Driver );

}

