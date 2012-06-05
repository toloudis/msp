///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "SceneSetupDialogBase.h"

///////////////////////////////////////////////////////////////////////////

SceneSetupDialogBase::SceneSetupDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_tabPage_Project = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( m_tabPage_Project, wxID_ANY, wxT("What Project is this for?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer61->Add( m_staticText4, 0, wxALL, 5 );
	
	m_listBox_Projects = new wxListBox( m_tabPage_Project, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer61->Add( m_listBox_Projects, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_tabPage_Project, wxID_ANY, wxT("Project Data") ), wxVERTICAL );
	
	m_staticText5 = new wxStaticText( m_tabPage_Project, wxID_ANY, wxT("Project Name (this will be the filename too)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	sbSizer2->Add( m_staticText5, 0, wxALL, 5 );
	
	m_textCtrl_ProjectName = new wxTextCtrl( m_tabPage_Project, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_textCtrl_ProjectName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( m_tabPage_Project, wxID_ANY, wxT("Project Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	sbSizer2->Add( m_staticText6, 0, wxALL, 5 );
	
	m_textCtrl_ProjectDescription = new wxTextCtrl( m_tabPage_Project, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_textCtrl_ProjectDescription, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( m_tabPage_Project, wxID_ANY, wxT("Project Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	sbSizer2->Add( m_staticText7, 0, wxALL, 5 );
	
	m_dirPicker_ProjectDir = new wxDirPickerCtrl( m_tabPage_Project, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	sbSizer2->Add( m_dirPicker_ProjectDir, 0, wxALL|wxEXPAND, 5 );
	
	bSizer61->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer71->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button_ProjectPrev = new wxButton( m_tabPage_Project, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_ProjectPrev->Enable( false );
	
	bSizer71->Add( m_button_ProjectPrev, 0, wxALL, 5 );
	
	m_button_ProjectNext = new wxButton( m_tabPage_Project, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button_ProjectNext, 0, wxALL, 5 );
	
	
	bSizer71->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer61->Add( bSizer71, 0, wxALL|wxEXPAND, 5 );
	
	m_tabPage_Project->SetSizer( bSizer61 );
	m_tabPage_Project->Layout();
	bSizer61->Fit( m_tabPage_Project );
	m_notebook1->AddPage( m_tabPage_Project, wxT("Project"), true );
	m_tabPage_Panel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( m_tabPage_Panel, wxID_ANY, wxT("Pick the scene"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer8->Add( m_staticText8, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox_Scenes = new wxListBox( m_tabPage_Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer9->Add( m_listBox_Scenes, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_button_RemoveScene = new wxButton( m_tabPage_Panel, wxID_ANY, wxT("Remove Scene"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_button_RemoveScene, 0, wxALL, 5 );
	
	m_button_ImportSceneList = new wxButton( m_tabPage_Panel, wxID_ANY, wxT("Import Scene\nList"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_button_ImportSceneList, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_tabPage_Panel, wxID_ANY, wxT("Scene Data") ), wxVERTICAL );
	
	m_staticText9 = new wxStaticText( m_tabPage_Panel, wxID_ANY, wxT("Scene Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	sbSizer3->Add( m_staticText9, 0, wxALL, 5 );
	
	m_textCtrl_SceneName = new wxTextCtrl( m_tabPage_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_textCtrl_SceneName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( m_tabPage_Panel, wxID_ANY, wxT("Scene Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	sbSizer3->Add( m_staticText10, 0, wxALL, 5 );
	
	m_textCtrl_SceneDescription = new wxTextCtrl( m_tabPage_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_textCtrl_SceneDescription, 0, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button_ScenePrev = new wxButton( m_tabPage_Panel, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button_ScenePrev, 0, wxALL, 5 );
	
	m_button_SceneNext = new wxButton( m_tabPage_Panel, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button_SceneNext, 0, wxALL, 5 );
	
	
	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer11, 0, wxALL|wxEXPAND, 5 );
	
	m_tabPage_Panel->SetSizer( bSizer8 );
	m_tabPage_Panel->Layout();
	bSizer8->Fit( m_tabPage_Panel );
	m_notebook1->AddPage( m_tabPage_Panel, wxT("Scene Manager"), false );
	m_tabPage_Summary = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_tabPage_Summary, wxID_ANY, wxT("Project-Scene-Summary") ), wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText_SummaryProject = new wxStaticText( m_tabPage_Summary, wxID_ANY, wxT("Project:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SummaryProject->Wrap( -1 );
	bSizer7->Add( m_staticText_SummaryProject, 0, wxALL, 5 );
	
	m_staticText_SummaryDirectory = new wxStaticText( m_tabPage_Summary, wxID_ANY, wxT("Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SummaryDirectory->Wrap( -1 );
	bSizer7->Add( m_staticText_SummaryDirectory, 0, wxALL, 5 );
	
	m_staticText_SummaryScene = new wxStaticText( m_tabPage_Summary, wxID_ANY, wxT("Scene:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SummaryScene->Wrap( -1 );
	bSizer7->Add( m_staticText_SummaryScene, 0, wxALL, 5 );
	
	sbSizer1->Add( bSizer7, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer1, 1, wxALL|wxEXPAND, 5 );
	
	m_checkBox_SetDefault = new wxCheckBox( m_tabPage_Summary, wxID_ANY, wxT("Set this scene as default"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_checkBox_SetDefault, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button_CreateDirs = new wxButton( m_tabPage_Summary, wxID_ANY, wxT("Create Directories"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_CreateDirs, 0, wxALIGN_CENTER, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button_Summary_Prev = new wxButton( m_tabPage_Summary, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button_Summary_Prev, 0, wxALL, 5 );
	
	m_button_Summary_Finish = new wxButton( m_tabPage_Summary, wxID_ANY, wxT("Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button_Summary_Finish, 0, wxALL, 5 );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer3->Add( bSizer6, 0, wxALL|wxEXPAND, 5 );
	
	m_tabPage_Summary->SetSizer( bSizer3 );
	m_tabPage_Summary->Layout();
	bSizer3->Fit( m_tabPage_Summary );
	m_notebook1->AddPage( m_tabPage_Summary, wxT("Summary"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_notebook1->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( SceneSetupDialogBase::OnNotebookPageChanged ), NULL, this );
	m_listBox_Projects->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SceneSetupDialogBase::listBox_Projects_SelIndexChanged ), NULL, this );
	m_button_ProjectNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ProjectNext_Click ), NULL, this );
	m_listBox_Scenes->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SceneSetupDialogBase::listBox_Scenes_SelIndexChanged ), NULL, this );
	m_button_RemoveScene->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_RemoveScene_Click ), NULL, this );
	m_button_ImportSceneList->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ImportSceneList_Click ), NULL, this );
	m_button_ScenePrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ScenePrev_Click ), NULL, this );
	m_button_SceneNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SceneNext_Click ), NULL, this );
	m_button_CreateDirs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_CreateDirs_Click ), NULL, this );
	m_button_Summary_Prev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SummaryPrev_Click ), NULL, this );
	m_button_Summary_Finish->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SummaryFinish_Click ), NULL, this );
}

SceneSetupDialogBase::~SceneSetupDialogBase()
{
	// Disconnect Events
	m_notebook1->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( SceneSetupDialogBase::OnNotebookPageChanged ), NULL, this );
	m_listBox_Projects->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SceneSetupDialogBase::listBox_Projects_SelIndexChanged ), NULL, this );
	m_button_ProjectNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ProjectNext_Click ), NULL, this );
	m_listBox_Scenes->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SceneSetupDialogBase::listBox_Scenes_SelIndexChanged ), NULL, this );
	m_button_RemoveScene->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_RemoveScene_Click ), NULL, this );
	m_button_ImportSceneList->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ImportSceneList_Click ), NULL, this );
	m_button_ScenePrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_ScenePrev_Click ), NULL, this );
	m_button_SceneNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SceneNext_Click ), NULL, this );
	m_button_CreateDirs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_CreateDirs_Click ), NULL, this );
	m_button_Summary_Prev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SummaryPrev_Click ), NULL, this );
	m_button_Summary_Finish->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneSetupDialogBase::button_SummaryFinish_Click ), NULL, this );
}

#endif
