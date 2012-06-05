/*****************************************************************************
**	cptrRenderBatchDataUtil.hpp
**
**		API for opening the capture batch dialog
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERBATCHDATAUTIL_HPP
#error cptrRenderBatchDataUtil.hpp multiply included
#endif
#define CPTR_RENDERBATCHDATAUTIL_HPP

#ifndef CPTR_RENDERBATCHDATA_HPP
#include "Features/Capture/cptrRenderBatchData.hpp"
#endif


//============================================================================
//============================================================================
namespace cptrRenderBatchDataUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData( const fsLocator& i_ConfigFile,
					cptrRenderBatchData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( const fsLocator& i_ConfigFile,
					const cptrRenderBatchData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(cptrRenderBatchData& i_NewData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetSkipSceneWithErrors(bool i_flag);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderBatchData& Data();

}	// end of namespace
