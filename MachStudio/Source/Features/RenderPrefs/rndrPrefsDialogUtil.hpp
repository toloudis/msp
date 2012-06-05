/*****************************************************************************
**	rndrPrefsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RNDRPREFSDIALOGUTIL_HPP
#error rndrPrefsDialogUtil.hpp multiply included
#endif
#define RNDRPREFSDIALOGUTIL_HPP


//============================================================================
//============================================================================
namespace rndrPrefsDialogUtil
{
	//------------------------------------------------------------------------
	//  CreateDialog - common step of creating form whether to be
	//	displayed modal or modeless
	//------------------------------------------------------------------------
	void  CreateDialog();

	//------------------------------------------------------------------------
	//  Show
	//------------------------------------------------------------------------
	void  Show();

	//------------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//------------------------------------------------------------------------
	void  Hide();

	//------------------------------------------------------------------------
	//  Update - Property listings have changed so we need to update
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	//  UpdatePfxPage - Property listings have changed so we need to update
	//------------------------------------------------------------------------
	void UpdatePfxPage();

}	// end of namespace
