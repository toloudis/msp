///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "pythPythonDialogBase.h"

///////////////////////////////////////////////////////////////////////////

pythPythonDialogBase::pythPythonDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl_PythLog = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer1->Add( m_textCtrl_PythLog, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Python Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxLEFT, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrl_PythCommand = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer2->Add( m_textCtrl_PythCommand, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_button_ExecutePyth = new wxButton( this, wxID_ANY, wxT("Execute"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button_ExecutePyth, 0, wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_button_ClearPyth = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button_ClearPyth, 0, wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	bSizer2->Add( bSizer3, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_textCtrl_PythCommand->Connect( wxEVT_KEY_UP, wxKeyEventHandler( pythPythonDialogBase::textCtrl_PythCommand_KeyUp ), NULL, this );
	m_button_ExecutePyth->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( pythPythonDialogBase::button_ExecutePyth_Click ), NULL, this );
	m_button_ClearPyth->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( pythPythonDialogBase::button_ClearPyth_Click ), NULL, this );
}

pythPythonDialogBase::~pythPythonDialogBase()
{
	// Disconnect Events
	m_textCtrl_PythCommand->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( pythPythonDialogBase::textCtrl_PythCommand_KeyUp ), NULL, this );
	m_button_ExecutePyth->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( pythPythonDialogBase::button_ExecutePyth_Click ), NULL, this );
	m_button_ClearPyth->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( pythPythonDialogBase::button_ClearPyth_Click ), NULL, this );
}

#endif
