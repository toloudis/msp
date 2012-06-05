/*****************************************************************************
**	grupGroupObjectsPage.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/GUI/wxGUI/grupGroupObjectsPage.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"
#include "Systems/Groups/Object/grupGroupObject.hpp"

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
grupGroupObjectsPage* grupGroupObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
grupGroupObjectsPage::grupGroupObjectsPage( wxWindow* parent )
: grupGroupObjectsPageBase( parent )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
grupGroupObjectsPage::~grupGroupObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing grupGroupObjectsPage()");
	if (grupGroupObjectsPage::Instance == this)
		grupGroupObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void grupGroupObjectsPage::Clear()
{
	//	clear the controls
	m_checkList_Objects->Clear();
	m_GroupName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void grupGroupObjectsPage::Update(const nameString& i_GroupName)
{	
	m_GroupName = i_GroupName;

	m_checkList_Objects->Clear();

	std::vector<nameString> all_objects;
	grpsGroupMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	grpsGroupMgr::GetObjectsInGroup(m_GroupName, set_objects);

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
void grupGroupObjectsPage::checkList_Objects_Toggle( wxCommandEvent& i_Event)
{
	if (!m_GroupName.IsEmpty())
	{
		int index = i_Event.GetSelection();
		bool bChecked = m_checkList_Objects->IsChecked(index);

		//DBG_LOG2("EnvObjects toggle: %d - %s", index, (bChecked) ? "true" : "false");

		std::vector<nameString> all_objects;
		grpsGroupMgr::GetAllObjects(all_objects);

		if (bChecked)
		{
			grupOperations::AddObjectToGroup(m_GroupName, all_objects[index]);
		}
		else
		{
			grupOperations::RemoveObjectFromGroup(m_GroupName, all_objects[index]);
		}
	}
}


#endif // USE_WXWIDGETS
