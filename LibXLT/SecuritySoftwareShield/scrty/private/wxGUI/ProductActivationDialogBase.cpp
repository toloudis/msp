///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ProductActivationDialogBase.h"

///////////////////////////////////////////////////////////////////////////

ProductActivationDialogBase::ProductActivationDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText_Serial = new wxStaticText( this, wxID_ANY, wxT("Enter the serial number to activate product"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_Serial->Wrap( -1 );
	bSizer1->Add( m_staticText_Serial, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl_Serial = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_textCtrl_Serial, 0, wxALL|wxEXPAND, 5 );
	
	m_button_ActivateSerial = new wxButton( this, wxID_ANY, wxT("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button_ActivateSerial, 0, wxALIGN_CENTER|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_button_ActivateSerial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProductActivationDialogBase::button_ActivateSerial_OnButtonClick ), NULL, this );
}

ProductActivationDialogBase::~ProductActivationDialogBase()
{
	// Disconnect Events
	m_button_ActivateSerial->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProductActivationDialogBase::button_ActivateSerial_OnButtonClick ), NULL, this );
}
