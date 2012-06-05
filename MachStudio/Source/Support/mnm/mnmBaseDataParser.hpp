/****************************************************************************\
**  mnmBaseDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_BASEDATAPARSER_HPP
#error mnmBaseDataParser.hpp multiply included
#endif
#define MNM_BASEDATAPARSER_HPP

#ifndef MNM_BASEDATA_HPP
#include "Support/mnm/mnmBaseData.hpp"
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
namespace mnmBaseDataParser
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
					mnmBaseData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData - use this one for external writing
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const mnmBaseData& i_Data );
}
