/*****************************************************************************
**	envtEnvironmentObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentObjectsPage.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/Object/envtScriptObject.hpp"

#include <vector>


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
envtEnvironmentObjectsPage* envtEnvironmentObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::envtEnvironmentObjectsPage( wxWindow* parent )
: envtEnvironmentObjectsPageBase( parent )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::~envtEnvironmentObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing envtEnvironmentObjectsPage()");
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
	m_EnvironmentName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void envtEnvironmentObjectsPage::Update(const nameString& i_EnvironmentName)
{	
	m_EnvironmentName = i_EnvironmentName;

	std::vector<nameString> all_objects;
	evmtEnvironmentMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	evmtEnvironmentMgr::GetObjectsInSet(m_EnvironmentName, set_objects);

	m_checkList_Objects->Clear();

	int list_index;
	const int num_objects = all_objects.size();
	if (i_EnvironmentName == evmtEnvironmentMgr::GetSwlEnvironmentName())
	{
		for (int i=0; i<num_objects; i++)
		{
			list_index = m_checkList_Objects->Append( wxString(all_objects[i].GetString().c_str(), wxConvUTF8) );
			m_checkList_Objects->Check(list_index, true );
		}
		m_checkList_Objects->Enable(false);
	}
	else
	{
		m_checkList_Objects->Enable(true);
		for (int i=0; i<num_objects; i++)
		{
			std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
			bool checked = (it != set_objects.end());

			list_index = m_checkList_Objects->Append( wxString(all_objects[i].GetString().c_str(), wxConvUTF8) );
			m_checkList_Objects->Check(list_index, checked );
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObjectsPage::checkList_Objects_Toggle( wxCommandEvent& i_Event)
{
	int index = i_Event.GetSelection();
	bool bNewCheckState = m_checkList_Objects->IsChecked(index);

	// Identify the object
	//std::vector<nameString> all_objects;
	//evmtEnvironmentMgr::GetAllObjects(all_objects);

	wxString cur_object = m_checkList_Objects->GetString( index );

	if (bNewCheckState)
		envtOperations::AddObjectToEnvironment(m_EnvironmentName, nameString(std::string(cur_object.utf8_str())));
	else
		envtOperations::RemoveObjectFromEnvironment(m_EnvironmentName, nameString(std::string(cur_object.utf8_str())));
}


#endif // USE_WXWIDGETS
