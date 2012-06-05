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

namespace
{
	wxAuiManager *l_pAuiMgr = NULL;

} // end of namespace


//---------------------------------------------------------------------------
// Create the pane manager
//---------------------------------------------------------------------------
void twxPaneMgr::Init()
{
	DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
	//l_pAuiMgr = new wxAuiManager(twxSystem::g_pMainForm);
	l_pAuiMgr = new wxAuiManager(twxSystem::g_pMainForm,
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
		if (pane_info.IsOk())
		{
			pane_info.Caption(wxString(i_Caption.c_str(), wxConvUTF8));
			l_pAuiMgr->Update();
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
