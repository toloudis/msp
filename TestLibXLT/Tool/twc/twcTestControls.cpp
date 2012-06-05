/****************************************************************************\
**	twcTestControls.cpp
**
**	Tests custom controls for wxWidgets property controls
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcColorRGBEdit.hpp"
#include "ToolUIWx/twc/twcFilePicker.hpp"
#include "ToolUIWx/twc/twcHotKeyCtrl.hpp"
#include "ToolUIWx/twc/twcNumericUpDown.hpp"
#include "ToolUIWx/twc/twcRangedFloat.hpp"
#include "ToolUIWx/twc/twcVector3Edit.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"

#include <wx/filepicker.h>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void FloatEdit_ValueChanged(wxCommandEvent& event);
	void Vector3Edit_ValueChanged(wxCommandEvent& event);
	void FilePicker_ValueChanged(wxCommandEvent& event);

protected:
		wxStaticText* m_staticText1;
		twcFloatEdit* m_textCtrl1;
		wxStaticText* m_staticText2;
		//wxSlider* m_slider1;
		twcRangedFloat* m_slider1;
		wxStaticText* m_staticText3;
		wxCheckBox* m_checkBox1;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrl2;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl3;
		twcVector3Edit* m_vectorEdit1;
		twcColorRGBEdit* m_colorEdit1;
		twcNumericUpDown* m_numericUpDown1;
		twcHotKeyCtrl* m_hotkeyCtrl;
		twcFilePicker* m_filePickerCtrl;
private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
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
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Test of twc controls"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxVERTICAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Large named property"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	////m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	//m_textCtrl1 = new twcFloatEdit( this );
	////this->Connect( m_textCtrl1->GetId(), wxEVT_VALUE_CHANGED,
	////				wxCommandEventHandler(MyFrame::FloatEdit_ValueChanged) );
	//fgSizer1->Add( m_textCtrl1, 0, wxALL|wxEXPAND, 5 );

	m_numericUpDown1 = new twcNumericUpDown( this );
	fgSizer1->Add( m_numericUpDown1, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Short"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticText2, 0, wxALL, 5 );
	
//	m_slider1 = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_slider1 = new twcRangedFloat( this );	
	this->Connect( m_slider1->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(MyFrame::FloatEdit_ValueChanged) );
	fgSizer1->Add( m_slider1, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Check box"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBox1, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Category"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	m_staticText4->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	
	fgSizer1->Add( m_staticText4, 0, wxALL|wxEXPAND, 0 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	m_staticText8->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	
	fgSizer1->Add( m_staticText8, 0, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticText6, 0, wxALL, 5 );
	
	m_vectorEdit1 = new twcVector3Edit( this );
	fgSizer1->Add( m_vectorEdit1, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 5 );
	
	m_colorEdit1 = new twcColorRGBEdit( this );
	m_colorEdit1->SetValue( maFloatRGBA(0.3f, 0.5f, 0.7f, 1.0f) );
	this->Connect( m_colorEdit1->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(MyFrame::Vector3Edit_ValueChanged) );
	fgSizer1->Add( m_colorEdit1, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *pStaticText8 = new wxStaticText( this, wxID_ANY, wxT("Hot Key"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pStaticText8, 0, wxALL, 5 );

	m_hotkeyCtrl = new twcHotKeyCtrl( this );
	fgSizer1->Add( m_hotkeyCtrl, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *pStaticText9 = new wxStaticText( this, wxID_ANY, wxT("File chooser"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pStaticText9, 0, wxALL, 5 );

	m_filePickerCtrl = new twcFilePicker( this );
	this->Connect( m_filePickerCtrl->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(MyFrame::FilePicker_ValueChanged) );
	fgSizer1->Add( m_filePickerCtrl, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

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
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& i_Event)
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This is the minimal wxWidgets sample\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About wxWidgets minimal sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::FloatEdit_ValueChanged(wxCommandEvent& WXUNUSED(event))
{

}
void MyFrame::Vector3Edit_ValueChanged(wxCommandEvent& WXUNUSED(event))
{

}
void MyFrame::FilePicker_ValueChanged(wxCommandEvent& WXUNUSED(event))
{
	std::string filename = itStringUtil::GetStdString(m_filePickerCtrl->GetFilename());
	std::string fullpath;
	fsFileUtil::LocatorToANSIFilename(m_filePickerCtrl->GetFullpath(), fullpath);
	DBG_LOG2("filepicker: %s fullpath: %s", filename.c_str(), fullpath.c_str());
}
