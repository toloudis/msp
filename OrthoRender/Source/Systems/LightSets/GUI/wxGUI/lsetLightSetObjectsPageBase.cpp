///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "lsetLightSetObjectsPageBase.h"

///////////////////////////////////////////////////////////////////////////

lsetLightSetObjectsPageBase::lsetLightSetObjectsPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl_Objects = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	bSizer3->Add( m_treeCtrl_Objects, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	m_treeCtrl_Objects->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( lsetLightSetObjectsPageBase::treeCtrl_Objects_LeftMouseDown ), NULL, this );
}

lsetLightSetObjectsPageBase::~lsetLightSetObjectsPageBase()
{
	// Disconnect Events
	m_treeCtrl_Objects->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( lsetLightSetObjectsPageBase::treeCtrl_Objects_LeftMouseDown ), NULL, this );
}
