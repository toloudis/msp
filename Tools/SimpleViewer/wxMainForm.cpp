/*****************************************************************************
**  wxMainForm.hpp
**
**     MainForm when using wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "wxMainForm.hpp"
#include "wxMRUManager.hpp"
#include "wxRenderCanvas.hpp"
#include "mnmApp.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"


#ifdef USE_WXWIDGETS

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

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(wxMainForm, wxFrame)
    EVT_MENU(Minimal_New,  wxMainForm::OnNew)
    EVT_MENU(Minimal_Open,  wxMainForm::OnOpen)
    EVT_MENU(Minimal_Quit,  wxMainForm::OnQuit)
    EVT_MENU(Minimal_About, wxMainForm::OnAbout)
    EVT_SIZE(wxMainForm::OnResize)
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
	this->SetSize(800,600);

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Minimal_New, _T("&New"), _T("New model"));
    fileMenu->Append(Minimal_Open, _T("&Open"), _T("Load model file"));
	fileMenu->AppendSeparator();

    wxMenu *recentFilesMenu = new wxMenu;
	PushEventHandler(new wxMRUManager(recentFilesMenu, 5, fileMenu));
	fileMenu->AppendSubMenu(recentFilesMenu, _T("&Recent Files"), _T("Recently loaded files"));
	fileMenu->AppendSeparator();

    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Terawatt using wxWidgets!"));
#endif // wxUSE_STATUSBAR

	m_pFramePanel = new wxPanel( this );

    m_pInitCanvas = new wxRenderCanvas( m_pFramePanel );
	m_pInitCanvas->SetClientSize(32, 32);

	int w = 0, h = 0;
	this->GetClientSize(&w, &h);
    m_pRenderCanvas = new wxRenderCanvas( m_pFramePanel );
	m_pRenderCanvas->SetSize(w, h);

	// Initialize the twx package with some of the pointers we just created
	twxSystem::g_pMainForm = this;
	twxSystem::g_pMainMenu = menuBar;
	wuiPackage::Initialize();
}

wxMainForm::~wxMainForm()
{
//	wuiPackage::DeInitialize();
}

wxRenderCanvas* wxMainForm::GetInitRenderWindow()
{
	return m_pInitCanvas;
}
wxRenderCanvas* wxMainForm::GetRenderWindow()
{
	return m_pRenderCanvas;
}

// event handlers

void wxMainForm::OnNew(wxCommandEvent& WXUNUSED(i_Event))
{
	guiCustomDocHandler::New();
}
void wxMainForm::OnOpen(wxCommandEvent& WXUNUSED(i_Event))
{
	guiCustomDocHandler::Open();
}
void wxMainForm::OnQuit(wxCommandEvent& WXUNUSED(i_Event))
{
	if (guiCustomDocHandler::SaveIfDirty())
	{
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

void wxMainForm::OnResize(wxSizeEvent& i_Event)
{
    // this is necessary to resize the panel
   this->OnSize(i_Event);

   // Resize our render canvas to fill the client area of the main window
	if (m_pRenderCanvas)
	{
		int w = 0, h = 0;
		this->GetClientSize(&w, &h);
		m_pRenderCanvas->SetSize(w, h);
	}
}

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
	
			render_3dWindow();
		}
	}
	
	m_bInIdleCallback = false;
}

#endif // USE_WXWIDGETS
