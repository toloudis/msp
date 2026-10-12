/****************************************************************************\
**	qtHelpUtil.hpp
**
**		Utility for displaying help info dialogs in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_HELPUTIL_HPP
#error qtHelpUtil.hpp multiply included
#endif
#define QT_HELPUTIL_HPP

//============================================================================
//============================================================================
namespace qtHelpUtil
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
