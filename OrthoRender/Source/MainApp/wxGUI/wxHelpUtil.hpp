/****************************************************************************\
**	wxHelpUtil.hpp
**
**		Utility for diplaying help info dialogs in wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef WX_HELPUTIL_HPP
#error wxHelpUtil.hpp multiply included
#endif
#define WX_HELPUTIL_HPP

//============================================================================
//============================================================================
namespace wxHelpUtil
{
	//--------------------------------------------------------------------
	// Show dialog with some help information about codes and hot keys.
	//--------------------------------------------------------------------
	void ShowHelpInfo();

	//--------------------------------------------------------------------
	// Show dialog with version information
	//--------------------------------------------------------------------
	void ShowHelpAbout();
};
