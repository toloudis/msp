/*****************************************************************************
**	tqtPaneMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
#ifdef QT_FINISH_PORT
	wxAuiManager *l_pAuiMgr = NULL;
#endif
} // end of namespace


//---------------------------------------------------------------------------
// Create the pane manager
//---------------------------------------------------------------------------
void tqtPaneMgr::Init()
{
	DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
#ifdef QT_FINISH_PORT
	//l_pAuiMgr = new wxAuiManager(tqtSystem::g_pMainForm);
	l_pAuiMgr = new wxAuiManager(tqtSystem::g_pMainForm,
					wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE);
#endif
}

//---------------------------------------------------------------------------
// Shut down the pane management
//---------------------------------------------------------------------------
void tqtPaneMgr::CleanUp()
{
	// Not sure if we need to delete it
	//bga - Yes, the sample code use a member variable (not pointer) for AUI manager, so
	// it is safe to delete the manager
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
		delete l_pAuiMgr;

	l_pAuiMgr = NULL;
#endif
}

//---------------------------------------------------------------------------
// Add new pane with given info
//---------------------------------------------------------------------------
#ifdef QT_FINISH_PORT
void tqtPaneMgr::AddPane(QWidget *i_pWindow, const wxAuiPaneInfo &i_PaneInfo)
{
	DBG_ASSERT(l_pAuiMgr, "tqtPaneMgr not yet initialized.");
	if (l_pAuiMgr != NULL)
	{
		l_pAuiMgr->AddPane(i_pWindow, i_PaneInfo);
		l_pAuiMgr->Update();
	}
}
#endif

//---------------------------------------------------------------------------
// Refresh panes in situations where the frame size changed without
// the pane manager knowing (i.e. when status bar changes visibility)
//---------------------------------------------------------------------------
void tqtPaneMgr::Update()
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
		l_pAuiMgr->Update();
#endif
}

//---------------------------------------------------------------------------
// Show the pane with the given name or window
// To Hide the pane, call with i_bShow==false
//---------------------------------------------------------------------------
void tqtPaneMgr::Show(const char* i_PaneName, bool i_bShow)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(wxString(i_PaneName, wxConvUTF8));
		if (pane_info.IsOk())
		{
			pane_info.Show(i_bShow);
			l_pAuiMgr->Update();
		}
	}
#endif
}
void tqtPaneMgr::Show(QWidget *i_pWindow, bool i_bShow)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			pane_info.Show(i_bShow);
			l_pAuiMgr->Update();
		}
	}
#endif
}

//---------------------------------------------------------------------------
// Return true if theis pane is visible currently
//---------------------------------------------------------------------------
bool tqtPaneMgr::IsVisible(const char* i_PaneName)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(wxString(i_PaneName, wxConvUTF8));
		if (pane_info.IsOk())
		{
			return pane_info.IsShown();
		}
	}
#endif
	return false;
}
bool tqtPaneMgr::IsVisible(QWidget *i_pWindow)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			return pane_info.IsShown();
		}
	}
#endif
	return false;
}

//---------------------------------------------------------------------------
// SetCaption - alter caption for pane
//---------------------------------------------------------------------------
void tqtPaneMgr::SetCaption(QWidget *i_pWindow, const std::string& i_Caption)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxAuiPaneInfo &pane_info = l_pAuiMgr->GetPane(i_pWindow);
		if (pane_info.IsOk())
		{
			pane_info.Caption(wxString(i_Caption.c_str(), wxConvUTF8));
			l_pAuiMgr->Update();
		}
	}
#endif
}

//---------------------------------------------------------------------------
// Resize the pane for the given window
//---------------------------------------------------------------------------
void tqtPaneMgr::ResizePane(QWidget *i_pWindow, int i_Width, int i_Height)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
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
#endif
}

//---------------------------------------------------------------------------
// A Perspective is a string that represents the configuration of panes.
//	Using these functions the layout can be saved and restored.
//---------------------------------------------------------------------------
bool tqtPaneMgr::GetCurrentPerspective(std::string& o_Perspective)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		wxString persp = l_pAuiMgr->SavePerspective();
		o_Perspective = persp.utf8_str();
		return true;
	}
#endif
	return false;
}

void tqtPaneMgr::SetCurrentPerspective(const std::string& i_Perspective)
{
#ifdef QT_FINISH_PORT
	if (l_pAuiMgr != NULL)
	{
		l_pAuiMgr->LoadPerspective(wxString(i_Perspective.c_str(), wxConvUTF8));
	}
#endif
}

#endif // USE_QT
