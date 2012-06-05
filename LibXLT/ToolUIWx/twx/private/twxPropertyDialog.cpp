/*****************************************************************************
**  twxPropertyDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxPropertyDialog.hpp"

#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"

#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twxPropertyDialog::twxPropertyDialog( wxWindow* parent, 
							const wxString& i_Title,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message ) 
:	twxPropertyDialogBase( parent, wxID_ANY, i_Title )
{
	if (i_Message)
		m_staticText_Message->SetLabel(wxString(i_Message, wxConvUTF8));
	std::string dialog_name("PropertyDialog"); // not needed, not using categories
	pwxFormControlBuilder::BuildForm(m_panel_Properties, dialog_name, i_PropertyContainer );

	// Make sure the properties panel is big enough to see some properties,
	// or all of them if just a few.
	int scw_w=0, scw_h=0;
	m_panel_Properties->GetVirtualSize(&scw_w,&scw_h);
	m_panel_Properties->SetMinSize(wxSize(-1, (scw_h < 400) ? scw_h : 400));

	// Redo layout and try to get the dialog to resize based
	// on the new size of the property panel
	this->Layout();
	this->GetSizer()->Fit( this );

	//bga - Note the above two sizing lines only work if they have been commented out in the
	// generated code in twxPropertyDialogBase's constructor.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twxPropertyDialog::twxPropertyDialog( wxWindow* parent, 
					const wxString& i_Title,
					std::vector<std::string>& i_RowNames,
					std::vector<std::string>& i_ColumnNames,
					std::vector<prtyObject*>& i_Rows)
: twxPropertyDialogBase( parent, wxID_ANY, i_Title )
{
	pwxFormControlBuilder::BuildGridForm(m_panel_Properties, i_RowNames, i_ColumnNames, i_Rows );

	// Make sure the properties panel is big enough to see some properties,
	// or all of them if just a few.
	int scw_w=0, scw_h=0;
	m_panel_Properties->GetVirtualSize(&scw_w,&scw_h);
	m_panel_Properties->SetMinSize(wxSize(-1, (scw_h < 400) ? scw_h : 400));

	// Redo layout and try to get the dialog to resize based
	// on the new size of the property panel
	this->Layout();
	this->GetSizer()->Fit( this );

	//bga - Note the above two sizing lines only work if they have been commented out in the
	// generated code in twxPropertyDialogBase's constructor.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twxPropertyDialog::~twxPropertyDialog()
{
	pwxFormControlBuilder::ClearForm(m_panel_Properties);
}

#endif // USE_WXWIDGETS
