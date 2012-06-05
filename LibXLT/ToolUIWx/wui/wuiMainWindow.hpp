/*****************************************************************************
**  wuiMainWindow.hpp
**
**      Control over main window of application
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef WUI_MAINWINDOW_HPP
#error wuiMainWindow.hpp multiply included
#endif
#define WUI_MAINWINDOW_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMainWindow.hpp"
#endif

//============================================================================
//============================================================================
class wuiMainWindow : public guiMainWindowImpl
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

