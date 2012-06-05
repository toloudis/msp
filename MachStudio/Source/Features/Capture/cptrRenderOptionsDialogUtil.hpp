/*****************************************************************************
**	cptrRenderOptionsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDEROPTIONSDIALOGUTIL_HPP
#error cptrRenderOptionsDialogUtil.hpp multiply included
#endif
#define CPTR_RENDEROPTIONSDIALOGUTIL_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif


//============================================================================
//============================================================================
namespace cptrRenderOptionsDialogUtil
{
	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show(captRenderOutputData& o_Data, bool i_bBatchMode = false );

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	//	returns true if the dialog is exitting by hitting the "X" button
	//--------------------------------------------------------------------
	bool IsExitting();
	void SetExitting(bool i_bExitting);

}	// end of namespace
