///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ToolUIWx/twx/twxWidgets.hpp"
#ifdef USE_WXWIDGETS
#include "cmmSceneDialogBase.h"

///////////////////////////////////////////////////////////////////////////

cmmSceneDialogBase::cmmSceneDialogBase( wxWindow* parent,
										wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer_Dialog;
	sizer_Dialog = new wxBoxSizer( wxVERTICAL );
		
	// Code is now broken up into panel classes. The base class is
	// just the notebook for holding the panels now.
	m_notebook1 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT );

	sizer_Dialog->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( sizer_Dialog );
	this->Layout();
}

cmmSceneDialogBase::~cmmSceneDialogBase()
{
}

#endif
