/*****************************************************************************
**  muiStatusBarMgr.hpp
**
**      A non-managed interface to the status bar.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_STATUSBARMGR_HPP
#error muiStatusBarMgr.hpp multiply included
#endif
#define MUI_STATUSBARMGR_HPP

#ifndef GUI_STATUSBARMGR_HPP
#include "Tool/gui/guiStatusBarMgr.hpp"
#endif

//============================================================================
//============================================================================
class muiStatusBarMgr : public guiStatusBarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	muiStatusBarMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~muiStatusBarMgr();

	//---------------------------------------------------------------------------
	//	SetText()
	//---------------------------------------------------------------------------
	virtual void SetText( int i_PanelIndex, const char * i_Text );

	//---------------------------------------------------------------------------
	//	GetText()
	//---------------------------------------------------------------------------
	virtual const char * GetText(int i_PanelIndex);

	//---------------------------------------------------------------------------
	//	SetToolTip() - set help text for an individual panel
	//---------------------------------------------------------------------------
	virtual void SetToolTip( int i_PanelIndex, const char * i_Text );

	//----------------------------------------------------------------------------
	//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void CreateModeBitmap();

	//----------------------------------------------------------------------------
	//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void ShowModeBitmap();

	//----------------------------------------------------------------------------
	//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
	//----------------------------------------------------------------------------
	virtual void HideModeBitmap();
};
