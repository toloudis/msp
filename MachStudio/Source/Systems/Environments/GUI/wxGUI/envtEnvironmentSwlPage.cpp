/*****************************************************************************
**	envtEnvironmentSwlPage.cpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentSwlPage.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/swl/swlPropertyObject.hpp"
#include "Support/swl/swlScriptObject.hpp"
#include "Support/swl/swlSelectionUtil.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"

#include <vector>


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
envtEnvironmentSwlPage* envtEnvironmentSwlPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentSwlPage::envtEnvironmentSwlPage( wxWindow* parent )
	: wxPanel(parent)
{
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	m_tabPage_SoftwareLighting = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_tabPage_SoftwareLighting->SetScrollRate( 0, 5 );
	bSizer->Add( m_tabPage_SoftwareLighting, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer );
	this->Layout();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentSwlPage::~envtEnvironmentSwlPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing envtEnvironmentObjectsPage()");
	if (envtEnvironmentSwlPage::Instance == this)
		envtEnvironmentSwlPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void envtEnvironmentSwlPage::Clear()
{
	//	clear the controls
	m_EnvironmentName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void envtEnvironmentSwlPage::Update(const nameString& i_EnvironmentName)
{	
	m_EnvironmentName = i_EnvironmentName;

	swlScriptObject* script_obj = swlSelectionUtil::GetSelectedScriptObject();
	if (script_obj)
	{
		pwxFormControlBuilder::BuildForm(this->m_tabPage_SoftwareLighting, "Software Lighting", 
			(script_obj->GetPropertyUI()->GetListContainer()), true, false );

		// If property object is read-only (i.e. locked materials),
		// disable whole panel. 
		// Maybe could just disable the property controls themselves sometime later.
		this->m_tabPage_SoftwareLighting->Enable(!script_obj->GetPropertyUI()->IsReadOnly());
	}
	else
	{
		pwxFormControlBuilder::ClearForm(this->m_tabPage_SoftwareLighting);
	}
}


#endif // USE_WXWIDGETS
