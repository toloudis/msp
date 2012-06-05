/*****************************************************************************
**	guiMainWindow.hpp
**
**		Control over main window of application
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_MAINWINDOW_HPP
#error guiMainWindow.hpp multiply included
#endif
#define GUI_MAINWINDOW_HPP

#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif 

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiMainWindowImpl;


//============================================================================
// static functions define API
//============================================================================
class guiMainWindow : public envAbstraction<guiMainWindowImpl>
{
public:
	//---------------------------------------------------------------------------
	//	SetAppTitle()
	//---------------------------------------------------------------------------
	static void SetAppTitle( const itString& i_Text );

	//---------------------------------------------------------------------------
	//	GetAppTitle()
	//---------------------------------------------------------------------------
	static itString GetAppTitle();

	//---------------------------------------------------------------------------
	//	give focus to the main window
	//---------------------------------------------------------------------------
	static void Focus();
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiMainWindowImpl
{
public:
	//---------------------------------------------------------------------------
	//	SetAppTitle()
	//---------------------------------------------------------------------------
	virtual void SetAppTitle( const itString& i_Text ) = 0;

	//---------------------------------------------------------------------------
	//	GetAppTitle()
	//---------------------------------------------------------------------------
	virtual itString GetAppTitle() = 0;

	//---------------------------------------------------------------------------
	//	give focus to the main window
	//---------------------------------------------------------------------------
	virtual void Focus() = 0;
};
