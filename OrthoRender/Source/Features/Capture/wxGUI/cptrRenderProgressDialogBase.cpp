///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cptrRenderProgressDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cptrRenderProgressDialogBase::cptrRenderProgressDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText_SceneNumber = new wxStaticText( m_panel1, wxID_ANY, wxT("Scene X of Y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SceneNumber->Wrap( -1 );
	bSizer3->Add( m_staticText_SceneNumber, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge_Scenes = new wxGauge( m_panel1, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer3->Add( m_gauge_Scenes, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText_SceneName = new wxStaticText( m_panel1, wxID_ANY, wxT("Scene Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SceneName->Wrap( -1 );
	bSizer3->Add( m_staticText_SceneName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText_CameraNumber = new wxStaticText( m_panel1, wxID_ANY, wxT("Camera X of Y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_CameraNumber->Wrap( -1 );
	bSizer3->Add( m_staticText_CameraNumber, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge_Cameras = new wxGauge( m_panel1, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer3->Add( m_gauge_Cameras, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText_CameraName = new wxStaticText( m_panel1, wxID_ANY, wxT("Camera Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_CameraName->Wrap( -1 );
	bSizer3->Add( m_staticText_CameraName, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge_Camera = new wxGauge( m_panel1, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer3->Add( m_gauge_Camera, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText_EstimatedTimeLeft = new wxStaticText( m_panel1, wxID_ANY, wxT("Est. Time Left  00:00:00.00"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_EstimatedTimeLeft->Wrap( -1 );
	bSizer3->Add( m_staticText_EstimatedTimeLeft, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_Pause = new wxButton( m_panel1, wxID_ANY, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_Pause, 0, wxALL, 5 );
	
	m_button_Save = new wxButton( m_panel1, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_Save, 0, wxALL, 5 );
	
	m_button_Abort = new wxButton( m_panel1, wxID_ANY, wxT("Abort"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_Abort, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer2->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
	bSizer2->Fit( this );
	
	// Connect Events
	m_button_Pause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Pause_OnButtonClick ), NULL, this );
	m_button_Save->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Save_OnButtonClick ), NULL, this );
	m_button_Abort->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Abort_OnButtonClick ), NULL, this );
}

cptrRenderProgressDialogBase::~cptrRenderProgressDialogBase()
{
	// Disconnect Events
	m_button_Pause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Pause_OnButtonClick ), NULL, this );
	m_button_Save->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Save_OnButtonClick ), NULL, this );
	m_button_Abort->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderProgressDialogBase::Button_Abort_OnButtonClick ), NULL, this );
}
