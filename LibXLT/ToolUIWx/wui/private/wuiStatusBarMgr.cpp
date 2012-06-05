/*****************************************************************************
**  wuiStatusBarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiStatusBarMgr.hpp"

#include "ToolUIWx/twc/twcStatusBar.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

//===========================================================================
//	wuiStatusBarMgr functions
//===========================================================================
namespace 
{
	int l_ErrorPaneIndex = -1;
#ifdef USE_WXWIDGETS
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
#endif // USE_WXWIDGETS
}


//---------------------------------------------------------------------------
//	SetText()
//---------------------------------------------------------------------------
void wuiStatusBarMgr::SetText( int i_PanelIndex, const char * i_Text )
{
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pStatusBar)
	{
		twxSystem::g_pStatusBar->SetStatusText(wxString(i_Text, wxConvUTF8), i_PanelIndex);
	}
#endif
}

//---------------------------------------------------------------------------
//	GetText()
//---------------------------------------------------------------------------
//const char * wuiStatusBarMgr::GetText( int i_PanelIndex)
//{
//#ifdef USE_WXWIDGETS
//	if (twxSystem::g_pStatusBar)
//	{
//		//bga -  this would return a temporary, can't be done this way.
//		// Commenting this function out.
//		return twxSystem::g_pStatusBar->GetStatusText(i_PanelIndex).c_str();
//	}
//#endif
//	return "";
//}

//---------------------------------------------------------------------------
//	SetToolTip() - set help text for an individual panel
//---------------------------------------------------------------------------
void wuiStatusBarMgr::SetToolTip( int i_PanelIndex, const char * i_Text )
{
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pStatusBar)
	{
		twcStatusBar *pToolTipStatusBar = dynamic_cast<twcStatusBar*>(twxSystem::g_pStatusBar);
		if (pToolTipStatusBar)
			pToolTipStatusBar->SetPanelToolTip(i_PanelIndex, i_Text);
	}
#endif
}

//---------------------------------------------------------------------------
// Set which pane to use for error messages
//---------------------------------------------------------------------------
void wuiStatusBarMgr::SetErrorPaneIndex(int i_Index)
{
	l_ErrorPaneIndex = i_Index;
}

//---------------------------------------------------------------------------
//	SetErrorMessage() sets text into main messaging panel
//---------------------------------------------------------------------------
void wuiStatusBarMgr::SetErrorMessage( const char * i_Text )
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, i_Text);
}
void wuiStatusBarMgr::ClearErrorMessage()
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, "");
}

//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void wuiStatusBarMgr::CreateModeBitmap()
{
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pStatusBar)
	{
		wxRect rect;
		twxSystem::g_pStatusBar->GetFieldRect(0, rect);
		l_ModeBMP = new wxBitmapButton(twxSystem::g_pStatusBar, wxID_ANY, wxBitmap(),
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
void wuiStatusBarMgr::ShowModeBitmap()
{
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pStatusBar && l_ModeBMP)
	{
		l_ModeBMP->Show();
		l_ModeBMP->Refresh();
	}
#endif
}

//----------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//----------------------------------------------------------------------------
void wuiStatusBarMgr::HideModeBitmap()
{
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pStatusBar && l_ModeBMP)
	{
		l_ModeBMP->Hide();
		l_ModeBMP->Refresh();
	}
#endif
}


