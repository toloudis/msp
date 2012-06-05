///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "lsetLightSetLightsPageBase.h"

///////////////////////////////////////////////////////////////////////////

lsetLightSetLightsPageBase::lsetLightSetLightsPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkList_LightsChoices;
	m_checkList_Lights = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList_LightsChoices, wxLB_SORT );
	bSizer3->Add( m_checkList_Lights, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	m_checkList_Lights->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( lsetLightSetLightsPageBase::checkList_Lights_Toggle ), NULL, this );
}

lsetLightSetLightsPageBase::~lsetLightSetLightsPageBase()
{
	// Disconnect Events
	m_checkList_Lights->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( lsetLightSetLightsPageBase::checkList_Lights_Toggle ), NULL, this );
}
