/*****************************************************************************
**	PrefsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PREFSDIALOGUTIL_HPP
#error PrefsDialogUtil.hpp multiply included
#endif
#define PREFSDIALOGUTIL_HPP

#include <string>

//============================================================================
//============================================================================
namespace PrefsDialogUtil
{
	//--------------------------------------------------------------------
	// Return name of toolbar for layouts in order to show/hide it 
	//	in a command.
	//--------------------------------------------------------------------
	const char* GetLayoutToolbarName();

	//------------------------------------------------------------------------
	//  CreateDialog - common step of creating form whether to be
	//	displayed modal or modeless
	//------------------------------------------------------------------------
	void  CreateDialog();

	//--------------------------------------------------------------------
	// Create combo box with layout configurations 
	//--------------------------------------------------------------------
	void  CreateLayoutToolbar();

	//------------------------------------------------------------------------
	//  UpdateLayouts - update list of named layouts.
	//	If i_CurrentLayoutName is non-empty, select it in the combo box
	//------------------------------------------------------------------------
	void  UpdateLayouts(const std::string& i_CurrentLayoutName = "");

	//------------------------------------------------------------------------
	//  Show
	//------------------------------------------------------------------------
	void  Show();

}	// end of namespace
