/*****************************************************************************
**	plbkPlaybackControlsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PLBK_PLAYBACKCONTROLSDIALOGUTIL_HPP
#error plbkPlaybackControlsDialogUtil.hpp multiply included
#endif
#define PLBK_PLAYBACKCONTROLSDIALOGUTIL_HPP


//============================================================================
//============================================================================
namespace plbkPlaybackControlsDialogUtil
{
	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide();

}	// end of namespace
