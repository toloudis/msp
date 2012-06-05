/*****************************************************************************
**  quiMainWindow.hpp
**
**      Control over main window of application
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_MAINWINDOW_HPP
#error quiMainWindow.hpp multiply included
#endif
#define QUI_MAINWINDOW_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMainWindow.hpp"
#endif

//============================================================================
//============================================================================
class quiMainWindow : public guiMainWindowImpl
{
	//---------------------------------------------------------------------------
	//	SetAppTitle()
	//---------------------------------------------------------------------------
	void SetAppTitle( const itString& i_Text );

	//---------------------------------------------------------------------------
	//	GetAppTitle()
	//---------------------------------------------------------------------------
	itString GetAppTitle();

	//---------------------------------------------------------------------------
	//	give focus to the main window
	//---------------------------------------------------------------------------
	void Focus();
};

