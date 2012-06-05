///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cmmSceneDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cmmSceneDialogBase::cmmSceneDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer_Dialog;
	sizer_Dialog = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook1 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT );
	m_tabPage_Available = new wxPanel( m_Notebook1, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer_Available;
	sizer_Available = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_Available = new wxTreeCtrl( m_tabPage_Available, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxTR_MULTIPLE );
	sizer_Available->Add( m_treeCtrl_Available, 3, wxEXPAND, 5 );
	
	wxBoxSizer* sizer_Available_ButtonBar;
	sizer_Available_ButtonBar = new wxBoxSizer( wxHORIZONTAL );
	
	sizer_Available_ButtonBar->SetMinSize( wxSize( -1,20 ) ); 
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer8->SetMinSize( wxSize( 40,-1 ) ); 
	m_button_Available_OpenAll = new wxButton( m_tabPage_Available, ID_DEFAULT, wxT("+"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer8->Add( m_button_Available_OpenAll, 0, wxALIGN_CENTER|wxALL, 1 );
	
	m_button_Available_CollapseAll = new wxButton( m_tabPage_Available, ID_DEFAULT, wxT("-"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer8->Add( m_button_Available_CollapseAll, 0, wxALIGN_CENTER|wxALL, 1 );
	
	m_button_Available_OpenPartial = new wxButton( m_tabPage_Available, ID_DEFAULT, wxT(":"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer8->Add( m_button_Available_OpenPartial, 0, wxALIGN_CENTER, 1 );
	
	
	bSizer8->Add( 4, 0, 1, wxALL, 5 );
	
	sizer_Available_ButtonBar->Add( bSizer8, 0, wxALIGN_CENTER|wxALIGN_LEFT, 3 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bpButton_Add = new wxBitmapButton( m_tabPage_Available, wxID_ANY, wxBitmap( wxT("./Data/Icons/available-add.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Add->SetDefault(); 
	m_bpButton_Add->SetToolTip( wxT("Add selected object to scene") );
	
	m_bpButton_Add->SetToolTip( wxT("Add selected object to scene") );
	
	bSizer9->Add( m_bpButton_Add, 0, wxALL, 2 );
	
	m_bpButton_Refresh = new wxBitmapButton( m_tabPage_Available, wxID_ANY, wxBitmap( wxT("./Data/Icons/available-refresh.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Refresh->SetToolTip( wxT("Refresh available list") );
	
	m_bpButton_Refresh->SetToolTip( wxT("Refresh available list") );
	
	bSizer9->Add( m_bpButton_Refresh, 0, wxALL, 2 );
	
	sizer_Available_ButtonBar->Add( bSizer9, 8, wxALIGN_RIGHT, 5 );
	
	sizer_Available->Add( sizer_Available_ButtonBar, 0, wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	m_tabPage_Available->SetSizer( sizer_Available );
	m_tabPage_Available->Layout();
	sizer_Available->Fit( m_tabPage_Available );
	m_Notebook1->AddPage( m_tabPage_Available, wxT("Available"), false, wxNullBitmap );
	m_tabPage_Placed = new wxPanel( m_Notebook1, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer_Placed;
	sizer_Placed = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_Placed = new wxTreeCtrl( m_tabPage_Placed, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	sizer_Placed->Add( m_treeCtrl_Placed, 3, wxEXPAND, 5 );
	
	wxBoxSizer* sizer_Placed_ButtonBar;
	sizer_Placed_ButtonBar = new wxBoxSizer( wxHORIZONTAL );
	
	sizer_Placed_ButtonBar->SetMinSize( wxSize( -1,20 ) ); 
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_Placed_OpenAll = new wxButton( m_tabPage_Placed, ID_DEFAULT, wxT("+"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer10->Add( m_button_Placed_OpenAll, 0, wxALIGN_LEFT, 1 );
	
	m_button_Placed_CollapseAll = new wxButton( m_tabPage_Placed, ID_DEFAULT, wxT("-"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer10->Add( m_button_Placed_CollapseAll, 0, wxALL, 1 );
	
	m_button_Placed_OpenPartial = new wxButton( m_tabPage_Placed, ID_DEFAULT, wxT(":"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	bSizer10->Add( m_button_Placed_OpenPartial, 0, wxALL, 1 );
	
	
	bSizer10->Add( 4, 0, 1, wxALL, 0 );
	
	sizer_Placed_ButtonBar->Add( bSizer10, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer5->SetMinSize( wxSize( -1,40 ) ); 
	
	m_bpButton_Edit = new wxBitmapButton( m_tabPage_Placed, wxID_ANY, wxBitmap( wxT("./Data/Icons/placed-edit.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Edit->SetDefault(); 
	m_bpButton_Edit->SetToolTip( wxT("Edit properties of selected objects") );
	
	m_bpButton_Edit->SetToolTip( wxT("Edit properties of selected objects") );
	
	bSizer5->Add( m_bpButton_Edit, 0, wxALL, 2 );
	
	m_bpButton_Duplicate = new wxBitmapButton( m_tabPage_Placed, wxID_ANY, wxBitmap( wxT("./Data/Icons/placed-duplicate.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Duplicate->SetToolTip( wxT("Duplicate selected object") );
	
	m_bpButton_Duplicate->SetToolTip( wxT("Duplicate selected object") );
	
	bSizer5->Add( m_bpButton_Duplicate, 0, wxALL, 2 );
	
	m_bpButton_Reload = new wxBitmapButton( m_tabPage_Placed, wxID_ANY, wxBitmap( wxT("./Data/Icons/placed-reload.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Reload->SetToolTip( wxT("Reload selected object") );
	
	m_bpButton_Reload->SetToolTip( wxT("Reload selected object") );
	
	bSizer5->Add( m_bpButton_Reload, 0, wxALL, 2 );
	
	m_bpButton_Delete = new wxBitmapButton( m_tabPage_Placed, wxID_ANY, wxBitmap( wxT("./Data/Icons/placed-delete.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton_Delete->SetToolTip( wxT("Delete selected object") );
	
	m_bpButton_Delete->SetToolTip( wxT("Delete selected object") );
	
	bSizer5->Add( m_bpButton_Delete, 0, wxALL, 2 );
	
	sizer_Placed_ButtonBar->Add( bSizer5, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 1 );
	
	sizer_Placed->Add( sizer_Placed_ButtonBar, 0, wxALIGN_BOTTOM|wxEXPAND, 1 );
	
	m_tabPage_Placed->SetSizer( sizer_Placed );
	m_tabPage_Placed->Layout();
	sizer_Placed->Fit( m_tabPage_Placed );
	m_Notebook1->AddPage( m_tabPage_Placed, wxT("Placed"), true, wxNullBitmap );
	
	sizer_Dialog->Add( m_Notebook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( sizer_Dialog );
	this->Layout();
	
	// Connect Events
	m_treeCtrl_Available->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Available_DoubleClick ), NULL, this );
	m_button_Available_OpenAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_OpenAll_Click ), NULL, this );
	m_button_Available_CollapseAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_CollapseAll_Click ), NULL, this );
	m_button_Available_OpenPartial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_OpenPartial_Click ), NULL, this );
	m_bpButton_Add->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Add_Click ), NULL, this );
	m_bpButton_Refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Refresh_Click ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_CHAR, wxKeyEventHandler( cmmSceneDialogBase::treeCtrl_Placed_CharPressed ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cmmSceneDialogBase::treeCtrl_Placed_LeftMouseDown ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( cmmSceneDialogBase::treeCtrl_Placed_LeftMouseUp ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Activated ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Selection ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Selecting ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_StateImageClick ), NULL, this );
	m_button_Placed_OpenAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_OpenAll_Click ), NULL, this );
	m_button_Placed_CollapseAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_CollapseAll_Click ), NULL, this );
	m_button_Placed_OpenPartial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_OpenPartial_Click ), NULL, this );
	m_bpButton_Edit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Edit_Click ), NULL, this );
	m_bpButton_Duplicate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Duplicate_Click ), NULL, this );
	m_bpButton_Reload->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Reload_Click ), NULL, this );
	m_bpButton_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Delete_Click ), NULL, this );
}

cmmSceneDialogBase::~cmmSceneDialogBase()
{
	// Disconnect Events
	m_treeCtrl_Available->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Available_DoubleClick ), NULL, this );
	m_button_Available_OpenAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_OpenAll_Click ), NULL, this );
	m_button_Available_CollapseAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_CollapseAll_Click ), NULL, this );
	m_button_Available_OpenPartial->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Available_OpenPartial_Click ), NULL, this );
	m_bpButton_Add->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Add_Click ), NULL, this );
	m_bpButton_Refresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Refresh_Click ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cmmSceneDialogBase::treeCtrl_Placed_CharPressed ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cmmSceneDialogBase::treeCtrl_Placed_LeftMouseDown ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( cmmSceneDialogBase::treeCtrl_Placed_LeftMouseUp ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Activated ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Selection ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_Selecting ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( cmmSceneDialogBase::treeCtrl_Placed_StateImageClick ), NULL, this );
	m_button_Placed_OpenAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_OpenAll_Click ), NULL, this );
	m_button_Placed_CollapseAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_CollapseAll_Click ), NULL, this );
	m_button_Placed_OpenPartial->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Placed_OpenPartial_Click ), NULL, this );
	m_bpButton_Edit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Edit_Click ), NULL, this );
	m_bpButton_Duplicate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Duplicate_Click ), NULL, this );
	m_bpButton_Reload->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Reload_Click ), NULL, this );
	m_bpButton_Delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmSceneDialogBase::button_Delete_Click ), NULL, this );
}
