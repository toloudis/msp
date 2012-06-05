/*****************************************************************************
**  quiStatusBarMgr.hpp
**
**      A non-managed interface to the status bar.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_STATUSBARMGR_HPP
#error quiStatusBarMgr.hpp multiply included
#endif
#define QUI_STATUSBARMGR_HPP

#ifndef GUI_STATUSBARMGR_HPP
#include "Tool/gui/guiStatusBarMgr.hpp"
#endif

//============================================================================
//============================================================================
class quiStatusBarMgr : public guiStatusBarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//	SetText()
	//---------------------------------------------------------------------------
	virtual void SetText( int i_PanelIndex, const char * i_Text );

	//---------------------------------------------------------------------------
	//	GetText()
	//---------------------------------------------------------------------------
	//virtual const char * GetText(int i_PanelIndex);

	//---------------------------------------------------------------------------
	//	SetToolTip() - set help text for an individual panel
	//---------------------------------------------------------------------------
	virtual void SetToolTip( int i_PanelIndex, const char * i_Text );

	//---------------------------------------------------------------------------
	// Set which pane to use for error messages
	//---------------------------------------------------------------------------
	static void SetErrorPaneIndex(int i_Index);

	//---------------------------------------------------------------------------
	//	SetErrorMessage() sets text into main messaging panel
	//---------------------------------------------------------------------------
	virtual void SetErrorMessage( const char * i_Text );
	virtual void ClearErrorMessage();

	//----------------------------------------------------------------------------
	//	CreateModeBitmap - create the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void CreateModeBitmap();

	//----------------------------------------------------------------------------
	//	ShowModeBitmap - Show the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void ShowModeBitmap();

	//----------------------------------------------------------------------------
	//	HideModeBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void HideModeBitmap();

};
