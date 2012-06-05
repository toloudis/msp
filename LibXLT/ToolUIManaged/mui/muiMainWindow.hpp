/*****************************************************************************
**  muiMainWindow.hpp
**
**      Control over main window of application
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_MAINWINDOW_HPP
#error muiMainWindow.hpp multiply included
#endif
#define MUI_MAINWINDOW_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMainWindow.hpp"
#endif

//============================================================================
//============================================================================
class muiMainWindow : public guiMainWindowImpl
{
	//---------------------------------------------------------------------------
	//	SetAppTitle()
	//---------------------------------------------------------------------------
	void SetAppTitle( const char * i_Text );

	//---------------------------------------------------------------------------
	//	GetAppTitle()
	//---------------------------------------------------------------------------
	const char * GetAppTitle();

	//---------------------------------------------------------------------------
	//	give focus to the main window
	//---------------------------------------------------------------------------
	void Focus();
};

