///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "cptrRenderStatsDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cptrRenderStatsDialogBase::cptrRenderStatsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("") ), wxVERTICAL );
	
	m_richText_stats = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS, wxDefaultValidator, wxT("RenderLog") );
	sbSizer1->Add( m_richText_stats, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( sbSizer1 );
	this->Layout();
	// The default size is in DIPs: scale it for the display's DPI, and
	// never let the window be smaller than its contents.
	this->SetSize( FromDIP( size ) );
	this->SetMinClientSize( this->GetSizer()->GetMinSize() );
	
	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( cptrRenderStatsDialogBase::RenderStatsDialog_OnActivate ) );
}

cptrRenderStatsDialogBase::~cptrRenderStatsDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( cptrRenderStatsDialogBase::RenderStatsDialog_OnActivate ) );
}

#endif
