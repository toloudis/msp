///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cptrRenderStatsDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cptrRenderStatsDialogBase::cptrRenderStatsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("label") ), wxVERTICAL );
	
	m_richText_stats = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS, wxDefaultValidator, wxT("RenderStats") );
	sbSizer1->Add( m_richText_stats, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( sbSizer1 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( cptrRenderStatsDialogBase::RenderStatsDialog_OnActivate ) );
}

cptrRenderStatsDialogBase::~cptrRenderStatsDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( cptrRenderStatsDialogBase::RenderStatsDialog_OnActivate ) );
}
