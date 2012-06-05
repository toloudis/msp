/*****************************************************************************
**	cptrRenderBakeDialogUtil.hpp
**
**		API for opening the capture bake dialog
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CPTR_RENDERBAKEDIALOGUTIL_HPP
#error cptrRenderBakeDialogUtil.hpp multiply included
#endif
#define CPTR_RENDERBAKEDIALOGUTIL_HPP

//#ifndef CPTR_RENDERBATCHDATA_HPP
//#include "Features/Capture/cptrRenderBatchData.hpp"
//#endif


//============================================================================
//============================================================================
namespace cptrRenderBakeDialogUtil
{
	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	//	IsDialogClosing() - return true if the user has hit the
	//	"capture" button closing the dialog
	//--------------------------------------------------------------------
	bool IsDialogClosing();

	//--------------------------------------------------------------------
	//	SetDialogClosing() - set to true if the user has hit the
	//	"capture" button closing the dialog
	//--------------------------------------------------------------------
	void SetDialogClosing( bool i_bClosing );

	//--------------------------------------------------------------------
	//	IsDialogExitting() - return true if the user has hit the
	//	"X" button Exitting the dialog
	//--------------------------------------------------------------------
	bool IsDialogExitting();

	//--------------------------------------------------------------------
	//	SetDialogExitting() - set to true if the user has hit the
	//	"X" button Exitting the dialog
	//--------------------------------------------------------------------
	void SetDialogExitting( bool i_bExitting );
}	// end of namespace
