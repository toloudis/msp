/*****************************************************************************
**	lsetLightSetLightsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetLightsPage.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include <vector>

#ifdef USE_WXWIDGETS


//============================================================================
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//============================================================================
lsetLightSetLightsPage* lsetLightSetLightsPage::Instance = NULL;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::lsetLightSetLightsPage( wxWindow* parent )
:	lsetLightSetLightsPageBase( parent )
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::~lsetLightSetLightsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing lsetLightSetLightsPage()");
	if (lsetLightSetLightsPage::Instance == this)
		lsetLightSetLightsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void lsetLightSetLightsPage::Clear()
{
	//	clear the controls
	m_checkList_Lights->Clear();
	m_LightSetName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void lsetLightSetLightsPage::Update(const nameString& i_LightSetName)
{	
	m_LightSetName = i_LightSetName;

	m_checkList_Lights->Clear();

	std::vector<nameString> all_lights;
	ltstLightSetMgr::GetAllLights(all_lights);

	std::vector<nameString> set_objects;
	ltstLightSetMgr::GetLightsInSet(m_LightSetName, set_objects);

	int list_index;
	const int num_objects = all_lights.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_lights[i]);
		bool checked = (it != set_objects.end());

		list_index = m_checkList_Lights->Append( wxString(all_lights[i].GetString().c_str(), wxConvUTF8) );
		m_checkList_Lights->Check(list_index, checked );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lsetLightSetLightsPage::checkList_Lights_Toggle( wxCommandEvent& i_Event)
{
	if (!m_LightSetName.IsEmpty())
	{
		int index = i_Event.GetSelection();
		bool bChecked = m_checkList_Lights->IsChecked(index);
		
		//DBG_LOG2("EnvLights toggle: %d - %s", index, (bChecked) ? "true" : "false");

		//std::vector<nameString> all_lights;
		//ltstLightSetMgr::GetAllLights(all_lights);
		wxString wxLightName = m_checkList_Lights->GetString(index);

		if (bChecked)
		{
			//lsetOperations::AddLightToSet(m_LightSetName, all_lights[index]);
			lsetOperations::AddLightToSet(m_LightSetName, nameString(std::string(wxLightName.utf8_str())));
		}
		else
		{
			//lsetOperations::RemoveLightFromSet(m_LightSetName, all_lights[index]);
			lsetOperations::RemoveLightFromSet(m_LightSetName, nameString(std::string(wxLightName.utf8_str())));
		}
	}
}


#endif // USE_WXWIDGETS
