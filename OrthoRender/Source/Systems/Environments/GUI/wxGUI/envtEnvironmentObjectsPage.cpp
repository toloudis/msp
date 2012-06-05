/*****************************************************************************
**	envtEnvironmentObjectsPage.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentObjectsPage.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/Object/envtScriptObject.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"

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
envtEnvironmentObjectsPage* envtEnvironmentObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::envtEnvironmentObjectsPage( wxWindow* parent )
: envtEnvironmentObjectsPageBase( parent ),
	m_pObject(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::~envtEnvironmentObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing envtEnvironmentObjectsPage()");
	if (envtEnvironmentObjectsPage::Instance == this)
		envtEnvironmentObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void envtEnvironmentObjectsPage::Clear()
{
	//	clear the controls
	m_checkList_Objects->Clear();
	m_pObject = NULL;
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void envtEnvironmentObjectsPage::Update(envtScriptObject *i_pScriptObject)
{	
	m_pObject = i_pScriptObject;

	m_checkList_Objects->Clear();

	std::vector<nameString> all_objects;
	evmtEnvironmentMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	if (m_pObject)
		m_pObject->GetObjectsInEnvironment(set_objects);

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());

		m_checkList_Objects->Append( all_objects[i].GetString() );
		m_checkList_Objects->Check(i, checked );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObjectsPage::checkList_Objects_Toggle( wxCommandEvent& i_Event)
{
	if (m_pObject)
	{
		int index = i_Event.GetSelection();
		bool bChecked = m_checkList_Objects->IsChecked(index);

		//DBG_LOG2("EnvObjects toggle: %d - %s", index, (bChecked) ? "true" : "false");

		std::vector<nameString> all_objects;
		evmtEnvironmentMgr::GetAllObjects(all_objects);

		if (bChecked)
		{
			envtOperations::AddObjectToEnvironment(m_pObject, all_objects[index]);
		}
		else
		{
			envtOperations::RemoveObjectFromEnvironment(m_pObject, all_objects[index]);
		}
	}
}


#endif // USE_WXWIDGETS
