/*****************************************************************************
**	guiStatusBarMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiStatusBarMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"


//---------------------------------------------------------------------------
//	SetText()
//---------------------------------------------------------------------------
void guiStatusBarMgr::SetText( int i_PanelIndex, const char * i_Text )
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetText(i_PanelIndex, i_Text);
	}
}

//---------------------------------------------------------------------------
//	GetText()
//---------------------------------------------------------------------------
//const char * guiStatusBarMgr::GetText( int i_PanelIndex)
//{
//	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
//	if (sm_pImplementation)
//	{
//		return sm_pImplementation->GetText(i_PanelIndex);
//	}
//	return "";
//}

//---------------------------------------------------------------------------
//	SetToolTip() - set help text for an individual panel
//---------------------------------------------------------------------------
void guiStatusBarMgr::SetToolTip( int i_PanelIndex, const char * i_Text )
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetToolTip(i_PanelIndex, i_Text);
	}
}

//---------------------------------------------------------------------------
//	SetErrorMessage() sets text into main messaging panel
//---------------------------------------------------------------------------
void guiStatusBarMgr::SetErrorMessage( const char * i_Text )
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetErrorMessage(i_Text);
	}
}
void guiStatusBarMgr::ClearErrorMessage()
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->ClearErrorMessage();
	}
}

//----------------------------------------------------------------------------
//	CreateModeBitmap - create the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void guiStatusBarMgr::CreateModeBitmap()
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->CreateModeBitmap();
	}
}
//----------------------------------------------------------------------------
//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void guiStatusBarMgr::ShowModeBitmap()
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->ShowModeBitmap();
	}
}
//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void guiStatusBarMgr::HideModeBitmap()
{
	DBG_ASSERT(sm_pImplementation, "guiStatusBarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->HideModeBitmap();
	}
}
