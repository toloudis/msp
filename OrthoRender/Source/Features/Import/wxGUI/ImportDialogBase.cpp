///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ImportDialogBase.h"

///////////////////////////////////////////////////////////////////////////

ImportDialogBase::ImportDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Root Scene/Shot file") ), wxVERTICAL );
	
	m_filePicker1 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a scene file to import"), wxT("*.mab"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	sbSizer2->Add( m_filePicker1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Root Scene/Shot Inventory List"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	m_treeCtrl_Import = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	bSizer1->Add( m_treeCtrl_Import, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button_Import = new wxButton( this, wxID_ANY, wxT("Import"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_Import, 0, wxALL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 1, wxALL, 3 );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 3 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_filePicker1->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ImportDialogBase::filePicker_FileChanged ), NULL, this );
	m_treeCtrl_Import->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ImportDialogBase::treeCtrl_LeftMouseDown ), NULL, this );
	m_button_Import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportDialogBase::buttonImport_Click ), NULL, this );
}

ImportDialogBase::~ImportDialogBase()
{
	// Disconnect Events
	m_filePicker1->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ImportDialogBase::filePicker_FileChanged ), NULL, this );
	m_treeCtrl_Import->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ImportDialogBase::treeCtrl_LeftMouseDown ), NULL, this );
	m_button_Import->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportDialogBase::buttonImport_Click ), NULL, this );
}
