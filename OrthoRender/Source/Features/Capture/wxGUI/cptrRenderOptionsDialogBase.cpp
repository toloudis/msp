///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cptrRenderOptionsDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cptrRenderOptionsDialogBase::cptrRenderOptionsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook_Options = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_notebook_Options, 1, wxALL|wxEXPAND, 5 );
	
	m_button_Render = new wxButton( this, wxID_ANY, wxT("Render"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button_Render, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( cptrRenderOptionsDialogBase::cptrRenderOptionsDialog_OnClose ) );
	m_button_Render->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderOptionsDialogBase::button_Render_OnButtonClick ), NULL, this );
}

cptrRenderOptionsDialogBase::~cptrRenderOptionsDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( cptrRenderOptionsDialogBase::cptrRenderOptionsDialog_OnClose ) );
	m_button_Render->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderOptionsDialogBase::button_Render_OnButtonClick ), NULL, this );
}
