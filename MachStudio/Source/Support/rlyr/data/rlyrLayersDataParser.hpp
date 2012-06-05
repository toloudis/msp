/********************************************************************************************\
**  rlyrLayersDataParser.hpp
**
**		Parse the render layer data.
**
**  studio|gpu
\********************************************************************************************/
#ifdef RLYR_LAYERSDATAPARSER_HPP
#error rlyrLayersDataParser.hpp multiply included
#endif
#define RLYR_LAYERSDATAPARSER_HPP

#ifndef RLYR_LAYERSDATA_HPP
#include "Support/rlyr/data/rlyrLayersData.hpp"
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
namespace rlyrLayersDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();
	////------------------------------------------------------------------------
	////   Write each piece of data for the current layer
	////------------------------------------------------------------------------
	void WriteAllLayerData(chWriter& o_Writer,
					const rlyrLayerDataItem& i_Data );

	//------------------------------------------------------------------------
	//   Read each piece of data for the current layer
	//------------------------------------------------------------------------
	void ReadAllLayerData(	chReader& i_Reader,
					chDefs::Version i_Version,
				chDefs::Size i_Size,
				rlyrLayerDataItem& o_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					rlyrLayersData& o_LayerData );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const rlyrLayersData& i_LayerData );

}

