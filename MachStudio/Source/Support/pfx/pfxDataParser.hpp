/********************************************************************************************\
**  pfxDataParser.hpp
**
**		Parse the pfx data.
**
**  studio|gpu
\********************************************************************************************/
#ifdef PFX_DATAPARSER_HPP
#error pfxDataParser.hpp multiply included
#endif
#define PFX_DATAPARSER_HPP

#ifndef PFX_DATA_HPP
#include "Support/pfx/pfxData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;
class effShaderParams;

//============================================================================
//============================================================================
namespace pfxDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadPrefsData
	//------------------------------------------------------------------------
	void ReadPfxData(	chReader& io_Reader, chDefs::Version i_Version,
						pfxData& o_PfxData);

	//------------------------------------------------------------------------
	//   WritePrefsData
	//------------------------------------------------------------------------
	void WritePfxData(chWriter& o_Writer,
					const pfxData& i_PfxData);

}

