///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "plbkPlaybackControlsDialogBase.h"

///////////////////////////////////////////////////////////////////////////

plbkPlaybackControlsDialogBase::plbkPlaybackControlsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer_panel;
	bSizer_panel = new wxBoxSizer( wxVERTICAL );
	
	m_panel_main = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 7, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bpButton_ToStart = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-begin.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_ToStart, 1, 0, 5 );
	
	m_bpButton_FastRev = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-fastrev.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_FastRev, 0, wxRIGHT|wxLEFT, 5 );
	
	m_bpButton_PlayBack = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-playrev.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_PlayBack, 0, wxRIGHT|wxLEFT, 5 );
	
	m_bpButton_Pause = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-pause.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_Pause, 0, wxRIGHT|wxLEFT, 5 );
	
	m_bpButton_PlayFwd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-playfwd.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_PlayFwd, 0, wxRIGHT|wxLEFT, 5 );
	
	m_bpButton_FastFwd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-fastfwd.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_FastFwd, 0, wxRIGHT|wxLEFT, 5 );
	
	m_bpButton_ToEnd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( wxT("./Data/Icons/playback-end.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer3->Add( m_bpButton_ToEnd, 0, wxLEFT, 5 );
	
	m_panel_main->SetSizer( fgSizer3 );
	m_panel_main->Layout();
	fgSizer3->Fit( m_panel_main );
	bSizer_panel->Add( m_panel_main, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_checkBox_mute = new wxCheckBox( this, wxID_ANY, wxT("Mute"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_checkBox_mute, 1, wxEXPAND|wxALL, 5 );
	
	m_checkBox_loop = new wxCheckBox( this, wxID_ANY, wxT("Loop"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_checkBox_loop, 1, wxEXPAND|wxALL, 5 );
	
	m_checkBox_lowres = new wxCheckBox( this, wxID_ANY, wxT("Low-Res"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_checkBox_lowres, 1, wxEXPAND|wxALL, 5 );
	
	m_staticText_SloMo = new wxStaticText( this, wxID_ANY, wxT("Slo-Mo"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SloMo->Wrap( -1 );
	bSizer3->Add( m_staticText_SloMo, 0, wxEXPAND|wxALL, 5 );
	
	m_slider_SloMo = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	bSizer3->Add( m_slider_SloMo, 0, 0, 5 );
	
	bSizer_panel->Add( bSizer3, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer_panel );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( plbkPlaybackControlsDialogBase::OnClose ) );
	m_bpButton_ToStart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_ToStart_OnButtonClick ), NULL, this );
	m_bpButton_FastRev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_FastRev_OnButtonClick ), NULL, this );
	m_bpButton_PlayBack->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_PlayBack_OnButtonClick ), NULL, this );
	m_bpButton_Pause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_Pause_OnButtonClick ), NULL, this );
	m_bpButton_PlayFwd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_PlayFwd_OnButtonClick ), NULL, this );
	m_bpButton_FastFwd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_FastFwd_OnButtonClick ), NULL, this );
	m_bpButton_ToEnd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_ToEnd_OnButtonClick ), NULL, this );
	m_checkBox_loop->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_Loop_OnCheck ), NULL, this );
	m_checkBox_lowres->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_LowRes_OnCheck ), NULL, this );
	m_checkBox_mute->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_Mute_OnCheck ), NULL, this );
	m_slider_SloMo->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( plbkPlaybackControlsDialogBase::Slider_SloMo_ScrollChanged ), NULL, this );
}

plbkPlaybackControlsDialogBase::~plbkPlaybackControlsDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( plbkPlaybackControlsDialogBase::OnClose ) );
	m_bpButton_ToStart->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_ToStart_OnButtonClick ), NULL, this );
	m_bpButton_FastRev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_FastRev_OnButtonClick ), NULL, this );
	m_bpButton_PlayBack->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_PlayBack_OnButtonClick ), NULL, this );
	m_bpButton_Pause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_Pause_OnButtonClick ), NULL, this );
	m_bpButton_PlayFwd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_PlayFwd_OnButtonClick ), NULL, this );
	m_bpButton_FastFwd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_FastFwd_OnButtonClick ), NULL, this );
	m_bpButton_ToEnd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::Button_ToEnd_OnButtonClick ), NULL, this );
	m_checkBox_loop->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_Loop_OnCheck ), NULL, this );
	m_checkBox_lowres->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_LowRes_OnCheck ), NULL, this );
	m_checkBox_mute->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( plbkPlaybackControlsDialogBase::CheckBox_Mute_OnCheck ), NULL, this );
	m_slider_SloMo->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( plbkPlaybackControlsDialogBase::Slider_SloMo_ScrollChanged ), NULL, this );
}
