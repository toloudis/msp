/*****************************************************************************
**  wxMainForm.hpp
**
**     MainForm when using wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "wxMainForm.hpp"

#include "wxMRUManager.hpp"
#include "wxLayoutMgr.hpp"
#include "wxRenderCanvas.hpp"
#include "wxPropertyPanel.hpp"

#include "mspApp.hpp"
#include "mspModel.hpp"
#include "mspVersion.hpp"
#include "mspViewSettingsObject.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/pwx/pwxControlFactoryBase.hpp"
#include "ToolUIWx/pwx/pwxControlFactoryCustom.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "ToolUIWx/twx/twxMenuMgr.hpp"
//#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Menu_New = wxID_NEW,
    Menu_Open = wxID_OPEN,
    Menu_Save = wxID_SAVE,
    Menu_SaveAs = wxID_SAVEAS,
    Menu_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
 //   Menu_About = wxID_ABOUT,

 //   Menu_LoadAnim = wxID_HIGHEST+1
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(wxMainForm, wxFrame)
    EVT_MENU(Menu_New,  wxMainForm::OnNew)
    EVT_MENU(Menu_Open,  wxMainForm::OnOpen)
    EVT_MENU(Menu_Save,  wxMainForm::OnSave)
    EVT_MENU(Menu_SaveAs,  wxMainForm::OnSaveAs)
    EVT_MENU(Menu_Quit,  wxMainForm::OnQuit)
//    EVT_MENU(Menu_About, wxMainForm::OnAbout)
//    EVT_SIZE(wxMainForm::OnResize)
	EVT_IDLE(wxMainForm::OnIdle)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
wxMainForm::wxMainForm(const wxString& title)
: wxFrame(NULL, wxID_ANY, title),
	   m_pFramePanel(NULL),
	   m_pInitCanvas(NULL),
	   m_pRenderCanvas(NULL),
	   m_bInIdleCallback(false)
{
	this->SetSize(1000,800);

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    //wxMenu *helpMenu = new wxMenu;
    //helpMenu->Append(Menu_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Menu_New, _T("&New"), _T("New material"));
    fileMenu->Append(Menu_Open, _T("&Open"), _T("Load material file"));
    fileMenu->Append(Menu_Save, _T("&Save"), _T("Save material file"));
    fileMenu->Append(Menu_SaveAs, _T("Save as..."), _T("Save material file"));
	fileMenu->AppendSeparator();
 //   fileMenu->Append(Menu_LoadAnim, _T("Load &Animation"), _T("Load animation file"));
	//fileMenu->AppendSeparator();

    wxMenu *recentFilesMenu = new wxMenu;
	PushEventHandler(new wxMRUManager(recentFilesMenu, 5, fileMenu));
	fileMenu->AppendSubMenu(recentFilesMenu, _T("&Recent Files"), _T("Recently loaded files"));
	fileMenu->AppendSeparator();

    fileMenu->Append(Menu_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    //menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

//#if wxUSE_STATUSBAR
//    // create a status bar just for fun (by default with 1 pane only)
//    CreateStatusBar(2);
//    SetStatusText(_T("Terawatt using wxWidgets!"));
//#endif // wxUSE_STATUSBAR

	m_pFramePanel = new wxPanel( this );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	//m_pInitCanvas = new wxRenderCanvas( m_pFramePanel );
	//m_pInitCanvas->SetClientSize(32, 32);

	//int w = 0, h = 0;
	//this->GetClientSize(&w, &h);
    m_pRenderCanvas = new wxRenderCanvas( m_pFramePanel );
	//m_pRenderCanvas->SetSize(w, h);
	bSizer1->Add( m_pRenderCanvas, 1, wxEXPAND | wxALL, 0 );
	m_pFramePanel->SetSizer( bSizer1 );
	m_pFramePanel->Layout();

	// Initialize the twx package with some of the pointers we just created
	twxSystem::g_pMainForm = this;
	twxSystem::g_pMainMenu = menuBar;

	// Start the AUI manager (handles panes and docking)
	twxPaneMgr::Init();

	// Panel for Material Properties
	wxPropertyPanel *pProperties = new wxPropertyPanel(this);

	twxPaneMgr::AddPane(m_pFramePanel, 
		wxAuiPaneInfo().Name(wxT("RenderArea")).Caption(L"Render View").Layer(3).Right().Position(1).BestSize(300,300));
		//wxAuiPaneInfo().Name(wxT("RenderArea")).CenterPane().Show());

	// Create the timeline slider pane, hidden to start
	//m_pSliderPane = new wxSliderPane(this);
	////twxPaneMgr::AddPane(m_pSliderPane, wxAuiPaneInfo().Name("TimeSlider").Hide().Layer(2).Bottom().Floatable(false));
	//twxPaneMgr::AddPane(m_pSliderPane, wxAuiPaneInfo().Name(L"TimeSlider").Layer(2).Bottom().Floatable(false));

	wuiPackage::Initialize();

	// Initíalize property interface
	pwxControlMgr::Initialize();
	pwxControlMgr::AddControlFactory( new pwxControlFactoryBase() );
	pwxControlMgr::AddControlFactory( new pwxControlFactoryCustom() );

}

wxMainForm::~wxMainForm()
{
//	wuiPackage::DeInitialize();
	//pwxFormControlBuilder::ClearForm(m_pSettingsPanel);
	//delete m_pViewSettingsObject;
	
	if (twxSystem::g_pMainForm == this)
	{
		twxSystem::g_pMainForm = NULL;
		twxSystem::g_pMainMenu = NULL;
		twxPaneMgr::CleanUp();
	}

}

//wxRenderCanvas* wxMainForm::GetInitRenderWindow()
//{
//	return m_pInitCanvas;
//}
wxRenderCanvas* wxMainForm::GetRenderWindow()
{
	return m_pRenderCanvas;
}

// event handlers

void wxMainForm::OnNew(wxCommandEvent& WXUNUSED(i_Event))
{
	if (wxPropertyPanel::DialogInstance)
			wxPropertyPanel::DialogInstance->ClearProperties();
	guiCustomDocHandler::New();
}
void wxMainForm::OnOpen(wxCommandEvent& WXUNUSED(i_Event))
{
	if (wxPropertyPanel::DialogInstance)
			wxPropertyPanel::DialogInstance->ClearProperties();
	guiCustomDocHandler::Open();
}
void wxMainForm::OnSave(wxCommandEvent& WXUNUSED(i_Event))
{
	guiCustomDocHandler::Save();
}
void wxMainForm::OnSaveAs(wxCommandEvent& WXUNUSED(i_Event))
{
	guiCustomDocHandler::SaveAs();
}

void wxMainForm::OnQuit(wxCommandEvent& WXUNUSED(i_Event))
{
	if (guiCustomDocHandler::SaveIfDirty())
	{
		// save last layout of panes
		wxLayoutMgr::SaveLastLayout();

		if (wxPropertyPanel::DialogInstance)
			wxPropertyPanel::DialogInstance->ClearProperties();

		// true is to force the frame to close
		Close(true);
	}
}

void wxMainForm::OnAbout(wxCommandEvent& WXUNUSED(i_Event))
{
    wxMessageBox(wxString::Format(
                    _T("Using %s!\n")
                    _T("\n")
                    _T("This is the Terawatt simple viewer tool using wxWidgets\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About SimpleViewer"),
                 wxOK | wxICON_INFORMATION,
                 this);
}
//
//void wxMainForm::OnResize(wxSizeEvent& i_Event)
//{
//    // this is necessary to resize the panel
//   this->OnSize(i_Event);
//
//   // Resize our render canvas to fill the client area of the main window
//	if (m_pRenderCanvas)
//	{
//		int w = 0, h = 0;
//		this->GetClientSize(&w, &h);
//		if (w > 0 && h > 0)
//		{
//			m_pRenderCanvas->SetSize(w, h);	
//			mspApp::ResizeRender(w, h);
//		}
//	}
//}

void wxMainForm::OnIdle(wxIdleEvent& i_Event)
{	
	if (m_bInIdleCallback) 
		return;

	m_bInIdleCallback = true;

	if (mspApp::IsActive())
	{
		if (!docSingleDocumentMgr::IsLoading() &&
			!docSingleDocumentMgr::IsSaving())
		{
			mspApp::ThinkApp();
	
			render_3dWindow();
		}
	}

	i_Event.RequestMore();
	
	m_bInIdleCallback = false;
}

#endif // USE_WXWIDGETS
