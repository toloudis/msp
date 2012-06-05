/*****************************************************************************
**	quiMessageBox.hpp
**
**		An interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_MESSAGEBOX_HPP
#error quiMessageBox.hpp multiply included
#endif
#define QUI_MESSAGEBOX_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif


//============================================================================
//============================================================================
class quiMessageBox : public guiMessageBoxImpl
{
	//---------------------------------------------------------------------------'
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	virtual int Show( const char* i_Message, const char* i_Title, int i_Type );
	virtual int Show( const itString& i_Message, const itString& i_Title, int i_Type );
};

