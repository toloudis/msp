/*****************************************************************************
**  muiStatusBarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiStatusBarMgr.hpp"
#include "ToolUIManaged/tma/tmaStatusBarMgr.hpp"


//===========================================================================
//	muiStatusBarMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiStatusBarMgr::muiStatusBarMgr()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiStatusBarMgr::~muiStatusBarMgr()
{
}


//---------------------------------------------------------------------------
//	SetText()
//---------------------------------------------------------------------------
void muiStatusBarMgr::SetText( int i_PanelIndex, const char * i_Text )
{
}

//---------------------------------------------------------------------------
//	GetText()
//---------------------------------------------------------------------------
const char * muiStatusBarMgr::GetText( int i_PanelIndex)
{
	return "";
}

//---------------------------------------------------------------------------
//	SetToolTip() - set help text for an individual panel
//---------------------------------------------------------------------------
void muiStatusBarMgr::SetToolTip( int i_PanelIndex, const char * i_Text )
{
	// No implementation in managed code
}

//----------------------------------------------------------------------------
//	CreateModeBitmap - create the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void muiStatusBarMgr::CreateModeBitmap()
{
	// no implementation in managed code
}
//----------------------------------------------------------------------------
//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void muiStatusBarMgr::ShowModeBitmap()
{
	// no implementation in managed code
}
//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void muiStatusBarMgr::HideModeBitmap()
{
	// no implementation in managed code
}

