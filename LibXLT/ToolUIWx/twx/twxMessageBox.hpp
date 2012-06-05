/*****************************************************************************
**  twxMessageBox.hpp
**
**      A non-managed interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_MESSAGEBOX_HPP
#error twxMessageBox.hpp multiply included
#endif
#define TWX_MESSAGEBOX_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
//	Forward references
//============================================================================
class itString;


//============================================================================
//============================================================================
namespace twxMessageBox
{
	enum MBReturn
	{
		e_No = 0,
		e_OK = 1,
		e_Yes = 1,
		e_Retry,
		e_Abort,
		e_Ignore,
		e_Cancel
	};

	enum MBType
	{
		e_OKOnly = 0,
		e_OKCancel,
		e_YesNo,
		e_YesNoCancel,
		e_RetryCancel,
		e_AbortRetryIgnore,
	};

	//---------------------------------------------------------------------------'
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	int Show( const char* i_Message, const char* i_Title, int i_Type = e_OKOnly );
	int Show( const itString& i_Message, const itString& i_Title, int i_Type = e_OKOnly );
}
#endif
