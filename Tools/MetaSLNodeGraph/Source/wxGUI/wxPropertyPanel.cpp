/*****************************************************************************
**  wxPropertyPanel.hpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "wxPropertyPanel.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxPropertyPanel* wxPropertyPanel::DialogInstance = NULL;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(wxPropertyPanel, wxScrolledWindow)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
wxPropertyPanel::wxPropertyPanel(wxWindow* parent)
: wxScrolledWindow(parent, wxID_ANY)
{
	wxPropertyPanel::DialogInstance = this;
	this->SetScrollRate( 5, 5 );
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"MaterialProperties").Caption(L"Material Properties").
												Layer(3).Right().BestSize(200,400));
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
wxPropertyPanel::~wxPropertyPanel()
{
	if (wxPropertyPanel::DialogInstance == this)
		wxPropertyPanel::DialogInstance = NULL;
}

//--------------------------------------------------------------------
// Clear display
//--------------------------------------------------------------------
void wxPropertyPanel::ClearProperties()
{
	pwxFormControlBuilder::ClearForm(this);
}

//--------------------------------------------------------------------
// Set property object to display
//--------------------------------------------------------------------
void wxPropertyPanel::SetProperties(const prtyPropertyUIInfoContainer& i_PropertyContainer)
{
	const bool bShowCategory = true;
	pwxFormControlBuilder::BuildForm(this, "MaterialDialog", i_PropertyContainer, bShowCategory);
}

#endif // USE_WXWIDGETS
