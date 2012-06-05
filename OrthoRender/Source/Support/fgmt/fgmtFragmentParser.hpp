/********************************************************************************************\
**  fgmtFragmentParser.hpp
**
**	Writes overriden fragments to scene chunk.
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef FGMT_FRAGMENTPARSER_HPP
#error fgmtFragmentParser.hpp multiply included
#endif
#define FGMT_FRAGMENTPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

//============================================================================
//============================================================================
class chReader;
class chWriter;
class fgmtFragmentData;
class fgmtFragmentsAOData;

//============================================================================
//============================================================================
namespace fgmtFragmentParser
{
	//------------------------------------------------------------------------
	//   ReadFragmentData
	//------------------------------------------------------------------------
	void ReadFragmentData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<fgmtFragmentData>& o_Controls,
					fgmtFragmentsAOData& o_AOData );

	//------------------------------------------------------------------------
	//   WriteFragmentData
	//------------------------------------------------------------------------
	void WriteFragmentData(	chWriter& o_Writer,
					const std::vector<fgmtFragmentData>& i_Controls,
					const fgmtFragmentsAOData& i_AOData );

}

