/*****************************************************************************
**  quiStatusBarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiStatusBarMgr.hpp"

#include "ToolUIQt/tqc/tqcStatusBar.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

//===========================================================================
//	quiStatusBarMgr functions
//===========================================================================
namespace 
{
	int l_ErrorPaneIndex = -1;
#ifdef QT_FINISH_PORT
	wxBitmapButton* l_ModeBMP;

	wxBitmap CreateBitmapForButton()
	{
		static const int BMP_BUTTON_SIZE_X = 90;
		static const int BMP_BUTTON_SIZE_Y = 20;
			
		wxBitmap bitmap(BMP_BUTTON_SIZE_X, BMP_BUTTON_SIZE_Y);
		wxMemoryDC dc;
		dc.SelectObject(bitmap);
		dc.SetBrush(*wxRED_BRUSH);
		dc.SetBackground(*wxLIGHT_GREY_BRUSH);
		dc.Clear();
		dc.DrawRectangle(0, 0, BMP_BUTTON_SIZE_X, BMP_BUTTON_SIZE_Y);
		dc.DrawText(L"Locked", 10, 0);
		dc.SelectObject(wxNullBitmap);

		return bitmap;
	}
#endif // USE_QT
}


//---------------------------------------------------------------------------
//	SetText()
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetText( int i_PanelIndex, const char * i_Text )
{
#ifdef QT_FINISH_PORT
	if (tqtSystem::g_pStatusBar)
	{
		tqtSystem::g_pStatusBar->SetStatusText(wxString(i_Text, wxConvUTF8), i_PanelIndex);
	}
#endif
}

//---------------------------------------------------------------------------
//	GetText()
//---------------------------------------------------------------------------
//const char * quiStatusBarMgr::GetText( int i_PanelIndex)
//{
//#ifdef QT_FINISH_PORT
//	if (tqtSystem::g_pStatusBar)
//	{
//		//bga -  this would return a temporary, can't be done this way.
//		// Commenting this function out.
//		return tqtSystem::g_pStatusBar->GetStatusText(i_PanelIndex).c_str();
//	}
//#endif
//	return "";
//}

//---------------------------------------------------------------------------
//	SetToolTip() - set help text for an individual panel
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetToolTip( int i_PanelIndex, const char * i_Text )
{
#ifdef QT_FINISH_PORT
	if (tqtSystem::g_pStatusBar)
	{
		tqcStatusBar *pToolTipStatusBar = dynamic_cast<tqcStatusBar*>(tqtSystem::g_pStatusBar);
		if (pToolTipStatusBar)
			pToolTipStatusBar->SetPanelToolTip(i_PanelIndex, i_Text);
	}
#endif
}

//---------------------------------------------------------------------------
// Set which pane to use for error messages
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetErrorPaneIndex(int i_Index)
{
	l_ErrorPaneIndex = i_Index;
}

//---------------------------------------------------------------------------
//	SetErrorMessage() sets text into main messaging panel
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetErrorMessage( const char * i_Text )
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, i_Text);
}
void quiStatusBarMgr::ClearErrorMessage()
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, "");
}

//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void quiStatusBarMgr::CreateModeBitmap()
{
#ifdef QT_FINISH_PORT
	if (tqtSystem::g_pStatusBar)
	{
		wxRect rect;
		tqtSystem::g_pStatusBar->GetFieldRect(0, rect);
		l_ModeBMP = new wxBitmapButton(tqtSystem::g_pStatusBar, wxID_ANY, wxBitmap(),
                                   rect.GetPosition(), rect.GetSize(),
                                   wxBU_EXACTFIT);

		l_ModeBMP->SetBitmapLabel(CreateBitmapForButton());
		l_ModeBMP->Refresh();
		l_ModeBMP->Hide();
	}
#endif
}

//----------------------------------------------------------------------------
//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void quiStatusBarMgr::ShowModeBitmap()
{
#ifdef QT_FINISH_PORT
	if (tqtSystem::g_pStatusBar && l_ModeBMP)
	{
		l_ModeBMP->Show();
		l_ModeBMP->Refresh();
	}
#endif
}

//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void quiStatusBarMgr::HideModeBitmap()
{
#ifdef QT_FINISH_PORT
	if (tqtSystem::g_pStatusBar && l_ModeBMP)
	{
		l_ModeBMP->Hide();
		l_ModeBMP->Refresh();
	}
#endif
}


