/*****************************************************************************
**  muiMessageBox.hpp
**
**      A non-managed interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_MESSAGEBOX_HPP
#error muiMessageBox.hpp multiply included
#endif
#define MUI_MESSAGEBOX_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif

//============================================================================
//============================================================================
class muiMessageBox : public guiMessageBoxImpl
{
	//---------------------------------------------------------------------------'
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	virtual int Show( const char* i_Message, const char* i_Title, int i_Type );
};

