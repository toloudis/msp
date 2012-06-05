/*****************************************************************************
**	cmmSceneOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmSceneOperations.hpp"
#include "Systems/Common/Gui/cmmDialogInterestMgr.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/mtrl/Gui/mtrlOperations.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Core/undo/undoMultipleOperationBlock.hpp"


//============================================================================
//============================================================================
namespace cmmSceneOperations
{
	namespace
	{
		//------------------------------------------------------------------------
		// Given a cmmDialogDataList and a selObject* pick object,
		// find the cmmDialogData for the pick object.
		// If the pick object is an object part, then return the name
		// of the part in o_PartName otherwise return "" for the name.
		//------------------------------------------------------------------------
		bool find_object(const cmmDialogDataList& i_DataList,
						 sel3dObject* i_pPickObject,
						 int &o_ObjectIndex,
						 std::string &o_PartCategory,
						 std::string &o_PartName)
		{
			o_ObjectIndex = -1;
			o_PartCategory = "";
			o_PartName = "";

			cmmDialogDataList::const_iterator it;
			int index = 0;
			for (it = i_DataList.begin(); it != i_DataList.end(); it++, index++)
			{
				if (it->m_pPickObject == i_pPickObject)
				{
					o_ObjectIndex = index;
					return true;
				}
				else if (!it->m_Parts.empty())
				{
					std::string part_name;
					std::map<std::string, std::vector<cmmDialogPartData> >::const_iterator category_it;
					for (category_it = it->m_Parts.begin(); category_it != it->m_Parts.end(); ++category_it)
					{
						int num_parts = category_it->second.size();
						for (int p=0; p<num_parts; ++p)
						{
							const cmmDialogPartData &part_data = category_it->second[p];
							if (part_data.m_pPickObject == i_pPickObject)
							{
								o_ObjectIndex = index;
								o_PartCategory = category_it->first;
								o_PartName = part_data.m_Name;
							}
						}
					}
				}
			}
			return false;
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Make copies of selected objects 
	//--------------------------------------------------------------------
	void DuplicateSelected()
	{
		// Make a copy of the selected list, because this will change.
		std::list<sel3dObject*> selected_objects = sel3dMgr::GetSelectedList();

		// Get copy of dialog data list because this will also change
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		// Use helper to begin and end the multiple undo operation block
		undoMultipleOperationBlock multiple_undo_block("Duplicate");

		int object_index = -1;
		std::string part_category, part_name;
		bool bWarnedAboutObjectPart = false;

		// For every duplicate we make, store the mapping from original name to
		// duplicate name. After we create all duplicates, we will go back
		// through the new objects and remap any internal attachments from the
		// original objects to the new objects. 
		std::map<nameString, nameString> duplicate_name_map;
		std::map<nameString, std::string> name_system_map;

		// Iterate through the old selected list, finding and deleting each object
		std::list<sel3dObject*>::iterator it;
		for (it = selected_objects.begin(); it != selected_objects.end(); ++it)
		{
			// find cmmDialogData for the pick object
			if (find_object(data_list, *it, object_index, part_category, part_name))
			{
				// Is this an object or a part?
				if (part_name.empty())
				{
					// make the duplicate
					nameString dup_name = cmmDialogInterestMgr::DuplicateObject(
						data_list[object_index].m_Name, 
						data_list[object_index].m_SystemName,
						duplicate_name_map);

					// store the system for this name so that we don't have to search
					// again when we remap names below
					if (!dup_name.IsEmpty())
						name_system_map[dup_name] = data_list[object_index].m_SystemName;
				}
				else if (!bWarnedAboutObjectPart)
				{
					bWarnedAboutObjectPart = true;
					guiMessageBox::Show("Cannot duplicate an object part", "Duplicate", guiMessageBox::e_OKOnly);
				}
			}
		}

		// Go back through the list of objects we created and remap 
		// internal name assignments from the original name to the duplicate
		// name. This makes it so that we can copy a light and object where the
		// light is attached to the object and then create a duplicate light and
		// object such that the new light is attached to the new object.
		std::map<nameString, std::string>::iterator nit;
		for (nit = name_system_map.begin(); nit != name_system_map.end(); ++nit)
		{
			cmmDialogInterestMgr::RemapNames(nit->first, nit->second, duplicate_name_map);
		}
	}

	//--------------------------------------------------------------------
	// Make copy of given named object and return the name of 
	// the new copy.
	//--------------------------------------------------------------------
	nameString DuplicateObject(const nameString& i_ObjectName,
							   std::map<nameString, nameString> &o_DuplicateNameMap)
	{
		// Don't know which system this object is in, so search through
		// all objects in the scene until we find the name match
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		cmmDialogDataList::const_iterator it;
		for (it = data_list.begin(); it != data_list.end(); it++)
		{
			if (it->m_Name == i_ObjectName)
			{
				return cmmDialogInterestMgr::DuplicateObject(it->m_Name, it->m_SystemName, o_DuplicateNameMap);
			}
		}
		return nameString();	// returns empty value if object is not found.
	}

	//--------------------------------------------------------------------
	// Change internal name attachments for given object based on name map
	//--------------------------------------------------------------------
	void RemapNames(const nameString& i_ObjectName,
					const std::map<nameString, nameString> &i_DuplicateNameMap)
	{
		// Don't know which system this object is in, so search through
		// all objects in the scene until we find the name match
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		cmmDialogDataList::const_iterator it;
		for (it = data_list.begin(); it != data_list.end(); it++)
		{
			if (it->m_Name == i_ObjectName)
			{
				cmmDialogInterestMgr::RemapNames(it->m_Name, it->m_SystemName, i_DuplicateNameMap);
				break;
			}
		}
	}

	//--------------------------------------------------------------------
	// Reload selected object 
	//--------------------------------------------------------------------
	void ReloadSelected()
	{
		// Operates only on selected object, not full list
		sel3dObject* selected_object = sel3dMgr::GetSelected();
		if (selected_object)
		{
			// Get copy of dialog data list 
			cmmDialogDataList data_list;
			cmmDialogInterestMgr::GetPlacedObjects(data_list);

			int object_index = -1;
			std::string part_category, part_name;

			// find cmmDialogData for the pick object
			if (find_object(data_list, selected_object, object_index, part_category, part_name))
			{
				// Is this an object or a part?
				if (part_name.empty())
				{
					const std::string& system_name = data_list[object_index].m_SystemName;

					//bga - This was copied from cmmSceneDialog.cpp during refactoring, but
					// there shouldn't be direct use of the system name here. The ReloadObject()
					// function should be handled by the system interests without this code
					// knowing which system handled it.

					if (system_name == "Objects")
						cmmDialogInterestMgr::ReloadObject(
							data_list[object_index].m_Name, 
							data_list[object_index].m_SystemName);
					else if (system_name == "Projected Lights")
						prjltOperations::ReloadTextures();
					else if (system_name == "Environments")
						evmtEnvironmentMgr::ReloadTextures(data_list[object_index].m_Name);
				}
				else
				{
					if (part_category == "Materials")
					{
						mtrlOperations::ReloadTextures();
						//mtrlOperations::ReloadShader();
					}
					else
						guiMessageBox::Show("Cannot reload an object part", "Reload", guiMessageBox::e_OKOnly);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// Delete selected objects 
	//--------------------------------------------------------------------
	void DeleteSelected()
	{
		// Make a copy of the selected list, because this will change.
		std::list<sel3dObject*> selected_objects = sel3dMgr::GetSelectedList();

		// Get copy of dialog data list because this will also change
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		// Use helper to begin and end the multiple undo operation block
		undoMultipleOperationBlock multiple_undo_block("Delete");

		// Clear the selection before deleting?
		//sel3dMgr::CreateUndoOperation();
		sel3dMgr::ClearSelection();

		int object_index = -1;
		std::string part_category, part_name;

		// Iterate through the old selected list, finding and deleting each object
		std::list<sel3dObject*>::iterator it;
		for (it = selected_objects.begin(); it != selected_objects.end(); ++it)
		{
			// find cmmDialogData for the pick object
			if (find_object(data_list, *it, object_index, part_category, part_name))
			{
				// Is this an object or a part?
				if (part_name.empty())
				{
					cmmDialogInterestMgr::DeleteObject(
						data_list[object_index].m_Name, 
						data_list[object_index].m_SystemName);
				}
				else
				{
					cmmDialogInterestMgr::DeleteObjectPart(
						data_list[object_index].m_Name, 
						data_list[object_index].m_SystemName,
						part_name,
						part_category);
				}
			}

		}
	}
	//--------------------------------------------------------------------
	// Set visibility for entire system at once 
	//--------------------------------------------------------------------
	void SetSystemVisibility(const std::string &i_SystemName, bool i_bVisible)
	{
		// Use helper to begin and end the multiple undo operation block
		undoMultipleOperationBlock multiple_undo_block("Set Visibility");

		// Get copy of dialog data list 
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);
	
		cmmDialogDataList::const_iterator it;
		for (it = data_list.begin(); it != data_list.end(); it++)
		{
			if (it->m_SystemName == i_SystemName)
			{
				//bga - this would be better if it used the name id, not just the string
				visMgr::SetVisibleInEditor(it->m_Name.GetString(), i_bVisible);
			}
		}
	}

	//--------------------------------------------------------------------
	// Select the objects in the list, clearing all previous selections
	//--------------------------------------------------------------------
	void Select(const std::list<sel3dObject*>& i_Objects )
	{
		//bga - this used to go through the cmmDialogInterestMgr, but in the
		// meantime we have changed things so that all cmmDialogData and parts
		// have the sel3dObject* anyway - so we can just select them directly now.
		//
		if (!i_Objects.empty())
		{
			sel3dMgr::CreateUndoOperation();

			std::list<sel3dObject*>::const_iterator it = i_Objects.begin();
			sel3dMgr::Select(*it++);
			for (; it != i_Objects.end(); ++it)
			{
				sel3dMgr::AddToSelection( *it );
			}
		}
	}

	//--------------------------------------------------------------------
	// Append objects in the list to the selection
	//--------------------------------------------------------------------
	void AddToSelection(const std::list<sel3dObject*>& i_Objects )
	{
		//bga - this used to go through the cmmDialogInterestMgr, but in the
		// meantime we have changed things so that all cmmDialogData and parts
		// have the sel3dObject* anyway - so we can just select them directly now.
		//
		if (!i_Objects.empty())
		{
			sel3dMgr::CreateUndoOperation();

			std::list<sel3dObject*>::const_iterator it;
			for (it = i_Objects.begin(); it != i_Objects.end(); ++it)
			{
				sel3dMgr::AddToSelection( *it );
			}
		}
	}

	//--------------------------------------------------------------------
	// Remove objects in the list from the selection
	//--------------------------------------------------------------------
	void RemoveFromSelection(const std::list<sel3dObject*>& i_Objects )
	{
		//bga - this used to go through the cmmDialogInterestMgr, but in the
		// meantime we have changed things so that all cmmDialogData and parts
		// have the sel3dObject* anyway - so we can just select them directly now.
		//
		if (!i_Objects.empty())
		{
			sel3dMgr::CreateUndoOperation();

			std::list<sel3dObject*>::const_iterator it;
			for (it = i_Objects.begin(); it != i_Objects.end(); ++it)
			{
				sel3dMgr::RemoveFromSelection( *it );
			}
		}
	}

}	// end of namespace
