/*****************************************************************************
**	cptrRenderBatchDataUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"

#include "Features/Capture/cptrRenderBatchDataParser.hpp"


//============================================================================
//============================================================================
namespace cptrRenderBatchDataUtil
{
	namespace
	{
		 cptrRenderBatchData l_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData( const fsLocator& i_ConfigFile,
					cptrRenderBatchData& o_Data )
	{
		cptrRenderBatchDataParser::ReadData( i_ConfigFile, o_Data );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( const fsLocator& i_ConfigFile,
					const cptrRenderBatchData& i_Data )
	{
		cptrRenderBatchDataParser::WriteData(i_ConfigFile, i_Data);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(cptrRenderBatchData& i_NewData)
	{
		l_Data = i_NewData;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderBatchData& Data()
	{
		return l_Data;
	}

}	// end of namespace
