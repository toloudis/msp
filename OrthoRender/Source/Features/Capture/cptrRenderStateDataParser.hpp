/********************************************************************************************\
**  cptrRenderStateDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef CPTR_RENDERSTATEDATAPARSER_HPP
#error cptrRenderStateDataParser.hpp multiply included
#endif
#define CPTR_RENDERSTATEDATAPARSER_HPP

#ifndef CPTR_RENDEROUTPUTDATA_HPP
#include "Features/Capture/cptrRenderOutputData.hpp"
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
					cptrRenderOutputData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( fsLocator& i_ConfigFile,
					const cptrRenderOutputData& i_Data );
}

