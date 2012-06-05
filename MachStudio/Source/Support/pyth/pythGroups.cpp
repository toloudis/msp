/****************************************************************************\
**	pythGroups.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythGroups.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/grps/grpsGroupMgr.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create a new group from selected objects
	//--------------------------------------------------------------------
	PyObject *
	create_group_from_selected(PyObject *self, PyObject *args)
	{
		const char *groupName;
		if (!PyArg_ParseTuple(args, "s", &groupName))
			return NULL;

		std::string set_name(groupName);
		if (!grpsGroupMgr::IsValidGroupName(set_name))
		{
			PyErr_SetString(PyExc_NameError, "Selection set already exists with given name.");
			return NULL;
		}

		grpsGroupMgr::CreateGroupFromSelected( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Delete a group by name
	//--------------------------------------------------------------------
	PyObject *
	delete_group(PyObject *self, PyObject *args)
	{
		const char *groupName;
		if (!PyArg_ParseTuple(args, "s", &groupName))
			return NULL;

		std::string set_name(groupName);
		grpsGroupMgr::DeleteGroup( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Rename a group
	//--------------------------------------------------------------------
	PyObject *
	rename_group(PyObject *self, PyObject *args)
	{
		const char *oldName, *newName;
		if (!PyArg_ParseTuple(args, "ss", &oldName, &newName))
			return NULL;

		grpsGroupMgr::RenameGroup( nameString(oldName), std::string(newName) );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Update group contents based on current selection
	//--------------------------------------------------------------------
	PyObject *
	update_group_from_selected(PyObject *self, PyObject *args)
	{
		const char *groupName;
		if (!PyArg_ParseTuple(args, "s", &groupName))
			return NULL;

		std::string set_name(groupName);
		grpsGroupMgr::UpdateGroupFromSelected( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Update group contents based on current selection
	//--------------------------------------------------------------------
	PyObject *
	select_group(PyObject *self, PyObject *args)
	{
		const char *groupName;
		if (!PyArg_ParseTuple(args, "s", &groupName))
			return NULL;

		std::string set_name(groupName);
		grpsGroupMgr::SelectGroupObjects( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Add object to group
	//--------------------------------------------------------------------
	//PyObject *
	//assign_object_to_group(PyObject *self, PyObject *args)
	//{
	//	const char *groupName, *objectName;
	//	if (!PyArg_ParseTuple(args, "ss", &objectName, &groupName))
	//		return NULL;

	//	grpsGroupMgr::AddObjectToGroup( nameString(groupName), nameString(objectName) );
	//	
	//	return pythFunctionUtil::ReturnNone();
	//}

	//--------------------------------------------------------------------
	// Return list of group names
	//--------------------------------------------------------------------
	PyObject *
	get_groups(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		grpsGroupMgr::GetGroupNames(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of objects in the given group
	//--------------------------------------------------------------------
	PyObject *
	get_objects_in_group(PyObject *self, PyObject *args)
	{
		const char *groupName;
		if (!PyArg_ParseTuple(args, "s", &groupName))
			return NULL;

		nameString set_name(groupName);
		std::vector<nameString> name_list;
		grpsGroupMgr::GetObjectsInGroup(set_name,name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of all objects that could be put into a group
	//--------------------------------------------------------------------
	//PyObject *
	//get_all_objects(PyObject *self, PyObject *args)
	//{
	//	std::vector<nameString> name_list;
	//	grpsGroupMgr::GetAllObjects(name_list);
	//	return pythFunctionUtil::ConvertNameList(name_list);
	//}



}	// end of namespace

//--------------------------------------------------------------------
// Add commands to given module
//--------------------------------------------------------------------
void pythGroups::AddCommands(const std::string &i_ModuleName)
{
	/*------------------------------------------------------------------------
	//	The operations for creating and deleting groups are removed 
	//	temporarily because the code for performing these tasks exists in the
	//	system level code for groups and cannot be accessed by support level
	//	python modules.
	------------------------------------------------------------------------*/
	/*pythModules::AddCommand(i_ModuleName, 
		"createGroupFromSelected", 
		"Create a new group with the given name for the current selected objects."
		"Name must be unique.", 
		create_group_from_selected);
	pythModules::AddCommand(i_ModuleName, 
		"deleteGroup", 
		"Delete group with the given name.", 
		delete_group);*/
	pythModules::AddCommand(i_ModuleName, 
		"renameGroup", 
		"Change name of group."
		"	renameGroup(oldName, newName)", 
		rename_group);
	pythModules::AddCommand(i_ModuleName, 
		"updateGroupFromSelected", 
		"Update group to contain the current selected objects.", 
		update_group_from_selected);
	
	pythModules::AddCommand(i_ModuleName, 
		"selectGroup", 
		"Select the contents of group with the given name", 
		select_group);

	//pythModules::AddCommand(i_ModuleName, 
	//	"assignObjectToGroup", 
	//	"Assign object with given name to group with given name."
	//	"	assignObjectToGroup(objectName, groupName)"
	//	"	assignObjectToGroup(objectName, \"\") - removes from group", 
	//	assign_object_to_group);

	pythModules::AddCommand(i_ModuleName, 
		"getGroups", 
		"Return list of group names.", 
		get_groups);
	pythModules::AddCommand(i_ModuleName, 
		"getObjectsInGroup", 
		"Return list of objects in the given group.", 
		get_objects_in_group);
	//pythModules::AddCommand(i_ModuleName, 
	//	"getAllObjectsForGroups", 
	//	"Return list of all objects that could be put into a group.", 
	//	get_all_objects);

}

#endif
