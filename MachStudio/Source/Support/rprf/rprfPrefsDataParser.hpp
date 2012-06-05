/********************************************************************************************\
**  rprfPrefsDataParser.hpp
**
**		Parse the render prefs data.
**
**  studio|gpu
\********************************************************************************************/
#ifdef RPRF_PREFSDATAPARSER_HPP
#error rdrLayersDataParser.hpp multiply included
#endif
#define RPRF_PREFSDATAPARSER_HPP

#ifndef RPRFPREFSDATA_HPP
#include "Support/rprf/rprfPrefsData.hpp"
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
namespace rprfPrefsDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					rprfPrefsData& o_PrefsData );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const rprfPrefsData& i_PrefsData );

	//------------------------------------------------------------------------
	//   ReadPrefsData
	//------------------------------------------------------------------------
	void ReadPrefsData(	chReader& io_Reader, chDefs::Version i_Version,
						rprfPrefsData& o_PrefsData );

	//------------------------------------------------------------------------
	//   WritePrefsData
	//------------------------------------------------------------------------
	void WritePrefsData(chWriter& o_Writer,
					const rprfPrefsData& i_PrefsData );

}

