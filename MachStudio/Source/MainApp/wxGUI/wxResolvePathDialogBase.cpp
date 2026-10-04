///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "wxResolvePathDialogBase.h"

///////////////////////////////////////////////////////////////////////////

wxResolvePathDialogBase::wxResolvePathDialogBase( wxWindow* parent, 
												 wxWindowID id, 
												 const wxString& title, 
												 const wxPoint& pos, 
												 const wxSize& size, 
												 long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("File not found:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	m_staticText_Filename = new wxStaticText( this, wxID_ANY, wxT("Message goes here"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_Filename->Wrap( -1 );
	bSizer1->Add( m_staticText_Filename, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_Locate = new wxButton( this, wxID_ANY, wxT("Locate..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button_Locate, 0, wxALL, 5 );
	
	m_button_Retry = new wxButton( this, wxID_ANY, wxT("Retry"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button_Retry, 0, wxALL, 5 );
	
	m_button_Skip = new wxButton( this, wxID_ANY, wxT("Skip"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button_Skip, 0, wxALL, 5 );
	
	m_button_SkipAll = new wxButton( this, wxID_ANY, wxT("Skip All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button_SkipAll, 0, wxALL, 5 );
	
	m_button_Abort = new wxButton( this, wxID_ANY, wxT("Abort"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button_Abort, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	//this->Layout();
	// The default size is in DIPs: scale it for the display's DPI, and
	// never let the window be smaller than its contents.
	this->SetSize( FromDIP( size ) );
	this->SetMinClientSize( this->GetSizer()->GetMinSize() );
	
	// Connect Events
	m_button_Locate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonLocateClick ), NULL, this );
	m_button_Retry->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonRetryClick ), NULL, this );
	m_button_Skip->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonSkipClick ), NULL, this );
	m_button_SkipAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonSkipAllClick ), NULL, this );
	m_button_Abort->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonAbortClick ), NULL, this );
}

wxResolvePathDialogBase::~wxResolvePathDialogBase()
{
	// Disconnect Events
	m_button_Locate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonLocateClick ), NULL, this );
	m_button_Retry->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonRetryClick ), NULL, this );
	m_button_Skip->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonSkipClick ), NULL, this );
	m_button_SkipAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonSkipAllClick ), NULL, this );
	m_button_Abort->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxResolvePathDialogBase::buttonAbortClick ), NULL, this );
}

#endif
