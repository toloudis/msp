///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "plbkPlaybackControlsDialogBase.h"

///////////////////////////////////////////////////////////////////////////

plbkPlaybackControlsDialogBase::plbkPlaybackControlsDialogBase( wxWindow* parent, 
																const wxString& i_IconDirectory,
															    wxWindowID id, 
																const wxPoint& pos, 
																const wxSize& size, 
																long style ) 
: wxPanel( parent, id, pos, size, style )
{
    wxLogNull nullLog;

	wxBoxSizer* bSizer_panel;
	bSizer_panel = new wxBoxSizer( wxVERTICAL );
	
	m_panel_main = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer_Buttons;
	fgSizer_Buttons = new wxFlexGridSizer( 2, 7, 0, 0 );
	fgSizer_Buttons->SetFlexibleDirection( wxBOTH );
	fgSizer_Buttons->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_bpButton_ToStart = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-begin.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,-1 ), wxNO_BORDER );
	m_bpButton_ToStart->SetToolTip(wxT("Move to beginning"));
	//m_bpButton_ToStart->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-begin_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToStart->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-begin_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToStart->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-begin_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToStart->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_ToStart, 1, 0, 3 );
	
	m_bpButton_FastRev = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-fastrev.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_FastRev->SetToolTip(wxT(" Fast rewind"));
	//m_bpButton_FastRev->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-fastrev_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastRev->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-fastrev_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastRev->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-fastrev_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastRev->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_FastRev, 0, wxRIGHT|wxLEFT, 3 );
	
	m_bpButton_PlayBack = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-playrev.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_PlayBack->SetToolTip(wxT("Play Reverse"));
	//m_bpButton_PlayBack->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-playrev_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayBack->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-playrev_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayBack->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-playrev_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayBack->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_PlayBack, 0, wxRIGHT|wxLEFT, 3 );
	
	m_bpButton_Pause = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-pause.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_Pause->SetToolTip(wxT("Pause Playback"));
	//m_bpButton_Pause->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-pause_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Pause->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-pause_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Pause->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-pause_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_Pause->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_Pause, 0, wxRIGHT|wxLEFT, 3 );
	
	m_bpButton_PlayFwd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-playfwd.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_PlayFwd->SetToolTip(wxT("Play Forward"));
	//m_bpButton_PlayFwd->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-playfwd_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayFwd->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-playfwd_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayFwd->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-playfwd_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_PlayFwd->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_PlayFwd, 0, wxRIGHT|wxLEFT, 3 );
	
	m_bpButton_FastFwd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-fastfwd.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_FastFwd->SetToolTip(wxT("Fast forward"));
	//m_bpButton_FastFwd->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-fastfwd_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastFwd->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-fastfwd_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastFwd->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-fastfwd_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_FastFwd->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_FastFwd, 0, wxRIGHT|wxLEFT, 3 );
	
	m_bpButton_ToEnd = new wxBitmapButton( m_panel_main, wxID_ANY, wxBitmap( i_IconDirectory + wxT("\\playback-end.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	m_bpButton_ToEnd->SetToolTip(wxT("Move to the end"));
	//m_bpButton_ToEnd->SetBitmapDisabled( wxBitmap( i_IconDirectory + wxT("\\playback-end_disabled.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToEnd->SetBitmapHover( wxBitmap( i_IconDirectory + wxT("\\playback-end_highlight.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToEnd->SetBitmapSelected( wxBitmap( i_IconDirectory + wxT("\\playback-end_pressed.PNG"), wxBITMAP_TYPE_ANY ));
	m_bpButton_ToEnd->SetSize(30,30);
	fgSizer_Buttons->Add( m_bpButton_ToEnd, 0, wxLEFT, 3 );
	
	m_panel_main->SetSizer( fgSizer_Buttons );
	m_panel_main->Layout();
	fgSizer_Buttons->Fit( m_panel_main );
	bSizer_panel->Add( m_panel_main, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer_Settings;
	bSizer_Settings = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBox_loop = new wxCheckBox( this, wxID_ANY, wxT("Loop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox_loop->SetToolTip(wxT("Loop"));
	bSizer_Settings->Add( m_checkBox_loop, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBox_lowres = new wxCheckBox( this, wxID_ANY, wxT("Low-Res"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox_lowres->SetToolTip(wxT("Low-Res Playback"));
	bSizer_Settings->Add( m_checkBox_lowres, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBox_mute = new wxCheckBox( this, wxID_ANY, wxT("Mute"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox_mute->SetToolTip(wxT("Mute")); 
	bSizer_Settings->Add( m_checkBox_mute, 0, wxALL, 5 );
	
	bSizer_panel->Add( bSizer_Settings, 0, wxALIGN_CENTER, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_slider_SloMo = new wxSlider( this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_slider_SloMo->SetToolTip( wxT("Speed of Playback") );
	
	bSizer4->Add( m_slider_SloMo, 0, 0, 1 );
	
	m_staticText_SloMo = new wxStaticText( this, wxID_ANY, wxT("Slo-Mo"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_SloMo->Wrap( -1 );
	bSizer4->Add( m_staticText_SloMo, 0, wxALL, 5 );
	
	bSizer_panel->Add( bSizer4, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer_panel );
	this->Layout();
	
	// Connect Events
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

#endif
