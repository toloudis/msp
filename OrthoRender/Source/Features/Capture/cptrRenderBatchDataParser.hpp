/********************************************************************************************\
**  cptrRenderBatchDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef CPTR_RENDERBATCHDATAPARSER_HPP
#error cptrRenderBatchDataParser.hpp multiply included
#endif
#define CPTR_RENDERBATCHDATAPARSER_HPP

#ifndef CPTR_RENDERBATCHDATA_HPP
#include "Features/Capture/cptrRenderBatchData.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace cptrRenderBatchDataParser
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData( const fsLocator& i_ConfigFile,
					cptrRenderBatchData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( const fsLocator& i_ConfigFile,
					const cptrRenderBatchData& i_Data );

}

