/*****************************************************************************
**	rndrPrefsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	Extra Large Technology
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
	//  Show
	//------------------------------------------------------------------------
	void  Show();

	//------------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//------------------------------------------------------------------------
	void  Hide();
}	// end of namespace
