///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "envtEnvironmentObjectsPageBase.h"

///////////////////////////////////////////////////////////////////////////

envtEnvironmentObjectsPageBase::envtEnvironmentObjectsPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkList_ObjectsChoices;
	m_checkList_Objects = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList_ObjectsChoices, wxLB_SORT );
	bSizer3->Add( m_checkList_Objects, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	m_checkList_Objects->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( envtEnvironmentObjectsPageBase::checkList_Objects_Toggle ), NULL, this );
}

envtEnvironmentObjectsPageBase::~envtEnvironmentObjectsPageBase()
{
	// Disconnect Events
	m_checkList_Objects->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( envtEnvironmentObjectsPageBase::checkList_Objects_Toggle ), NULL, this );
}
