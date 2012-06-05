///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dbgConsoleDialogBase.h"

///////////////////////////////////////////////////////////////////////////

dbgConsoleDialogBase::dbgConsoleDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_boxSizer_Main;
	m_boxSizer_Main = new wxBoxSizer( wxVERTICAL );
	
	m_richText_Console = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	m_boxSizer_Main->Add( m_richText_Console, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( m_boxSizer_Main );
	this->Layout();
}

dbgConsoleDialogBase::~dbgConsoleDialogBase()
{
}
