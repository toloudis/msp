/*****************************************************************************
**	tqtMessageBox.hpp
**
**	An interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_MESSAGEBOX_HPP
#error tqtMessageBox.hpp multiply included
#endif
#define TQT_MESSAGEBOX_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif


//============================================================================
//	Forward references
//============================================================================
class itString;


//============================================================================
//============================================================================
namespace tqtMessageBox
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

	//---------------------------------------------------------------------------
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	int Show( const char* i_Message, const char* i_Title, int i_Type = e_OKOnly );
	int Show( const itString& i_Message, const itString& i_Title, int i_Type = e_OKOnly );
}
