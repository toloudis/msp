///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "cptrRenderBatchDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cptrRenderBatchDialogBase::cptrRenderBatchDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer_Main;
	bSizer_Main = new wxBoxSizer( wxVERTICAL );
	
	m_staticText_instructions = new wxStaticText( this, wxID_ANY, wxT("Add scenes and save as a batch.  Saved batches can be loaded later."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_instructions->Wrap( -1 );
	bSizer_Main->Add( m_staticText_instructions, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer_Scenes;
	bSizer_Scenes = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkList_ScenesChoices;
	m_checkList_Scenes = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList_ScenesChoices, 0 );
	bSizer_Scenes->Add( m_checkList_Scenes, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer_SceneButtons;
	bSizer_SceneButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_AddScene = new wxButton( this, wxID_ANY, wxT("Add Scene"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_SceneButtons->Add( m_button_AddScene, 0, wxALL, 5 );
	
	m_button_AddCurrent = new wxButton( this, wxID_ANY, wxT("Add Current"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_SceneButtons->Add( m_button_AddCurrent, 0, wxALL, 5 );
	
	m_button_DeleteScene = new wxButton( this, wxID_ANY, wxT("Del Scene"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_SceneButtons->Add( m_button_DeleteScene, 0, wxALL, 5 );

	m_button_MoveUp = new wxButton( this, wxID_ANY, wxT("Move Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_SceneButtons->Add( m_button_MoveUp, 0, wxALL, 5 );

	m_button_MoveDown = new wxButton( this, wxID_ANY, wxT("Move Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_SceneButtons->Add( m_button_MoveDown, 0, wxALL, 5 );

	bSizer_Scenes->Add( bSizer_SceneButtons, 0, wxALIGN_CENTER, 5 );

	wxBoxSizer* bSizer_BatchCheckBoxes;
	bSizer_BatchCheckBoxes = new wxBoxSizer( wxHORIZONTAL );

	m_checkBox_SkipScene = new wxCheckBox( this, wxID_ANY, wxT("Skip Scene When Error Occurs"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_BatchCheckBoxes->Add( m_checkBox_SkipScene, 0, wxALL, 5 );

	bSizer_Scenes->Add( bSizer_BatchCheckBoxes, 0, wxALIGN_CENTER, 5 );
	
	
	bSizer_Main->Add( bSizer_Scenes, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer_BatchFile;
	bSizer_BatchFile = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer_BatchFile;
	sbSizer_BatchFile = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Batch File") ), wxVERTICAL );
	
	m_textCtrl_BatchFile = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer_BatchFile->Add( m_textCtrl_BatchFile, 0, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* gSizer_BatchFileButtons;
	gSizer_BatchFileButtons = new wxGridSizer( 2, 2, 0, 0 );
	
	m_button_LoadBatch = new wxButton( this, wxID_ANY, wxT("Load Batch"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer_BatchFileButtons->Add( m_button_LoadBatch, 0, wxALL, 5 );
	
	m_button_SaveBatch = new wxButton( this, wxID_ANY, wxT("Save Batch"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer_BatchFileButtons->Add( m_button_SaveBatch, 0, wxALL, 5 );
	
	sbSizer_BatchFile->Add( gSizer_BatchFileButtons, 0, wxALIGN_CENTER, 5 );
	
	bSizer_BatchFile->Add( sbSizer_BatchFile, 1, wxEXPAND, 5 );
	
	bSizer_Main->Add( bSizer_BatchFile, 0, wxEXPAND, 5 );
	
	m_button_Continue = new wxButton( this, wxID_ANY, wxT("Render"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer_Main->Add( m_button_Continue, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer_Main->SetSizeHints(this);
	this->SetSizer( bSizer_Main );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( cptrRenderBatchDialogBase::OnClose ) );
	m_checkList_Scenes->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::checkList_Scenes_OnCheckListBoxDClick ), NULL, this );
	m_checkList_Scenes->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( cptrRenderBatchDialogBase::checkList_Scenes_OnCheckListBoxToggled ), NULL, this );
	m_button_AddScene->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_AddScene_OnButtonClick ), NULL, this );
	m_button_MoveUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_MoveUp_OnButtonClick ), NULL, this );
	m_button_MoveDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_MoveDown_OnButtonClick ), NULL, this );
	m_button_AddCurrent->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_AddCurrent_OnButtonClick ), NULL, this );
	m_button_DeleteScene->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_DeleteScene_OnButtonClick ), NULL, this );
	m_textCtrl_BatchFile->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( cptrRenderBatchDialogBase::textCtrl_BatchFile_OnTextEnter ), NULL, this );
	m_button_LoadBatch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_LoadBatch_OnButtonClick ), NULL, this );
	m_button_SaveBatch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_SaveBatch_OnButtonClick ), NULL, this );
	m_button_Continue->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_Continue_OnButtonClick ), NULL, this );
}

cptrRenderBatchDialogBase::~cptrRenderBatchDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( cptrRenderBatchDialogBase::OnClose ) );
	m_checkList_Scenes->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::checkList_Scenes_OnCheckListBoxDClick ), NULL, this );
	m_checkList_Scenes->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( cptrRenderBatchDialogBase::checkList_Scenes_OnCheckListBoxToggled ), NULL, this );
	m_button_AddScene->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_AddScene_OnButtonClick ), NULL, this );
	m_button_MoveUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_MoveUp_OnButtonClick ), NULL, this );
	m_button_MoveDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_MoveDown_OnButtonClick ), NULL, this );
	m_button_AddCurrent->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_AddCurrent_OnButtonClick ), NULL, this );
	m_button_DeleteScene->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_DeleteScene_OnButtonClick ), NULL, this );
	m_textCtrl_BatchFile->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( cptrRenderBatchDialogBase::textCtrl_BatchFile_OnTextEnter ), NULL, this );
	m_button_LoadBatch->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_LoadBatch_OnButtonClick ), NULL, this );
	m_button_SaveBatch->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_SaveBatch_OnButtonClick ), NULL, this );
	m_button_Continue->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBatchDialogBase::button_Continue_OnButtonClick ), NULL, this );
}

#endif
