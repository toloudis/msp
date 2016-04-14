///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "twcColorDialogBase.h"

///////////////////////////////////////////////////////////////////////////

twcColorDialogBase::twcColorDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel_2dpicker = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 256,256 ), wxTAB_TRAVERSAL );
	m_panel_2dpicker->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_panel_2dpicker->SetMinSize( wxSize( 256,256 ) );
	
	bSizer3->Add( m_panel_2dpicker, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_panel_vertSlider = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 40,256 ), wxNO_BORDER|wxTAB_TRAVERSAL );
	m_panel_vertSlider->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_panel_vertSlider->SetMinSize( wxSize( 40,256 ) );
	
	bSizer3->Add( m_panel_vertSlider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Color") ), wxHORIZONTAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	bSizer5->SetMinSize( wxSize( 40,-1 ) ); 
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 1, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Old"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer5->Add( m_staticText2, 1, wxALL, 5 );
	
	sbSizer1->Add( bSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_panel_NewColor = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 60,30 ), wxTAB_TRAVERSAL );
	m_panel_NewColor->SetBackgroundColour( wxColour( 234, 239, 44 ) );
	
	bSizer6->Add( m_panel_NewColor, 1, wxEXPAND | wxALL, 0 );
	
	m_panel_OldColor = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 60,30 ), wxTAB_TRAVERSAL );
	m_panel_OldColor->SetBackgroundColour( wxColour( 239, 63, 44 ) );
	
	bSizer6->Add( m_panel_OldColor, 1, wxEXPAND | wxALL, 0 );
	
	sbSizer1->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Channels") ), wxVERTICAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_HSV_H = new wxRadioButton( this, wxID_ANY, wxT("H:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_radioBtn_HSV_H, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	
	m_spinCtrl_HSV_H = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS|wxSP_WRAP, 0, 360, 0 );
	m_spinCtrl_HSV_H->SetSelection(-1,-1);
	bSizer9->Add( m_spinCtrl_HSV_H, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	sbSizer2->Add( bSizer9, 3, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_HSV_S = new wxRadioButton( this, wxID_ANY, wxT("S:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer91->Add( m_radioBtn_HSV_S, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_spinCtrl_HSV_S = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
	m_spinCtrl_HSV_S->SetSelection(-1,-1);
	bSizer91->Add( m_spinCtrl_HSV_S, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	sbSizer2->Add( bSizer91, 3, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_HSV_V = new wxRadioButton( this, wxID_ANY, wxT("V:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer92->Add( m_radioBtn_HSV_V, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_spinCtrl_HSV_V = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
	m_spinCtrl_HSV_V->SetSelection(-1,-1);
	bSizer92->Add( m_spinCtrl_HSV_V, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	sbSizer2->Add( bSizer92, 3, wxEXPAND, 5 );
	
	
	sbSizer2->Add( 0, 0, 1, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_RGB_R = new wxRadioButton( this, wxID_ANY, wxT("R:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer93->Add( m_radioBtn_RGB_R, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_spinCtrl_RGB_R = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	m_spinCtrl_RGB_R->SetSelection(-1,-1);
	bSizer93->Add( m_spinCtrl_RGB_R, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	sbSizer2->Add( bSizer93, 3, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_RGB_G = new wxRadioButton( this, wxID_ANY, wxT("G:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer94->Add( m_radioBtn_RGB_G, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_spinCtrl_RGB_G = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	m_spinCtrl_RGB_G->SetSelection(-1,-1);
	bSizer94->Add( m_spinCtrl_RGB_G, 3, wxALL, 2 );
	
	sbSizer2->Add( bSizer94, 3, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer95;
	bSizer95 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn_RGB_B = new wxRadioButton( this, wxID_ANY, wxT("B:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer95->Add( m_radioBtn_RGB_B, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_spinCtrl_RGB_B = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	m_spinCtrl_RGB_B->SetSelection(-1,-1);
	bSizer95->Add( m_spinCtrl_RGB_B, 3, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	sbSizer2->Add( bSizer95, 3, wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBox_Continuous = new wxCheckBox( this, wxID_ANY, wxT("Update Immediately"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer13->Add( m_checkBox_Continuous, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Apply = new wxButton( this, wxID_APPLY );
	m_sdbSizer1->AddButton( m_sdbSizer1Apply );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer13->Add( m_sdbSizer1, 3, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer13, 0, wxALL|wxEXPAND, 2 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( twcColorDialogBase::OnClose ) );
	m_panel_OldColor->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( twcColorDialogBase::panelOld_MouseDown ), NULL, this );
	m_radioBtn_HSV_H->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_H_Changed ), NULL, this );
	m_spinCtrl_HSV_H->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_H->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_spinCtrl_HSV_H->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_radioBtn_HSV_S->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_S_Changed ), NULL, this );
	m_spinCtrl_HSV_S->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_S->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_spinCtrl_HSV_S->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_radioBtn_HSV_V->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_V_Changed ), NULL, this );
	m_spinCtrl_HSV_V->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_V->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_spinCtrl_HSV_V->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_radioBtn_RGB_R->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_R_Changed ), NULL, this );
	m_spinCtrl_RGB_R->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_R->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_spinCtrl_RGB_R->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_radioBtn_RGB_G->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_G_Changed ), NULL, this );
	m_spinCtrl_RGB_G->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_G->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_spinCtrl_RGB_G->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_radioBtn_RGB_B->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_B_Changed ), NULL, this );
	m_spinCtrl_RGB_B->Connect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_B->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_spinCtrl_RGB_B->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_checkBox_Continuous->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( twcColorDialogBase::checkBox_Continuous_Changed ), NULL, this );
	m_sdbSizer1Apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Apply_Click ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Cancel_Click ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Ok_Click ), NULL, this );
}

twcColorDialogBase::~twcColorDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( twcColorDialogBase::OnClose ) );
	m_panel_OldColor->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( twcColorDialogBase::panelOld_MouseDown ), NULL, this );
	m_radioBtn_HSV_H->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_H_Changed ), NULL, this );
	m_spinCtrl_HSV_H->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_H->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_spinCtrl_HSV_H->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_radioBtn_HSV_S->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_S_Changed ), NULL, this );
	m_spinCtrl_HSV_S->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_S->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_spinCtrl_HSV_S->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_radioBtn_HSV_V->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioHSV_V_Changed ), NULL, this );
	m_spinCtrl_HSV_V->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_HSV_Char ), NULL, this );
	m_spinCtrl_HSV_V->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_HSV_Changed ), NULL, this );
	m_spinCtrl_HSV_V->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_HSV_Focus ), NULL, this );
	m_radioBtn_RGB_R->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_R_Changed ), NULL, this );
	m_spinCtrl_RGB_R->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_R->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_spinCtrl_RGB_R->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_radioBtn_RGB_G->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_G_Changed ), NULL, this );
	m_spinCtrl_RGB_G->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_G->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_spinCtrl_RGB_G->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_radioBtn_RGB_B->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( twcColorDialogBase::radioRGB_B_Changed ), NULL, this );
	m_spinCtrl_RGB_B->Disconnect( wxEVT_CHAR, wxKeyEventHandler( twcColorDialogBase::spinCtrl_RGB_Char ), NULL, this );
	m_spinCtrl_RGB_B->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( twcColorDialogBase::spinCtrl_RGB_Changed ), NULL, this );
	m_spinCtrl_RGB_B->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( twcColorDialogBase::spinCtrl_RGB_Focus ), NULL, this );
	m_checkBox_Continuous->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( twcColorDialogBase::checkBox_Continuous_Changed ), NULL, this );
	m_sdbSizer1Apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Apply_Click ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Cancel_Click ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( twcColorDialogBase::button_Ok_Click ), NULL, this );
}

#endif
