///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "LoadPreferencesDialogBase.h"

///////////////////////////////////////////////////////////////////////////

LoadPreferencesDialogBase::LoadPreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_auinotebook1 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT );
	m_panel_LoadPrefs = new wxScrolledWindow( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_panel_LoadPrefs->SetScrollRate( 5, 5 );
	m_auinotebook1->AddPage( m_panel_LoadPrefs, wxT("Load Prefs"), false, wxNullBitmap );
	m_panel_MissingTextures = new wxPanel( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel_MissingTextures, wxID_ANY, wxT("Textures skipped when loading:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_listBox1 = new wxListBox( m_panel_MissingTextures, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer2->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	m_panel_MissingTextures->SetSizer( bSizer2 );
	m_panel_MissingTextures->Layout();
	bSizer2->Fit( m_panel_MissingTextures );
	m_auinotebook1->AddPage( m_panel_MissingTextures, wxT("Missing Textures"), false, wxNullBitmap );
	
	bSizer1->Add( m_auinotebook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

LoadPreferencesDialogBase::~LoadPreferencesDialogBase()
{
}

#endif
