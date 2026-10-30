/*****************************************************************************
**  twxPaneMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"


#ifdef USE_WXWIDGETS

#include <wx/dcbuffer.h>

namespace
{
	//------------------------------------------------------------------------
	// AUI draws its captions, sashes and borders straight onto the frame,
	// so a caption updated every frame (the timeline's, during playback)
	// flickers between its background and its text: draw through a buffer
	//------------------------------------------------------------------------
	class twxBufferedAuiManager : public wxAuiManager
	{
	public:
		twxBufferedAuiManager(wxWindow* i_pManagedWindow, unsigned int i_Flags)
		:	wxAuiManager(i_pManagedWindow, i_Flags)
		{
			// a bound handler runs before wxAuiManager's event table
			Bind(wxEVT_PAINT, &twxBufferedAuiManager::OnBufferedPaint, this);
		}

	private:
		void OnBufferedPaint(wxPaintEvent&)
		{
			wxBufferedPaintDC dc(GetManagedWindow());
			Repaint(&dc);
		}
	};

	wxAuiManager *l_pAuiMgr = NULL;

} // end of namespace


//---------------------------------------------------------------------------
// Create the pane manager
//---------------------------------------------------------------------------
void twxPaneMgr::Init()
{
	DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
	//l_pAuiMgr = new wxAuiManager(twxSystem::g_pMainForm);
	l_pAuiMgr = new twxBufferedAuiManager(twxSystem::g_pMainForm,
					wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE);
}

//---------------------------------------------------------------------------
// Shut down the pane management
//---------------------------------------------------------------------------
void twxPaneMgr::CleanUp()
{
	// Not sure if we need to delete it
	//bga - Yes, the sample code use a member variable (not pointer) for AUI manager, so
	// it is safe to delete the manager
	if (l_pAuiMgr)
		delete l_pAuiMgr;

	l_pAuiMgr = NULL;
}

//---------------------------------------------------------------------------
// Add new pane with given info
//---------------------------------------------------------------------------
void twxPaneMgr::AddPane(wxWindow *i_pWindow, const wxAuiPaneInfo &i_PaneInfo)
{
	DBG_ASSERT(l_pAuiMgr, "twxPaneMgr not yet initialized.");
	if (l_pAuiMgr)
	{
		l_pAuiMgr->AddPane(i_pWindow, i_PaneInfo);
		l_pAuiMgr->Update();
	}
}

//---------------------------------------------------------------------------
// Refresh panes in situations where the frame size changed without
// the pane manager knowing (i.e. when status bar changes visibility)
//---------------------------------------------------------------------------
void twxPaneMgr::Update()
{
	if (l_pAuiMgr)
		l_pAuiMgr->Update();
}

//---------------------------------------------------------------------------
// Show the pane with the given name or window
// To Hide the pane, call with i_bShow==false
//---------------------------------------------------------------------------
void twxPaneMgr::Show(const char* i_PaneName, bool i_bShow)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(wxString(i_PaneName, wxConvUTF8));
		if (pane_info.IsOk())
		{
			pane_info.Show(i_bShow);
			l_pAuiMgr->Update();
		}
	}
}
void twxPaneMgr::Show(wxWindow *i_pWindow, bool i_bShow)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			pane_info.Show(i_bShow);
			l_pAuiMgr->Update();
		}
	}
}

//---------------------------------------------------------------------------
// Return true if theis pane is visible currently
//---------------------------------------------------------------------------
bool twxPaneMgr::IsVisible(const char* i_PaneName)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(wxString(i_PaneName, wxConvUTF8));
		if (pane_info.IsOk())
		{
			return pane_info.IsShown();
		}
	}
	return false;
}
bool twxPaneMgr::IsVisible(wxWindow *i_pWindow)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			return pane_info.IsShown();
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// SetCaption - alter caption for pane
//---------------------------------------------------------------------------
void twxPaneMgr::SetCaption(wxWindow *i_pWindow, const std::string& i_Caption)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		const wxString caption(i_Caption.c_str(), wxConvUTF8);
		if (pane_info.IsOk() && pane_info.caption != caption)
		{
			// Only the caption is redrawn: l_pAuiMgr->Update() would lay out
			// the whole frame again, freezing, resizing and repainting every pane, to
			// change one caption's text (the timeline's changes every frame
			// of playback). Qt's pane manager likewise only sets the title.
			pane_info.Caption(caption);
			if (pane_info.IsFloating() && pane_info.frame)
			{
				pane_info.frame->SetTitle(caption);
			}
			else if (pane_info.IsDocked() && pane_info.IsShown() && pane_info.HasCaption())
			{
				// a docked caption is drawn on the managed window, just
				// above the pane's window
				const wxRect window_rect = i_pWindow->GetRect();
				const int caption_size = l_pAuiMgr->GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
				l_pAuiMgr->GetManagedWindow()->RefreshRect(
					wxRect(window_rect.x, window_rect.y - caption_size, window_rect.width, caption_size), false);
			}
		}
	}
}

//---------------------------------------------------------------------------
// Resize the pane for the given window
//---------------------------------------------------------------------------
void twxPaneMgr::ResizePane(wxWindow *i_pWindow, int i_Width, int i_Height)
{
	if (l_pAuiMgr)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			wxSize s(i_Width, i_Height);
			//pane_info.MinSize(s).BestSize(s);
			pane_info.BestSize(s);
			l_pAuiMgr->Update();
		}
	}
}

//---------------------------------------------------------------------------
// A Perspective is a string that represents the configuration of panes.
//	Using these functions the layout can be saved and restored.
//---------------------------------------------------------------------------
bool twxPaneMgr::GetCurrentPerspective(std::string& o_Perspective)
{
	if (l_pAuiMgr)
	{
		wxString persp = l_pAuiMgr->SavePerspective();
		o_Perspective = persp.utf8_str();
		return true;
	}
	return false;
	
}
void twxPaneMgr::SetCurrentPerspective(const std::string& i_Perspective)
{
	if (l_pAuiMgr)
	{
		l_pAuiMgr->LoadPerspective(wxString(i_Perspective.c_str(), wxConvUTF8));
	}
}

#endif // USE_WXWIDGETS
