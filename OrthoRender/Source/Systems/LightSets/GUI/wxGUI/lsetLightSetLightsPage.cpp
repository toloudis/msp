/*****************************************************************************
**	lsetLightSetLightsPage.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetLightsPage.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/dbg/dbgLog.hpp"

#include <vector>

#ifdef USE_WXWIDGETS

namespace
{

}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
lsetLightSetLightsPage* lsetLightSetLightsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::lsetLightSetLightsPage( wxWindow* parent )
: lsetLightSetLightsPageBase( parent )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::~lsetLightSetLightsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing lsetLightSetLightsPage()");
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

	const int num_objects = all_lights.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_lights[i]);
		bool checked = (it != set_objects.end());

		m_checkList_Lights->Append( all_lights[i].GetString() );
		m_checkList_Lights->Check(i, checked );
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

		std::vector<nameString> all_lights;
		ltstLightSetMgr::GetAllLights(all_lights);

		if (bChecked)
		{
			lsetOperations::AddLightToSet(m_LightSetName, all_lights[index]);
		}
		else
		{
			lsetOperations::RemoveLightFromSet(m_LightSetName, all_lights[index]);
		}
	}
}


#endif // USE_WXWIDGETS
