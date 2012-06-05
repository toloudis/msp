///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "cmmPlacedPaneBase.h"
#include "ToolUIWx/twc/twcTreeView.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itString.hpp"

///////////////////////////////////////////////////////////////////////////

cmmPlacedPaneBase::cmmPlacedPaneBase( wxWindow* parent,
										const wxString& i_IconDirectory,
										wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) 
: wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer_Placed;
	sizer_Placed = new wxBoxSizer( wxVERTICAL );
	
	//m_treeCtrl_Placed = new wxTreeCtrl( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	m_treeCtrl_Placed = new twcTreeView( this );
	sizer_Placed->Add( m_treeCtrl_Placed, 3, wxEXPAND, 5 );
	
	wxBoxSizer* sizer_Placed_ButtonBar;
	sizer_Placed_ButtonBar = new wxBoxSizer( wxHORIZONTAL );
	
	sizer_Placed_ButtonBar->SetMinSize( wxSize( -1,20 ) ); 
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_Placed_OpenAll = new wxButton( this, ID_DEFAULT, wxT("+"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	m_button_Placed_OpenAll->SetToolTip( wxT("Expand the List") );
	
	bSizer10->Add( m_button_Placed_OpenAll, 0, wxALIGN_LEFT|wxALL, 1 );
	
	m_button_Placed_CollapseAll = new wxButton( this, ID_DEFAULT, wxT("-"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	m_button_Placed_CollapseAll->SetToolTip( wxT("Contract the List") );
	
	bSizer10->Add( m_button_Placed_CollapseAll, 0, wxALL, 1 );
	
	m_button_Placed_OpenPartial = new wxButton( this, ID_DEFAULT, wxT(":"), wxDefaultPosition, wxSize( 16,16 ), wxBU_EXACTFIT );
	m_button_Placed_OpenPartial->SetToolTip( wxT("Expand only the selected item") );
	
	bSizer10->Add( m_button_Placed_OpenPartial, 0, wxALL, 1 );
	
	
	bSizer10->Add( 4, 0, 1, wxALL, 0 );
	
	sizer_Placed_ButtonBar->Add( bSizer10, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer5->SetMinSize( wxSize( -1,40 ) ); 
	m_bpButton_Edit = new wxBitmapButton( this, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\placed-edit.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_Edit->SetDefault(); 
	m_bpButton_Edit->SetToolTip( wxT("Edit properties of selected objects") );
	m_bpButton_Edit->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\placed-edit_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Edit->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\placed-edit_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Edit->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\placed-edit_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Edit->SetSize(30,30);
	bSizer5->Add( m_bpButton_Edit, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2 );
	
	m_bpButton_Duplicate = new wxBitmapButton( this, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\placed-duplicate.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_Duplicate->SetToolTip( wxT("Duplicate selected object") );
	m_bpButton_Duplicate->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\placed-duplicate_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Duplicate->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\placed-duplicate_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Duplicate->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\placed-duplicate_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Duplicate->SetSize(30,30);
	
	bSizer5->Add( m_bpButton_Duplicate, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2 );
	
	m_bpButton_Reload = new wxBitmapButton( this, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\placed-reload.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_Reload->SetToolTip( wxT("Reload selected object") );
	m_bpButton_Reload->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\placed-reload_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Reload->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\placed-reload_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Reload->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\placed-reload_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Reload->SetSize(30,30);
	
	bSizer5->Add( m_bpButton_Reload, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2 );
	
	m_bpButton_Delete = new wxBitmapButton( this, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\placed-delete.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_Delete->SetToolTip( wxT("Delete selected object") );
	m_bpButton_Delete->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\placed-delete_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Delete->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\placed-delete_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Delete->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\placed-delete_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Delete->SetSize(30,30);
	
	bSizer5->Add( m_bpButton_Delete, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2 );
	
	sizer_Placed_ButtonBar->Add( bSizer5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 1 );
	
	sizer_Placed->Add( sizer_Placed_ButtonBar, 0, wxALIGN_BOTTOM|wxEXPAND, 1 );
		
	this->SetSizer( sizer_Placed );
	this->Layout();
	
	// Connect Events
	m_treeCtrl_Placed->Connect( wxEVT_CHAR, wxKeyEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_CharPressed ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_BeginDrag ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_EndDrag ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_Activated ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_ContextMenu ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_RightClick ), NULL, this );
	m_button_Placed_OpenAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_OpenAll_Click ), NULL, this );
	m_button_Placed_CollapseAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_CollapseAll_Click ), NULL, this );
	m_button_Placed_OpenPartial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_OpenPartial_Click ), NULL, this );
	m_bpButton_Edit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Edit_Click ), NULL, this );
	m_bpButton_Duplicate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Duplicate_Click ), NULL, this );
	m_bpButton_Reload->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Reload_Click ), NULL, this );
	m_bpButton_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Delete_Click ), NULL, this );
}

cmmPlacedPaneBase::~cmmPlacedPaneBase()
{
	// Disconnect Events
	m_treeCtrl_Placed->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_CharPressed ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_BeginDrag ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_EndDrag ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_Activated ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_ContextMenu ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( cmmPlacedPaneBase::treeCtrl_Placed_RightClick ), NULL, this );
	m_button_Placed_OpenAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_OpenAll_Click ), NULL, this );
	m_button_Placed_CollapseAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_CollapseAll_Click ), NULL, this );
	m_button_Placed_OpenPartial->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Placed_OpenPartial_Click ), NULL, this );
	m_bpButton_Edit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Edit_Click ), NULL, this );
	m_bpButton_Duplicate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Duplicate_Click ), NULL, this );
	m_bpButton_Reload->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Reload_Click ), NULL, this );
	m_bpButton_Delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cmmPlacedPaneBase::button_Delete_Click ), NULL, this );
}

#endif
