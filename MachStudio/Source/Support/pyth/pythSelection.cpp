/****************************************************************************\
**	pythSelection.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythSelection.hpp"

#include "Support/pyth/pythModules.hpp"

#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/sel3d/sel3dObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <vector>

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//--------------------------------------------------------------------
	// Get pickable object by name
	//--------------------------------------------------------------------
	sel3dObject* get_pick_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		return dynamic_cast<sel3dObject*>(pNameObj);
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return list of selected objects that have names
	//--------------------------------------------------------------------
	PyObject *
	get_selection(PyObject *self, PyObject *args)
	{
		const std::list<sel3dObject*> &pick_list = sel3dMgr::GetSelectedList();
		std::vector<nameObject*> name_list;

		// Get out only the objects with names
		std::list<sel3dObject*>::const_iterator it;
		for (it = pick_list.begin(); it != pick_list.end(); ++it)
		{
			nameObject *pNameObj = dynamic_cast<nameObject*>(*it);
			if (pNameObj)
				name_list.push_back(pNameObj);
		}

		const int num_names = name_list.size();
		PyObject* SelList = PyList_New(num_names);
		if (SelList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				PyList_SetItem(SelList, i, PyString_FromString(name_list[i]->GetName().GetString().c_str()));
			}

		//? Py_DECREF(MyList);
		}
		
		return SelList;
	}

	//--------------------------------------------------------------------
	// Clear selected list
	//--------------------------------------------------------------------
	PyObject *
	clear_selection(PyObject *self, PyObject *args)
	{
		sel3dMgr::ClearSelection();

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// Lock/Unlock selected list
	//--------------------------------------------------------------------
	PyObject *
	lock_selection(PyObject *self, PyObject *args)
	{
		sel3dMgr::ToggleSelectionLock();

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// get Lock/Unlock selection flag
	//--------------------------------------------------------------------
	PyObject *
	get_selection_lock(PyObject *self, PyObject *args)
	{
		if(sel3dMgr::getSelectionLock())
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	//--------------------------------------------------------------------
	// get Lock/Unlock selection flag
	//--------------------------------------------------------------------
	PyObject *
	set_selection_lock(PyObject *self, PyObject *args)
	{
		bool _bLockSelection;
		int num_args = PyTuple_Size( args );
		if (num_args != 1)
		{
			PyErr_SetString(PyExc_TypeError, "setSelectionLock only takes 1 value, True or False");
			return NULL;
		}
		
		if(PyTuple_GetItem( args, 0 ) == Py_True)
		{
			_bLockSelection = true;
		}
		else if(PyTuple_GetItem( args, 0 ) == Py_False)
		{
			_bLockSelection = false;
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "setSelectionLock must take a bool value, True or False");
			return NULL;
		}
		
		sel3dMgr::setSelectionLock(_bLockSelection);

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// Select a single object
	//--------------------------------------------------------------------
	PyObject *
	select_object(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		sel3dObject *pPickObj = get_pick_object(objectName);
		if (!pPickObj)
		{
			PyErr_SetString(PyExc_NameError, "Could not find object by name.");
			return NULL;
		}

		sel3dMgr::Select( pPickObj );
		
		Py_INCREF(Py_None);
		return Py_None;
	}
	//--------------------------------------------------------------------
	// Append a single object to the selection list
	//--------------------------------------------------------------------
	PyObject *
	append_select(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		sel3dObject *pPickObj = get_pick_object(objectName);
		if (!pPickObj)
		{
			PyErr_SetString(PyExc_NameError, "Could not find object by name.");
			return NULL;
		}

		sel3dMgr::AddToSelection( pPickObj );
		
		Py_INCREF(Py_None);
		return Py_None;
	}
	//--------------------------------------------------------------------
	// Remove a single object from the selection list
	//--------------------------------------------------------------------
	PyObject *
	remove_select(PyObject *self, PyObject *args)
	{
		const char *objectName;

		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		sel3dObject *pPickObj = get_pick_object(objectName);
		if (!pPickObj)
		{
			PyErr_SetString(PyExc_NameError, "Could not find object by name.");
			return NULL;
		}

		sel3dMgr::RemoveFromSelection( pPickObj );
		
		Py_INCREF(Py_None);
		return Py_None;
	}

}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythSelection::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, "getSelection", 
		"Get list of selected objects.", 
		get_selection);
	pythModules::AddCommand(i_ModuleName, "clearSelection", 
		"Clear list of selected objects.", 
		clear_selection);
	pythModules::AddCommand(i_ModuleName, "toggleSelectionLock", 
		"Lock/Unlock list of selected objects.", 
		lock_selection);
	pythModules::AddCommand(i_ModuleName, "getSelectionLock", 
		"Get the selection lock value", 
		get_selection_lock);
	pythModules::AddCommand(i_ModuleName, "setSelectionLock", 
		"Set the selection lock value", 
		set_selection_lock);
	pythModules::AddCommand(i_ModuleName, "selectObject", 
		"Select object by name.", 
		select_object);
	pythModules::AddCommand(i_ModuleName, "appendSelect", 
		"Append object by name to the selection list.", 
		append_select);
	pythModules::AddCommand(i_ModuleName, "removeSelect", 
		"Remove object by name from the selection list.", 
		remove_select);

}

#endif
