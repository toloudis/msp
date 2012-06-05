/****************************************************************************\
**	pythLightSets.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythLightSets.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Core/ma/maFloatRGBA.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create a new light set
	//--------------------------------------------------------------------
	PyObject *
	create_light_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		if (!PyArg_ParseTuple(args, "s", &lightSetName))
			return NULL;

		std::string set_name(lightSetName);
		if (!ltstLightSetMgr::IsValidLightSetName(set_name))
		{
			PyErr_SetString(PyExc_NameError, "Light Set already exists with given name.");
			return NULL;
		}

		ltstLightSetMgr::CreateLightSet( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Delete a light set by name
	//--------------------------------------------------------------------
	PyObject *
	delete_light_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		if (!PyArg_ParseTuple(args, "s", &lightSetName))
			return NULL;

		std::string set_name(lightSetName);
		ltstLightSetMgr::DeleteLightSet( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Rename a light set
	//--------------------------------------------------------------------
	PyObject *
	rename_light_set(PyObject *self, PyObject *args)
	{
		const char *oldName, *newName;
		if (!PyArg_ParseTuple(args, "ss", &oldName, &newName))
			return NULL;

		ltstLightSetMgr::RenameLightSet( nameString(oldName), std::string(newName) );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Add light to light set
	//--------------------------------------------------------------------
	PyObject *
	assign_light_to_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName, *lightName;
		if (!PyArg_ParseTuple(args, "ss", &lightName, &lightSetName))
			return NULL;

		ltstLightSetMgr::AddLightToSet( nameString(lightSetName), nameString(lightName) );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Add object to light set
	//--------------------------------------------------------------------
	PyObject *
	add_object_to_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName, *objectName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &lightSetName))
			return NULL;

		ltstLightSetMgr::AddObjectToLightSet( nameString(lightSetName), nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Remove object from light set
	//--------------------------------------------------------------------
	PyObject *
	remove_object_from_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName, *objectName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &lightSetName))
			return NULL;

		ltstLightSetMgr::RemoveObjectFromLightSet( nameString(lightSetName), nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}
	
	//--------------------------------------------------------------------
	// Return list of light set names
	//--------------------------------------------------------------------
	PyObject *
	get_light_sets(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		ltstLightSetMgr::GetLightSetNames(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of lights in the given light set
	//--------------------------------------------------------------------
	PyObject *
	get_lights_in_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		if (!PyArg_ParseTuple(args, "s", &lightSetName))
			return NULL;

		nameString set_name(lightSetName);
		std::vector<nameString> name_list;
		ltstLightSetMgr::GetLightsInSet(set_name,name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of objects in the given light set
	//--------------------------------------------------------------------
	PyObject *
	get_objects_in_set(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		if (!PyArg_ParseTuple(args, "s", &lightSetName))
			return NULL;

		nameString set_name(lightSetName);
		std::vector<nameString> name_list;
		ltstLightSetMgr::GetObjectsInSet(set_name,name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of all objects that could be put into a light set
	//--------------------------------------------------------------------
	PyObject *
	get_all_objects(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		ltstLightSetMgr::GetAllObjects(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// Return name of light set that contains the given light
	//--------------------------------------------------------------------
	PyObject *
	get_light_set_for_light(PyObject *self, PyObject *args)
	{
		const char *lightName;
		if (!PyArg_ParseTuple(args, "s", &lightName))
			return NULL;

		nameString set_name;
		if (ltstLightSetMgr::GetSetNameFromLight(nameString(lightName), set_name))
		{
			return pythFunctionUtil::ConvertString( set_name.GetString() );
		}
		else
		{
			return pythFunctionUtil::ConvertString("");
		}
	}

	//--------------------------------------------------------------------
	// Return ambient light color of light set
	//--------------------------------------------------------------------
	PyObject *
	get_light_set_ambient(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		if (!PyArg_ParseTuple(args, "s", &lightSetName))
			return NULL;

		maFloatRGBA ambient;
		
		if (::strlen(lightSetName) == 0)
			ambient = ltstLightSetMgr::GetSceneAmbientLight();
		else
			
			ambient = ltstLightSetMgr::GetLightSetAmbientLight(nameString(lightSetName));

		return pythFunctionUtil::ConvertColor(ambient);
	}

	//--------------------------------------------------------------------
	// Sets ambient light color of light set
	//--------------------------------------------------------------------
	PyObject *
	set_light_set_ambient(PyObject *self, PyObject *args)
	{
		const char *lightSetName;
		float r,g,b,a;
		if (!PyArg_ParseTuple(args, "s(ffff)", &lightSetName, &r, &g, &b, &a))
			return NULL;

		maFloatRGBA ambient(r,g,b,a);
		if (::strlen(lightSetName) == 0)
			ltstLightSetMgr::SetSceneAmbientLight(ambient);
		else	
			ltstLightSetMgr::SetLightSetAmbientLight(nameString(lightSetName), ambient);

		return pythFunctionUtil::ReturnNone();
	}	

}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythLightSets::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"createLightSet", 
		"Create a new light set with the given name."
		"Name must be unique.", 
		create_light_set);
	pythModules::AddCommand(i_ModuleName, 
		"deleteLightSet", 
		"Delete light set with the given name.", 
		delete_light_set);
	pythModules::AddCommand(i_ModuleName, 
		"renameLightSet", 
		"Change name of light set."
		"	renameLightSet(oldName, newName)", 
		rename_light_set);

	pythModules::AddCommand(i_ModuleName, 
		"assignLightToLightSet", 
		"Assign light with given name to light set with given name."
		"	assignLightToLightSet(lightName, lightSetName)"
		"	assignLightToLightSet(lightName, \"\") - removes from light set", 
		assign_light_to_set);

	pythModules::AddCommand(i_ModuleName, 
		"addObjectToLightSet", 
		"Add object with given name to light set with given name."
		"	addObjectToLightSet(objectName, lightSetName)", 
		add_object_to_set);
	pythModules::AddCommand(i_ModuleName, 
		"removeObjectFromLightSet", 
		"Remove object with given name from light set with given name."
		"	removeObjectFromLightSet(objectName, lightSetName)", 
		remove_object_from_set);

	pythModules::AddCommand(i_ModuleName, 
		"getLightSets", 
		"Return list of light set names.", 
		get_light_sets);
	pythModules::AddCommand(i_ModuleName, 
		"getLightsInLightSet", 
		"Return list of lights in the given light set.", 
		get_lights_in_set);
	pythModules::AddCommand(i_ModuleName, 
		"getObjectsInLightSet", 
		"Return list of objects in the given light set.", 
		get_objects_in_set);
	pythModules::AddCommand(i_ModuleName, 
		"getAllObjectsForLightSets", 
		"Return list of all objects that could be put into a light set.", 
		get_all_objects);

	pythModules::AddCommand(i_ModuleName, 
		"getLightSetForLight", 
		"Return name of light set that contains the given light.", 
		get_light_set_for_light);

	pythModules::AddCommand(i_ModuleName, 
		"getLightSetAmbient", 
		"Return ambient light color of light set, use \"\" for scene ambient.", 
		get_light_set_ambient);
	pythModules::AddCommand(i_ModuleName, 
		"setLightSetAmbient", 
		"Sets ambient light color of light set, use \"\" for scene ambient.", 
		set_light_set_ambient);
}

#endif
