///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "UndoHistoryDialogBase.h"

///////////////////////////////////////////////////////////////////////////

UndoHistoryDialogBase::UndoHistoryDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_listBox_UndoList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer4->Add( m_listBox_UndoList, 1, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 1, 2, 0, 0 );
	
	m_button_Undo = new wxButton( this, wxID_ANY, wxT("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer5->Add( m_button_Undo, 0, wxALL, 5 );
	
	m_button_Redo = new wxButton( this, wxID_ANY, wxT("Redo"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer5->Add( m_button_Redo, 0, wxALL, 5 );
	
	bSizer3->Add( gSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( UndoHistoryDialogBase::UndoHistoryDialog_OnActivate ) );
	m_button_Undo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UndoHistoryDialogBase::button_Undo_OnButtonClick ), NULL, this );
	m_button_Redo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UndoHistoryDialogBase::button_Redo_OnButtonClick ), NULL, this );
}

UndoHistoryDialogBase::~UndoHistoryDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( UndoHistoryDialogBase::UndoHistoryDialog_OnActivate ) );
	m_button_Undo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UndoHistoryDialogBase::button_Undo_OnButtonClick ), NULL, this );
	m_button_Redo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UndoHistoryDialogBase::button_Redo_OnButtonClick ), NULL, this );
}
