///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "rdrLayersDialogBase.h"

///////////////////////////////////////////////////////////////////////////

rdrLayersDialogBase::rdrLayersDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer_Dialog;
	sizer_Dialog = new wxBoxSizer( wxVERTICAL );
	
	panel_Dialog = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* sizer_rdrLayer_Tree1;
	sizer_rdrLayer_Tree1 = new wxBoxSizer( wxVERTICAL );
	
	m_layerTree = new wxTreeCtrl( panel_Dialog, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS );
	sizer_rdrLayer_Tree1->Add( m_layerTree, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 1, 4, 0, 0 );
	
	m_addButton = new wxButton( panel_Dialog, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_addButton->SetToolTip( wxT("Add New Render Layer") );
	
	gSizer5->Add( m_addButton, 0, wxALL, 5 );
	
	m_duplicateButton = new wxButton( panel_Dialog, wxID_ANY, wxT("Duplicate"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_duplicateButton->SetFont( wxFont( 7, 70, 90, 90, false, wxEmptyString ) );
	m_duplicateButton->SetToolTip( wxT("Duplicate Render Layer") );
	
	gSizer5->Add( m_duplicateButton, 0, wxALL, 5 );
	
	m_deleteButton = new wxButton( panel_Dialog, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_deleteButton->SetToolTip( wxT("Delete Render Layer") );
	
	gSizer5->Add( m_deleteButton, 0, wxALL, 5 );
	
	m_renameButton = new wxButton( panel_Dialog, wxID_ANY, wxT("Rename"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_renameButton->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	m_renameButton->SetToolTip( wxT("Rename Render Layer") );
	
	gSizer5->Add( m_renameButton, 0, wxALL, 5 );
	
	sizer_rdrLayer_Tree1->Add( gSizer5, 0, 0, 5 );
	
	fgSizer2->Add( sizer_rdrLayer_Tree1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_layerNameText = new wxStaticText( panel_Dialog, wxID_ANY, wxT("Layer Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_layerNameText->Wrap( -1 );
	bSizer81->Add( m_layerNameText, 0, wxALL, 5 );
	
	m_layerTabPages = new wxAuiNotebook( panel_Dialog, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxAUI_NB_SCROLL_BUTTONS );
	
	bSizer81->Add( m_layerTabPages, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( bSizer81, 1, wxEXPAND, 5 );
	
	panel_Dialog->SetSizer( fgSizer2 );
	panel_Dialog->Layout();
	fgSizer2->Fit( panel_Dialog );
	sizer_Dialog->Add( panel_Dialog, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( sizer_Dialog );
	this->Layout();
	
	// Connect Events
	m_layerTree->Connect( wxEVT_CHAR, wxKeyEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeCharPressed ), NULL, this );
	m_layerTree->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeLeftMouseDown ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeBeginLabelEdit ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeDeleteItem ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeEndLabelEdit ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeItemActivated ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeKeyDown ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeSelChanged ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeSelChanging ), NULL, this );
	m_layerTree->Connect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeImageClick ), NULL, this );
	m_addButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_AddButtonClick ), NULL, this );
	m_duplicateButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_DuplicateButtonClick ), NULL, this );
	m_deleteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_DeleteButtonClick ), NULL, this );
	m_renameButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_RenameButtonClick ), NULL, this );
	m_layerTabPages->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler( rdrLayersDialogBase::rdrLayersDialog_TabPageChanged ), NULL, this );
	m_layerTabPages->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEventHandler( rdrLayersDialogBase::rdrLayersDialog_TabPageChanging ), NULL, this );
}

rdrLayersDialogBase::~rdrLayersDialogBase()
{
	// Disconnect Events
	m_layerTree->Disconnect( wxEVT_CHAR, wxKeyEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeCharPressed ), NULL, this );
	m_layerTree->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeLeftMouseDown ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeBeginLabelEdit ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeDeleteItem ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeEndLabelEdit ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeItemActivated ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeKeyDown ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeSelChanged ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeSelChanging ), NULL, this );
	m_layerTree->Disconnect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( rdrLayersDialogBase::rdrLayersDialog_TreeImageClick ), NULL, this );
	m_addButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_AddButtonClick ), NULL, this );
	m_duplicateButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_DuplicateButtonClick ), NULL, this );
	m_deleteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_DeleteButtonClick ), NULL, this );
	m_renameButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rdrLayersDialogBase::rdrLayersDialog_RenameButtonClick ), NULL, this );
	m_layerTabPages->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler( rdrLayersDialogBase::rdrLayersDialog_TabPageChanged ), NULL, this );
	m_layerTabPages->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEventHandler( rdrLayersDialogBase::rdrLayersDialog_TabPageChanging ), NULL, this );
}

#endif
