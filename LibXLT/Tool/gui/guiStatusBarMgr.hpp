/*****************************************************************************
**	guiStatusBarMgr.hpp
**
**		Interface to the status bar.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_STATUSBARMGR_HPP
#error guiStatusBarMgr.hpp multiply included
#endif
#define GUI_STATUSBARMGR_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiStatusBarMgrImpl;


//============================================================================
// static functions define API
//============================================================================
class guiStatusBarMgr : public envAbstraction<guiStatusBarMgrImpl>
{
public:
	//---------------------------------------------------------------------------
	//	SetText()
	//---------------------------------------------------------------------------
	static void SetText( int i_PanelIndex, const char * i_Text );

	//---------------------------------------------------------------------------
	//	GetText()
	//---------------------------------------------------------------------------
	//static const char * GetText(int i_PanelIndex);

	//---------------------------------------------------------------------------
	//	SetToolTip() - set help text for an individual panel
	//---------------------------------------------------------------------------
	static void SetToolTip( int i_PanelIndex, const char * i_Text );

	//---------------------------------------------------------------------------
	//	SetErrorMessage() sets text into main messaging panel
	//---------------------------------------------------------------------------
	static void SetErrorMessage( const char * i_Text );
	static void ClearErrorMessage();

	//----------------------------------------------------------------------------
	//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	static void CreateModeBitmap();

	//----------------------------------------------------------------------------
	//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	static void ShowModeBitmap();

	//----------------------------------------------------------------------------
	//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	static void HideModeBitmap();

};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiStatusBarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//	SetText()
	//---------------------------------------------------------------------------
	virtual void SetText( int i_PanelIndex, const char * i_Text ) = 0;

	//---------------------------------------------------------------------------
	//	GetText()
	//---------------------------------------------------------------------------
	//virtual const char * GetText(int i_PanelIndex) = 0;

	//---------------------------------------------------------------------------
	//	SetToolTip() - set help text for an individual panel
	//---------------------------------------------------------------------------
	virtual void SetToolTip( int i_PanelIndex, const char * i_Text ) = 0;

	//---------------------------------------------------------------------------
	//	SetErrorMessage() sets text into main messaging panel
	//---------------------------------------------------------------------------
	virtual void SetErrorMessage( const char * i_Text ) = 0;
	virtual void ClearErrorMessage() = 0;

	//----------------------------------------------------------------------------
	//	HideModeBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void CreateModeBitmap() = 0;

	//----------------------------------------------------------------------------
	//	ShowModeBitmap - Show the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void ShowModeBitmap() = 0;

	//----------------------------------------------------------------------------
	//	HideModeBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void HideModeBitmap() = 0;
};

