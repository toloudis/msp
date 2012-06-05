///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cmmObjectDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cmmObjectDialogBase::cmmObjectDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer_Dialog;
	sizer_Dialog = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizer_Name;
	sizer_Name = new wxBoxSizer( wxHORIZONTAL );
	
	sizer_Name->SetMinSize( wxSize( -1,20 ) ); 
	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sizer_Name->Add( m_staticText1, 0, wxALL, 5 );
	
	m_label_Name = new wxStaticText( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_label_Name->Wrap( -1 );
	sizer_Name->Add( m_label_Name, 0, wxALL, 5 );
	
	sizer_Dialog->Add( sizer_Name, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );
	
	m_notebook1 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT );
	m_tabPage_Properties = new wxScrolledWindow( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_tabPage_Properties->SetScrollRate( 0, 5 );
	m_notebook1->AddPage( m_tabPage_Properties, wxT("Properties"), false, wxNullBitmap );
	m_tabPage_Drivers = new wxPanel( m_notebook1, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer_Drivers;
	sizer_Drivers = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_Drivers = new wxTreeCtrl( m_tabPage_Drivers, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	sizer_Drivers->Add( m_treeCtrl_Drivers, 1, wxEXPAND, 5 );
	
	m_button_AttachDriver = new wxButton( m_tabPage_Drivers, ID_DEFAULT, wxT("Attach Driver"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_AttachDriver->SetMinSize( wxSize( -1,30 ) );
	
	sizer_Drivers->Add( m_button_AttachDriver, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_tabPage_Drivers->SetSizer( sizer_Drivers );
	m_tabPage_Drivers->Layout();
	sizer_Drivers->Fit( m_tabPage_Drivers );
	m_notebook1->AddPage( m_tabPage_Drivers, wxT("Drivers"), true, wxNullBitmap );
	
	sizer_Dialog->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizer_Description;
	sizer_Description = new wxBoxSizer( wxHORIZONTAL );
	
	sizer_Description->SetMinSize( wxSize( -1,40 ) ); 
	m_staticText3 = new wxStaticText( this, ID_DEFAULT, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sizer_Description->Add( m_staticText3, 0, wxALL, 5 );
	
	m_labelDescription = new wxStaticText( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelDescription->Wrap( -1 );
	sizer_Description->Add( m_labelDescription, 0, wxALL, 5 );
	
	sizer_Dialog->Add( sizer_Description, 0, wxEXPAND|wxRIGHT, 5 );
	
	this->SetSizer( sizer_Dialog );
	this->Layout();
	
	// Connect Events
	m_treeCtrl_Drivers->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmObjectDialogBase::treeCtrl_Drivers_DoubleClick ), NULL, this );
	m_button_AttachDriver->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmObjectDialogBase::button_AddDriver_Click ), NULL, this );
}

cmmObjectDialogBase::~cmmObjectDialogBase()
{
	// Disconnect Events
	m_treeCtrl_Drivers->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmObjectDialogBase::treeCtrl_Drivers_DoubleClick ), NULL, this );
	m_button_AttachDriver->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmObjectDialogBase::button_AddDriver_Click ), NULL, this );
}
