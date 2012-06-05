///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ResourceTrackerDialogBase.h"

///////////////////////////////////////////////////////////////////////////

ResourceTrackerDialogBase::ResourceTrackerDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer_resources;
	bSizer_resources = new wxBoxSizer( wxVERTICAL );
	
	m_auinotebook_resources = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel_resources_flat = new wxPanel( m_auinotebook_resources, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer_flat;
	bSizer_flat = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_resources = new wxTreeCtrl( m_panel_resources_flat, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer_flat->Add( m_treeCtrl_resources, 1, wxEXPAND, 5 );
	
	m_panel_resources_flat->SetSizer( bSizer_flat );
	m_panel_resources_flat->Layout();
	bSizer_flat->Fit( m_panel_resources_flat );
	m_auinotebook_resources->AddPage( m_panel_resources_flat, wxT("Resources"), true, wxNullBitmap );
	m_panel_resources_hierarchy = new wxPanel( m_auinotebook_resources, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer_hierarchy;
	bSizer_hierarchy = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_hierarchy = new wxTreeCtrl( m_panel_resources_hierarchy, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer_hierarchy->Add( m_treeCtrl_hierarchy, 1, wxEXPAND, 5 );
	
	m_panel_resources_hierarchy->SetSizer( bSizer_hierarchy );
	m_panel_resources_hierarchy->Layout();
	bSizer_hierarchy->Fit( m_panel_resources_hierarchy );
	m_auinotebook_resources->AddPage( m_panel_resources_hierarchy, wxT("Hierarchy"), false, wxNullBitmap );
	
	bSizer_resources->Add( m_auinotebook_resources, 1, wxEXPAND | wxALL, 5 );
	
	m_button_refresh = new wxButton( this, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_resources->Add( m_button_refresh, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer_resources );
	this->Layout();
	
	// Connect Events
	m_button_refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceTrackerDialogBase::button_refresh_OnButtonClick ), NULL, this );
}

ResourceTrackerDialogBase::~ResourceTrackerDialogBase()
{
	// Disconnect Events
	m_button_refresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceTrackerDialogBase::button_refresh_OnButtonClick ), NULL, this );
}
