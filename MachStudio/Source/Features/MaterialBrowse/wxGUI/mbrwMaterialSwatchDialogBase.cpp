///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "mbrwMaterialSwatchDialogBase.h"

///////////////////////////////////////////////////////////////////////////

mbrwMaterialSwatchDialogBase::mbrwMaterialSwatchDialogBase( wxWindow* parent, 				   
										const wxString& i_IconDirectory,
										wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer_MaterialBrowse;
	bSizer_MaterialBrowse = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_Browse = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-open.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Browse->SetToolTip( wxT("Browse to directory") );
	//m_button_Browse->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-open_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Browse->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-open_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Browse->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-open_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Browse->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Browse, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Home = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-home.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Home->SetToolTip( wxT("Home directory") );
	m_button_Home->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-home_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Home->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-home_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Home->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-home_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Home->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Home, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Refresh = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-refresh.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Refresh->SetToolTip( wxT("Refresh Icons") );
	m_button_Refresh->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-refresh_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Refresh->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-refresh_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Refresh->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-refresh_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Refresh->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Refresh, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Back = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-back.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Back->SetToolTip( wxT("Previous Directory") );
	m_button_Back->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-back_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Back->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-back_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Back->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-back_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Back->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Back, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Forward = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-forward.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Forward->SetToolTip( wxT("Next Directory") );
	m_button_Forward->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-forward_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Forward->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-forward_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Forward->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-forward_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Forward->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Forward, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Up = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-up.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Up->SetToolTip( wxT("Parent Directory") );
	m_button_Up->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-up_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Up->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-up_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Up->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-up_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Up->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Up, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button_Paint = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-paint.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_Paint->SetToolTip( wxT("Apply to selected material") );
	m_button_Paint->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-paint_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Paint->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-paint_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Paint->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-paint_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_Paint->SetSize(30,30);
	bSizer_MaterialBrowse->Add( m_button_Paint, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_button_NewFolder = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\matbrowse-new.PNG"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_button_NewFolder->SetToolTip( wxT("Create a new folder") );
	m_button_NewFolder->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\matbrowse-new_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_NewFolder->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\matbrowse-new_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_NewFolder->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\matbrowse-new_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_button_NewFolder->SetSize(30,30);
	
	bSizer_MaterialBrowse->Add( m_button_NewFolder, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer_MaterialBrowse->Add( 0, 0, 1, wxEXPAND, 5 );

	wxString m_choice_viewChoices[] = { wxT("Thumbnails"), wxT("List") /*, wxT("Icon List")*/ };
	int m_choice_viewNChoices = sizeof( m_choice_viewChoices ) / sizeof( wxString );
	m_choice_view = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice_viewNChoices, m_choice_viewChoices, 0 );
	m_choice_view->SetSelection( 0 );
	bSizer_MaterialBrowse->Add( m_choice_view, 0, wxALL, 5 );
	
	m_panel1->SetSizer( bSizer_MaterialBrowse );
	m_panel1->Layout();
	bSizer_MaterialBrowse->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_listCtrl_Icons = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE|wxLC_ICON|wxLC_SINGLE_SEL );
	m_listCtrl_Icons->SetForegroundColour( wxColour( 255, 255, 255 ) );
	m_listCtrl_Icons->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	bSizer1->Add( m_listCtrl_Icons, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_button_Browse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonBrowse_Click ), NULL, this );
	m_button_Home->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonHome_Click ), NULL, this );
	m_button_Refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonRefresh_Click ), NULL, this );
	m_button_Back->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonBack_Click ), NULL, this );
	m_button_Forward->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonForward_Click ), NULL, this );
	m_button_Up->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonUp_Click ), NULL, this );
	m_button_Paint->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonPaint_Click ), NULL, this );
	m_button_NewFolder->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonNewFolder_Click ), NULL, this );
	m_listCtrl_Icons->Connect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_beginDrag ), NULL, this );
	m_listCtrl_Icons->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_doubleClick ), NULL, this );
	m_listCtrl_Icons->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_selectionChange ), NULL, this );
	m_listCtrl_Icons->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_selectionChange ), NULL, this );
	m_choice_view->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::OnChoice ), NULL, this );
}

mbrwMaterialSwatchDialogBase::~mbrwMaterialSwatchDialogBase()
{
	// Disconnect Events
	m_button_Browse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonBrowse_Click ), NULL, this );
	m_button_Home->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonHome_Click ), NULL, this );
	m_button_Refresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonRefresh_Click ), NULL, this );
	m_button_Back->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonBack_Click ), NULL, this );
	m_button_Forward->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonForward_Click ), NULL, this );
	m_button_Up->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonUp_Click ), NULL, this );
	m_button_Paint->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonPaint_Click ), NULL, this );
	m_button_NewFolder->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::buttonNewFolder_Click ), NULL, this );
	m_listCtrl_Icons->Disconnect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_beginDrag ), NULL, this );
	m_listCtrl_Icons->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_doubleClick ), NULL, this );
	m_listCtrl_Icons->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_selectionChange ), NULL, this );
	m_listCtrl_Icons->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( mbrwMaterialSwatchDialogBase::listCtrl_selectionChange ), NULL, this );
	m_choice_view->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mbrwMaterialSwatchDialogBase::OnChoice ), NULL, this );
}

#endif
