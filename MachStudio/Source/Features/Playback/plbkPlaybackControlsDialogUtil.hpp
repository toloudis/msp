/*****************************************************************************
**	plbkPlaybackControlsDialogUtil.hpp
**
**		API for opening the playback dialog
**
**	StudioGPU
**	Copyright(C) 2004-9 - All Rights Reserved
\****************************************************************************/
#ifdef PLBK_PLAYBACKCONTROLSDIALOGUTIL_HPP
#error plbkPlaybackControlsDialogUtil.hpp multiply included
#endif
#define PLBK_PLAYBACKCONTROLSDIALOGUTIL_HPP


//============================================================================
//============================================================================
namespace plbkPlaybackControlsDialogUtil
{
	//------------------------------------------------------------------------
	//  Init - call to initialize the dialog
	//------------------------------------------------------------------------
	void  Init();

	//------------------------------------------------------------------------
	// CleanUp - only call this once on app clean-up
	//------------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide();

}	// end of namespace
