/*****************************************************************************
**  prefsLayoutChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/wxGUI/prefsLayoutChoice.hpp"

#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"

#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the control, will be cleared
//	when the panel grid is deleted.
//--------------------------------------------------------------------
prefsLayoutChoice* prefsLayoutChoice::Instance = NULL;

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
prefsLayoutChoice::prefsLayoutChoice(wxWindow* parent)
: wxChoice(parent, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(160, -1), parent)),
	m_bDisableNotify(false)
{
	this->Connect(this->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
				wxCommandEventHandler(prefsLayoutChoice::SelectedIndexChanged) );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
prefsLayoutChoice::~prefsLayoutChoice()
{
	if (prefsLayoutChoice::Instance == this)
		prefsLayoutChoice::Instance = NULL;

}

//------------------------------------------------------------------------
// Set list of layout names
//------------------------------------------------------------------------
void prefsLayoutChoice::Update()
{			
	wxString last_selection = this->GetStringSelection();
	this->Clear();

	std::vector<std::string> layout_names;
	prefsLayoutMgr::GetLayoutNames(layout_names);

	// Add names of layouts
	const int num_layouts = layout_names.size();
	for (int i=0; i<num_layouts; i++)
	{
		this->Append( wxString(layout_names[i].c_str(), wxConvUTF8) );
	}

	// Try to restore last selection, if it is still available
	this->SetStringSelection(last_selection);
}

//------------------------------------------------------------------------
// event callbacks
//------------------------------------------------------------------------
void prefsLayoutChoice::SelectedIndexChanged(wxCommandEvent& i_Event)
{
	if (!this->GetStringSelection().empty())
	{
		std::string layout_name(this->GetStringSelection().utf8_str());
		prefsLayoutMgr::SwitchToNamedLayout(layout_name);
	}
}

#endif // USE_WXWIDGETS