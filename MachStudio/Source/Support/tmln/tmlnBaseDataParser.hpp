/****************************************************************************\
**  tmlnBaseDataParser.hpp
**
**  TODO: phase out this class/file
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_BASEDATAPARSER_HPP
#error tmlnBaseDataParser.hpp multiply included
#endif
#define TMLN_BASEDATAPARSER_HPP

#ifndef TMLN_BASEDATA_HPP
#include "Support/tmln/tmlnBaseData.hpp"
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
namespace tmlnBaseDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData - use this one for external reading
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
				    chDefs::Name i_SystemCode,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnBaseData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData - use this one for external writing
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
				    chDefs::Name i_SystemCode,
					const tmlnBaseData& i_Data );
}
