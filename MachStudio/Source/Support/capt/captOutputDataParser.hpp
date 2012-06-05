/********************************************************************************************\
**  captOutputDataParser.hpp
**
**		Parse the capture data.
**
**  StudioGPU
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#ifdef CAPT_OUTPUTDATAPARSER_HPP
#error captOutputDataParser.hpp multiply included
#endif
#define CAPT_OUTPUTDATAPARSER_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif

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
namespace captOutputDataParser
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
					captRenderOutputData& o_CaptureData,
					rlyrLayersData& o_LayerData);

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const captRenderOutputData& i_CaptureData);

	//------------------------------------------------------------------------
	//   ReadCaptureData
	//------------------------------------------------------------------------
	void ReadCaptureData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							captRenderOutputData& o_Data);

	//------------------------------------------------------------------------
	//   WriteCaptureData
	//------------------------------------------------------------------------
	void WriteCaptureData(	chWriter& o_Writer,
							const captRenderOutputData& i_CaptureData);

}

