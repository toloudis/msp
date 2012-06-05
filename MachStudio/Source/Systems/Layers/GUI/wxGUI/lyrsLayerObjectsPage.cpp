/*****************************************************************************
**	lyrsLayerObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/GUI/wxGUI/lyrsLayerObjectsPage.hpp"
#include "Systems/Layers/Undo/lyrsOperations.hpp"
#include "Systems/Layers/Object/lyrsLayerObject.hpp"


#include <vector>

#ifdef USE_WXWIDGETS

namespace
{

}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
lyrsLayerObjectsPage* lyrsLayerObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lyrsLayerObjectsPage::lyrsLayerObjectsPage( wxWindow* parent )
: lyrsLayerObjectsPageBase( parent )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lyrsLayerObjectsPage::~lyrsLayerObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing lyrsLayerObjectsPage()");
	if (lyrsLayerObjectsPage::Instance == this)
		lyrsLayerObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void lyrsLayerObjectsPage::Clear()
{
	//	clear the controls
	m_checkList_Objects->Clear();
	m_LayerName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void lyrsLayerObjectsPage::Update(const nameString& i_LayerName)
{	
	m_LayerName = i_LayerName;

	m_checkList_Objects->Clear();

	std::vector<nameString> all_objects;
	lyerLayerMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	lyerLayerMgr::GetObjectsInLayer(m_LayerName, set_objects);

	int list_index;
	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());

		list_index = m_checkList_Objects->Append( wxString(all_objects[i].GetString().c_str(), wxConvUTF8) );
		m_checkList_Objects->Check(list_index, checked );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lyrsLayerObjectsPage::checkList_Objects_Toggle( wxCommandEvent& i_Event)
{
	if (!m_LayerName.IsEmpty())
	{
		int index = i_Event.GetSelection();
		bool bChecked = m_checkList_Objects->IsChecked(index);

		//DBG_LOG2("EnvObjects toggle: %d - %s", index, (bChecked) ? "true" : "false");

		//std::vector<nameString> all_objects;
		//lyerLayerMgr::GetAllObjects(all_objects);

		wxString cur_object = m_checkList_Objects->GetString( index );

		if (bChecked)
		{
			lyrsOperations::AddObjectToLayer(m_LayerName, nameString(std::string(cur_object.utf8_str())));
		}
		else
		{
			lyrsOperations::RemoveObjectFromLayer(m_LayerName, nameString(std::string(cur_object.utf8_str())));
		}
	}
}


#endif // USE_WXWIDGETS
