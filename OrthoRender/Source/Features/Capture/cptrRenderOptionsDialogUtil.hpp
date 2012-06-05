/*****************************************************************************
**	cptrRenderOptionsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDEROPTIONSDIALOGUTIL_HPP
#error cptrRenderOptionsDialogUtil.hpp multiply included
#endif
#define CPTR_RENDEROPTIONSDIALOGUTIL_HPP

#ifndef CPTR_RENDEROUTPUTDATA_HPP
#include "Features/Capture/cptrRenderOutputData.hpp"
#endif


//============================================================================
//============================================================================
namespace cptrRenderOptionsDialogUtil
{
	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show(cptrRenderOutputData& o_Data, bool i_bBatchMode = false );

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
