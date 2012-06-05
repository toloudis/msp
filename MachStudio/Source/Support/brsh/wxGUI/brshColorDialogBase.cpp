///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "brshColorDialogBase.h"

///////////////////////////////////////////////////////////////////////////

brshColorDialogBase::brshColorDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel_Brush = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_panel_Brush, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	//this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( brshColorDialogBase::brshColorDialog_onClose ) );
}

brshColorDialogBase::~brshColorDialogBase()
{
	// Disconnect Events
	//this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( brshColorDialogBase::brshColorDialog_onClose ) );
}

#endif
