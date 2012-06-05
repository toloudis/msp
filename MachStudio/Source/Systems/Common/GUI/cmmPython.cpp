/****************************************************************************\
**	cmmPython.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmPython.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

#include "Support/pyth/pythModules.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// Get name object from selection
	//--------------------------------------------------------------------
	nameObject* get_sel_object()
	{
		sel3dObject *pick_obj = sel3dMgr::GetSelected();
		if (pick_obj)
		{
			return dynamic_cast<nameObject*>(pick_obj);
		}
		return NULL;
	}
	//--------------------------------------------------------------------
	// Get system name and full name string for object with given name
	//--------------------------------------------------------------------
	bool get_system_name(const char *i_ObjectName,
						 nameString &o_Name, 
						 std::string& o_SystemName)
	{
		nameString name_str(i_ObjectName);
		//nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		//if (!pNameObj)
		//	return false;

		// get the actual nameString, including the UID within
		//const nameString &full_name = pNameObj->GetName();

		// Now try to find this name within the placed list
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		cmmDialogDataList::iterator it;
		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			const cmmDialogData &data = (*it);
			//if (data.m_Name == full_name)
			if (data.m_Name == name_str)
			{
				o_Name = data.m_Name;
				o_SystemName = data.m_SystemName;
				return true;
			}
		}
		return false;
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create object from system name and filename
	//--------------------------------------------------------------------
	PyObject *
	create_object(PyObject *self, PyObject *args)
	{
		const char *systemName, *pathName;

		// Command takes arguments as system name and then filename:
		//		createObject("Point Lights", "Point Light")
		//		createObject("Prop", "YAguitar.mhx")
		if (!PyArg_ParseTuple(args, "ss", &systemName, &pathName))
			return NULL;

		if (!cmmDialogInterestMgr::SystemExists(systemName))
		{
			PyErr_SetString(PyExc_NameError, "System with given name does not exist.");
			return NULL;
		}

		// Clear selection so that we can tell when no new object was created
		sel3dMgr::ClearSelection();

		// AddObject function expects that the first name in the path is the
		// system name and then that the rest is the path to the asset.
		fsLocator path_loc;
		fsFileUtil::ANSIFilenameToLocator(pathName, path_loc);
		fsLocator full_path;
		full_path.Push(systemName);
		full_path.Push(path_loc);
		cmmDialogInterestMgr::AddObject(full_path.GetLastName(), full_path);

		// Assuming that the newly created object is the selected object now
		nameObject *pNameObject = get_sel_object();
		if (!pNameObject)
		{
			PyErr_SetString(PyExc_NameError, "Could not create object.");
			return NULL;
		}
		
		return Py_BuildValue("s", pNameObject->GetName().GetString().c_str());
	}

	//--------------------------------------------------------------------
	// Delete object by name
	//--------------------------------------------------------------------
	PyObject *
	delete_object(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		nameString name;
		std::string systemName;
		if (!get_system_name(objectName, name, systemName))
		{
			PyErr_SetString(PyExc_NameError, "Could not find object to delete.");
			return NULL;
		}

		// Clear selection so that we can tell when no new object was created
		sel3dMgr::ClearSelection();

		// Create a duplicate of this object
		cmmDialogInterestMgr::DeleteObject(name, systemName);

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// Duplicate object by name, returns name of new object
	//--------------------------------------------------------------------
	PyObject *
	duplicate_object(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		nameString name;
		std::string systemName;
		if (!get_system_name(objectName, name, systemName))
		{
			PyErr_SetString(PyExc_NameError, "Could not find object to duplicate.");
			return NULL;
		}

		std::map<nameString, nameString> duplicate_name_map;
		nameString dup_name = cmmDialogInterestMgr::DuplicateObject(name, systemName, duplicate_name_map);
		cmmDialogInterestMgr::RemapNames(name, systemName, duplicate_name_map);

		// Assuming that the newly created object is the selected object now
		//nameObject *pNameObject = get_sel_object();
		//if (!pNameObject)
		if (dup_name.IsEmpty())
		{
			PyErr_SetString(PyExc_NameError, "Could not duplicate object.");
			return NULL;
		}
		
		//return Py_BuildValue("s", pNameObject->GetName().GetString().c_str());
		return Py_BuildValue("s", dup_name.GetString().c_str());
	}

	//--------------------------------------------------------------------
	// Return list of placed objects that have names
	//--------------------------------------------------------------------
	PyObject *
	get_placed_list(PyObject *self, PyObject *args)
	{
		// Use nameMgr, not the actual placed list because we want to get the
		// real name objects, not just the interface names like "Fog", etc.
		nameList name_list;
		nameMgr::GetNameList(name_list);

		const int num_names = name_list.size();
		PyObject* NameList = PyList_New(num_names);
		if (NameList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				PyList_SetItem(NameList, i, PyString_FromString(name_list[i]->GetString().c_str()));
			}

		//? Py_DECREF(NameList);
		}
		
		return NameList;
	}

	//--------------------------------------------------------------------
	// Return list of placed objects that have a given type
	//--------------------------------------------------------------------
	PyObject *
	get_placed_by_type(PyObject *self, PyObject *args)
	{
		const char *systemName;

		// Command takes system name as argument:
		//		getPlacedOfType("Point Lights")
		if (!PyArg_ParseTuple(args, "s", &systemName))
			return NULL;

		if (!cmmDialogInterestMgr::SystemExists(systemName))
		{
			PyErr_SetString(PyExc_NameError, "System with given name does not exist.");
			return NULL;
		}
		
		// Now gather list of the names that are in this system
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);

		std::vector<nameString> name_list;
		cmmDialogDataList::iterator it;
		std::string desiredSystem(systemName);
		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			if (it->m_SystemName == desiredSystem)
			{
				name_list.push_back( it->m_Name );
			}
		}

		const int num_names = name_list.size();
		PyObject* NameList = PyList_New(num_names);
		if (NameList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				PyList_SetItem(NameList, i, PyString_FromString(name_list[i].GetString().c_str()));
			}

		//? Py_DECREF(NameList);
		}
		
		return NameList;
	}

	//--------------------------------------------------------------------
	// Return system name of object
	//--------------------------------------------------------------------
	PyObject *
	get_type_of_object(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		nameString name;
		std::string systemName;
		if (!get_system_name(objectName, name, systemName))
		{
			PyErr_SetString(PyExc_NameError, "Could not find object by name.");
			return NULL;
		}
		
		return Py_BuildValue("s", systemName.c_str());
	}
#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void cmmPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "createObject", 
		"Create object from available list, returns name of created object.\n"
		"	Takes arguments as system name and then filename:\n"
		"		light = createObject(\"Point Lights\", \"Point Light\")\n"
		"		prop = createObject(\"Props\", \"YAguitar.mhx\")",
		create_object);
	pythModules::AddCommand(i_ModuleName, "deleteObject", 
		"Delete object by name.",
		delete_object);
	pythModules::AddCommand(i_ModuleName, "duplicateObject", 
		"Duplicate object by name, returns name of new object.",
		duplicate_object);

	pythModules::AddCommand(i_ModuleName, "getPlacedObjects", 
		"Get list of names of all placed objects.\n"
		"		list = getPlacedObjects()", 
		get_placed_list);
	pythModules::AddCommand(i_ModuleName, "getPlacedOfType", 
		"Get list of names of objects of a given type.\n"
		"		list = getPlacedOfType(\"Props\")", 
		get_placed_by_type);


	pythModules::AddCommand(i_ModuleName, "getType", 
		"Returns type of object as string.\n"
		"		This string is the same as the system name category in the Placed tab.", 
		get_type_of_object);
#endif
}