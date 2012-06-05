/********************************************************************************************\
**  cptrRenderStateDataParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef CPTR_RENDERSTATEDATAPARSER_HPP
#error cptrRenderStateDataParser.hpp multiply included
#endif
#define CPTR_RENDERSTATEDATAPARSER_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace cptrRenderStateDataParser
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData(	fsLocator& i_ConfigFile,
					captRenderOutputData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( fsLocator& i_ConfigFile,
					const captRenderOutputData& i_Data );
}

