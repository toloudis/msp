/*****************************************************************************
**  wxMainForm.hpp
**
**     MainForm when using wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/wxGUI/wxMainForm.hpp"
#include "MainApp/wxGUI/wxMRUManager.hpp"
#include "MainApp/wxGUI/wxTimeSlider.hpp"
#include "MainApp/mnmApp.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "AudioDS/Sn/snSoundManager.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/prty/prtyFlags.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "ToolUIWx/twc/twcStatusBar.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxMessaging.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"

#include <sstream>

#ifdef USE_WXWIDGETS

namespace
{
	// ----------------------------------------------------------------------------
	// constants
	// ----------------------------------------------------------------------------

	// IDs for the controls and the menu commands
	enum
	{
		// menu items
		Minimal_New = wxID_NEW,
		Minimal_Open = wxID_OPEN,
		Minimal_Quit = wxID_EXIT,

		// it is important for the id corresponding to the "About" command to have
		// this standard value as otherwise it won't be handled properly under Mac
		// (where it is special and put into the "Apple" menu)
		Minimal_About = wxID_ABOUT
	};

}	// end of namespace

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(wxMainForm, wxFrame)
//    EVT_MENU(Minimal_New,  wxMainForm::OnNew)
//    EVT_MENU(Minimal_Open,  wxMainForm::OnOpen)
//    EVT_MENU(Minimal_Quit,  wxMainForm::OnQuit)
//    EVT_MENU(Minimal_About, wxMainForm::OnAbout)
	EVT_CLOSE(wxMainForm::OnClose)
    EVT_SIZE(wxMainForm::OnResize)
	EVT_IDLE(wxMainForm::OnIdle)
	EVT_KEY_UP(wxMainForm::OnKeyUp)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

//--------------------------------------------------------------------
// frame constructor
//--------------------------------------------------------------------
wxMainForm::wxMainForm(const wxString& title)
: wxFrame(NULL, wxID_ANY, title),
	   m_pRenderGrid(NULL),
	   m_bInIdleCallback(false),
	   m_pMRUManager(NULL)
{
	this->SetSize(800,600);

    // set the frame icon
    SetIcon(wxICON(sample));
    
	// create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    //wxMenu *helpMenu = new wxMenu;
    //helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

//    fileMenu->Append(Minimal_New, _T("&New"), _T("New model"));
//    fileMenu->Append(Minimal_Open, _T("&Open"), _T("Load model file"));
//	fileMenu->AppendSeparator();

    wxMenu *recentFilesMenu = new wxMenu;
	
    //fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));
	
	//initialize the MRUManager for the wx setup, initialzed to 0.
	m_pMRUManager = new wxMRUManager(recentFilesMenu, 0, fileMenu);
	PushEventHandler(m_pMRUManager);
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
	
    //menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar
	const int c_StatusPaneWidths[] = {
		100, 	// Mode
		-1, 	// Messages
		40, 	// Key
		40, 	// Mute
		40, 	// Particles
		80, 	// Camera
		80, 	// Render Flags
		60, 	// Subdiv
		100,	// Memory
	};
	const int c_NumStatusPanes = 9;
    wxStatusBar *statusBar = CreateStatusBar(c_NumStatusPanes);
	statusBar->SetStatusWidths(c_NumStatusPanes, c_StatusPaneWidths);
	SetStatusBarPane(1); // second pane of variable width contains messages
    //SetStatusText(_T("using wxWidgets!"), 1);

	// Setup panel for the render areas, which is placed
	// inside an empty content pane in order to allow the
	// render panels to preserve aspect ratio and maximum size:
	m_pContentPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pContentPaneSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pRenderGrid = new rpnPanelGrid( m_pContentPane );
	m_pContentPaneSizer->Add( m_pRenderGrid, 1, wxSHAPED, 0 );
	m_pContentPane->SetSizer(m_pContentPaneSizer);
	m_pContentPane->Layout();
	twxMessaging::WindowWantsHotKeys(m_pContentPane);

	// Initialize the twx package with some of the pointers we just created
	twxSystem::g_pMainForm = this;
	twxSystem::g_pMainMenu = menuBar;
	twxSystem::g_nNumMenuOffset = 2;
	twxSystem::g_pStatusBar = statusBar;

	// Start the AUI manager (handles panes and docking)
	twxPaneMgr::Init();
	twxPaneMgr::AddPane(m_pContentPane, 
		wxAuiPaneInfo().Name(wxT("RenderArea")).CenterPane().Show());

	// Initialize the wxWidgets implementation of Tool/gui
	wuiPackage::Initialize();

	//	set the functions to handle menu items being checked and
	//	checking if hot keys are valid here.
	cmaCommandMgr::SetMenuCheckedFunction(guiMenuMgr::MenuObjectsCheck);
	cmaCommandMgr::SetValidHotKeyFunction(guiMenuMgr::IsValidShortcut);

	//	Create some menu items so we control the order
	//
	//	TODO uncomment the menu items when they are completely command driven
	//
	//guiMenuMgr::AddMenuItem("File","");
	
	//now set the MRU list to hold the number that is defined in the prefs data field
	PrefsData &data = PrefsMgr::Data();
	int numItems = data.m_MRUHistory.GetValue();
	m_pMRUManager = new wxMRUManager(recentFilesMenu, numItems, fileMenu);
	
	fileMenu->AppendSubMenu(recentFilesMenu, _T("&Recent Files"), _T("Recently loaded files"));
	fileMenu->AppendSeparator();
	
	guiMenuMgr::AddMenu("Edit","");
	guiMenuMgr::AddMenu("View","");
	guiMenuMgr::AddMenu("Tools","");
	guiMenuMgr::AddMenu("Actions","");
	guiMenuMgr::AddMenu("Modes","");
	guiMenuMgr::AddMenu("Objects","");
	guiMenuMgr::AddMenu("Windows","");
	guiMenuMgr::AddMenu("Help","");

	// Set initial tooltips for some panels:
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mode, "Current Mode" );
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Particles, "Particles Inactive" );
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Key, "Auto Key: Off" );

	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Memory, "Video Memory: Used / Max" );
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Memory, "0 / 0" );

	// Create the time slider that will appear at the bottom of the main window
	m_pTimeSlider = new wxTimeSlider(this);
}

//--------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------
wxMainForm::~wxMainForm()
{
	if (twxSystem::g_pMainForm == this)
	{
		twxSystem::g_pMainForm = NULL;
		twxSystem::g_pMainMenu = NULL;
		twxSystem::g_pStatusBar = NULL;
		twxPaneMgr::CleanUp();
	}
	delete m_pMRUManager;
}

//--------------------------------------------------------------------
// Access to the render areas
//--------------------------------------------------------------------
rpnRenderPanel* wxMainForm::GetRenderWindow()
{
	return m_pRenderGrid->GetRenderPanel(0);
}
rpnRenderPanel* wxMainForm::GetRenderPane(int i_Index)
{
	return m_pRenderGrid->GetRenderPanel(i_Index);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void wxMainForm::ResizeRenderWindow(int i_Width, int i_Height)
{
	if ( twxSystem::g_pMainForm && (i_Width > 0) && (i_Height > 0) )
	{
		wxMainForm *pMainForm = dynamic_cast<wxMainForm*>(twxSystem::g_pMainForm);
		if (pMainForm)
		{
			wxSize form_size = pMainForm->GetSize();

			// Because the render panel sits in the panel grid with a 1 pixel border,
			// the actual render window client size is still 2 pixels smaller
			// than what is returned here. Offset it in the math.
			int grid_width = i_Width + 2;
			int grid_height = i_Height + 2;
			wxSize current_size = pMainForm->m_pContentPane->GetClientSize();
			//wxSize current_size = pMainForm->m_pRenderGrid->GetClientSize();

			// Setup maximum size and aspect ratio in the sizers first,
			// then apply the new size
			pMainForm->m_pRenderGrid->SetMaxRenderSize(grid_width, grid_height);
			pMainForm->m_pContentPaneSizer->GetItem((size_t)0)->SetRatio(grid_width, grid_height);

			int delta_width = grid_width - current_size.GetWidth();
			int delta_height = grid_height - current_size.GetHeight();

			pMainForm->SetSize(form_size.GetWidth() + delta_width,
							   form_size.GetHeight() + delta_height);
		}
	}
}

//--------------------------------------------------------------------
// Exit the application
//--------------------------------------------------------------------
//static
void wxMainForm::Exit()
{
	twxSystem::g_pMainForm->Close();
}

//--------------------------------------------------------------------
//	read the prefs and apply them where they need to go
//--------------------------------------------------------------------
void wxMainForm::prefs_ReadAndApply()
{
	//	get subdiv level + display
	int level = api3dSubdiv::GetSubdivLevel();
	char levelstr[8];
	sprintf(levelstr, "DIV%1d", level);
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Subdiv, levelstr );
	std::string tool_tip("Subdivision Level: ");
	tool_tip += ((level == 0) ? "Base Mesh" : levelstr);
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Subdiv, tool_tip.c_str());

	//	display the render prefs data in the status bar
	std::string rpstring = rndrPrefsMgr::GetPrefsString(rndrPrefsMgr::e_ViewportPrefs);
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Render_Flags, rpstring.c_str() );
	tool_tip = rndrPrefsMgr::GetPrefsStringLong(rndrPrefsMgr::e_ViewportPrefs);
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Render_Flags, tool_tip.c_str() );

	//	Preferences
	PrefsMgr::ReadPrefs();
	PrefsData prefsdata = PrefsMgr::Data();

	//	Hot Keys
	PrefsMgr::ReadHotKeys();

	PrefsDialogUtil::CreateDialog();

	//
	//	take the data and apply it where it is supposed to go.
	//

	//	render window size
	ResizeRenderWindow( prefsdata.m_RenderWindowWidth.GetValue(), prefsdata.m_RenderWindowHeight.GetValue() );

	//	Undo limits
	undoUndoMgr::SetLimits( prefsdata.m_UndoMemory.GetValue(), prefsdata.m_UndoLevels.GetValue() );

	//	Audio
	snSoundManager::Mute( prefsdata.m_bMuteAudio.GetValue() );
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mute, prefsdata.m_bMuteAudio.GetValue()?"Mute":"" );
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mute, 
			prefsdata.m_bMuteAudio.GetValue() ? "Audio: Off" : "Audio: On" );

	//	visuals
	//updateViewActionToolbar( prefsdata.m_bMainActionToolbarVisible.GetValue() );
	//updateViewModeToolbar( prefsdata.m_bMainModeToolbarVisible.GetValue() );
	//this->panelTime->Visible =  ( prefsdata.m_bMainTimelineVisible.GetValue() );
	//this->statusStripMain->Visible =  ( prefsdata.m_bMainStatusBarVisible.GetValue() );
	//resize_mainwindow();

	// axis compass
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_World, prefsdata.m_bAxisCompassVisible.GetValue() );

	//	property
	prtyFlags::SetShowToolTips( prefsdata.m_bShowToolTips.GetValue() );

	// get last layout of panes
	prefsLayoutMgr::LoadLastLayout();

	// If there is a default project and scene, then use that to set the paths
	if (!prefsdata.m_DefaultProjectName.GetValue().empty())
	{
		ProjectSetupMgr::SetProjectSceneDir(prefsdata.m_DefaultProjectName.GetValue(),
									    prefsdata.m_DefaultProjectDirectory.GetValue(),
									    prefsdata.m_DefaultSceneName.GetValue());
		guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
		cmmSystemDialogUtil::UpdateDialog();
	}

}
//---------------------------------------------------------------------------
//	write the prefs on closing the app
//---------------------------------------------------------------------------
void wxMainForm::prefs_UpdateAndWrite()
{
	PrefsData& prefsdata = PrefsMgr::Data();

	////	render window size
	//int wnd_width = -1, wnd_height = -1;
	//m_pRenderGrid->GetMaxRenderSize(wnd_width, wnd_height);
	//if ((wnd_width > 0) && (wnd_height > 0))
	//{
	//	prefsdata.m_RenderWindowHeight	= wnd_height;
	//	prefsdata.m_RenderWindowWidth	= wnd_width;
	//}

	//	visuals - saved in layouts now
	//prefsdata.m_bMainActionToolbarVisible.SetValue( IsViewActionToolbar() );
	//prefsdata.m_bMainModeToolbarVisible.SetValue( IsViewModeToolbar() );
	//prefsdata.m_bMainTimelineVisible.SetValue( this->panelTime->Visible );
	//prefsdata.m_bMainStatusBarVisible.SetValue( this->statusStripMain->Visible );

	//	axis compass
	prefsdata.m_bAxisCompassVisible.SetValue( cmpsCompassMgr::GetRenderable(cmpsCompassMgr::e_World) );

	//	property
	prefsdata.m_bShowToolTips.SetValue( prtyFlags::IsShowToolTips() );

	//
	PrefsMgr::WritePrefs();

	// write hot keys also
	PrefsMgr::WriteHotKeys();
}

//--------------------------------------------------------------------
// Overriding this function allows us to create a status bar
//	of our own type
//--------------------------------------------------------------------
//virtual 
wxStatusBar* wxMainForm::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
	//wxStatusBar *pStatusBar = new wxStatusBar(this, id, style, name);
	wxStatusBar *pStatusBar = new twcStatusBar(this, id, style, name);
	pStatusBar->SetFieldsCount(number);
	return pStatusBar;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxMainForm::OnClose(wxCloseEvent& i_Event)
{
	if (i_Event.CanVeto() &&
		!guiSingleDocHandler::SaveIfDirty())
	{
		i_Event.Veto();
	}
	else
	{
		// Stop the Idle callbacks
		mnmApp::SetActive(false);

		// save last layout of panes
		prefsLayoutMgr::SaveLastLayout();
		
		//	store last render window size into preferences
		int wnd_width = -1, wnd_height = -1;
		m_pRenderGrid->GetMaxRenderSize(wnd_width, wnd_height);
		if ((wnd_width > 0) && (wnd_height > 0))
		{
			// The render grid has a one pixel buffer around the actual
			// render area, so subtract 2 from width and height
			PrefsMgr::Data().m_RenderWindowHeight	= wnd_height-2;
			PrefsMgr::Data().m_RenderWindowWidth	= wnd_width-2;
		}

		//force the frame to close, deleting the window
		twxSystem::g_pMainForm->Destroy();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxMainForm::OnResize(wxSizeEvent& i_Event)
{
    // this is necessary to resize the panel
   this->OnSize(i_Event);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxMainForm::OnIdle(wxIdleEvent& i_Event)
{	
	if (m_bInIdleCallback) 
		return;

	m_bInIdleCallback = true;

	if (mnmApp::IsActive())
	{
		if (!docSingleDocumentMgr::IsLoading() &&
			!docSingleDocumentMgr::IsSaving())
		{
			mnmApp::ThinkApp();

			//update memory status
			int val = mnmApp::QueryVideoMemory();
			int max = mnmApp::QueryMaxVideoMemory();
			static char string[256];
			sprintf( string, "%dM / %dM", (max - val) / 1000000, max / 1000000 );
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Memory, string );
		}
	}
	
	i_Event.RequestMore();

	m_bInIdleCallback = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxMainForm::OnKeyUp(wxKeyEvent& i_Event)
{	
	// Handle hot keys here
	//twxMessaging::ProcessKeyEvent(i_Event);
	i_Event.Skip();

}

#endif // USE_WXWIDGETS
