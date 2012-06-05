/****************************************************************************\
**	pythLayers.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythLayers.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/lyer/lyerLayerMgr.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create a new layer
	//--------------------------------------------------------------------
	PyObject *
	create_layer(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		std::string set_name(layerName);
		if (!lyerLayerMgr::IsValidLayerName(set_name))
		{
			PyErr_SetString(PyExc_NameError, "Layer already exists with given name.");
			return NULL;
		}

		lyerLayerMgr::CreateLayer( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Delete a layer by name
	//--------------------------------------------------------------------
	PyObject *
	delete_layer(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		std::string set_name(layerName);
		lyerLayerMgr::DeleteLayer( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Rename a layer
	//--------------------------------------------------------------------
	PyObject *
	rename_layer(PyObject *self, PyObject *args)
	{
		const char *oldName, *newName;
		if (!PyArg_ParseTuple(args, "ss", &oldName, &newName))
			return NULL;

		lyerLayerMgr::RenameLayer( nameString(oldName), std::string(newName) );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Add object to layer
	//--------------------------------------------------------------------
	PyObject *
	assign_object_to_layer(PyObject *self, PyObject *args)
	{
		const char *layerName, *objectName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &layerName))
			return NULL;

		lyerLayerMgr::AddObjectToLayer( nameString(layerName), nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Return list of layer names
	//--------------------------------------------------------------------
	PyObject *
	get_layers(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		lyerLayerMgr::GetLayerNames(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of objects in the given layer
	//--------------------------------------------------------------------
	PyObject *
	get_objects_in_layer(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		nameString set_name(layerName);
		std::vector<nameString> name_list;
		lyerLayerMgr::GetObjectsInLayer(set_name,name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of all objects that could be put into a layer
	//--------------------------------------------------------------------
	PyObject *
	get_all_objects(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		lyerLayerMgr::GetAllObjects(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// Return name of layer that contains the given object
	//--------------------------------------------------------------------
	PyObject *
	get_layer_for_object(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		nameString set_name;
		if (lyerLayerMgr::GetLayerNameFromObject(nameString(objectName), set_name))
		{
			return pythFunctionUtil::ConvertString(set_name.GetString());
		}
		else
		{
			return pythFunctionUtil::ConvertString("");
		}
	}

	//--------------------------------------------------------------------
	// Return visible state of layer
	//--------------------------------------------------------------------
	PyObject *
	get_layer_visible(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		bool bVisible = lyerLayerMgr::GetLayerVisible(nameString(layerName));
		return pythFunctionUtil::ConvertBoolean(bVisible);
	}

	//--------------------------------------------------------------------
	// Sets visible state of layer
	//--------------------------------------------------------------------
	PyObject *
	set_layer_visible(PyObject *self, PyObject *args)
	{
		const char *layerName;
		bool bVisible;
		if (!PyArg_ParseTuple(args, "sb", &layerName, &bVisible))
			return NULL;

		lyerLayerMgr::SetLayerVisible(nameString(layerName), bVisible);
		return pythFunctionUtil::ReturnNone();
	}	

	//--------------------------------------------------------------------
	// Return pickable state of layer
	//--------------------------------------------------------------------
	PyObject *
	get_layer_pickable(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		bool bPickable = lyerLayerMgr::GetLayerPickable(nameString(layerName));
		return pythFunctionUtil::ConvertBoolean(bPickable);
	}

	//--------------------------------------------------------------------
	// Sets pickable state of layer
	//--------------------------------------------------------------------
	PyObject *
	set_layer_pickable(PyObject *self, PyObject *args)
	{
		const char *layerName;
		bool bPickable;
		if (!PyArg_ParseTuple(args, "sb", &layerName, &bPickable))
			return NULL;

		lyerLayerMgr::SetLayerPickable(nameString(layerName), bPickable);
		return pythFunctionUtil::ReturnNone();
	}	

	//--------------------------------------------------------------------
	// Return wireframe state of layer
	//--------------------------------------------------------------------
	PyObject *
	get_layer_wireframe(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		bool bWireframe = lyerLayerMgr::GetLayerWireframe(nameString(layerName));
		return pythFunctionUtil::ConvertBoolean(bWireframe);
	}

	//--------------------------------------------------------------------
	// Sets wireframe state of layer
	//--------------------------------------------------------------------
	PyObject *
	set_layer_wireframe(PyObject *self, PyObject *args)
	{
		const char *layerName;
		bool bWireframe;
		if (!PyArg_ParseTuple(args, "sb", &layerName, &bWireframe))
			return NULL;

		lyerLayerMgr::SetLayerWireframe(nameString(layerName), bWireframe);
		return pythFunctionUtil::ReturnNone();
	}	
	//--------------------------------------------------------------------
	// Return lowres state of layer
	//--------------------------------------------------------------------
	PyObject *
	get_layer_lowres(PyObject *self, PyObject *args)
	{
		const char *layerName;
		if (!PyArg_ParseTuple(args, "s", &layerName))
			return NULL;

		bool bLowRes = lyerLayerMgr::GetLayerLowRes(nameString(layerName));
		return pythFunctionUtil::ConvertBoolean(bLowRes);
	}

	//--------------------------------------------------------------------
	// Sets lowres state of layer
	//--------------------------------------------------------------------
	PyObject *
	set_layer_lowres(PyObject *self, PyObject *args)
	{
		const char *layerName;
		bool bLowRes;
		if (!PyArg_ParseTuple(args, "sb", &layerName, &bLowRes))
			return NULL;

		lyerLayerMgr::SetLayerLowRes(nameString(layerName), bLowRes);
		return pythFunctionUtil::ReturnNone();
	}	

}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythLayers::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"createLayer", 
		"Create a new layer with the given name."
		"Name must be unique.", 
		create_layer);
	pythModules::AddCommand(i_ModuleName, 
		"deleteLayer", 
		"Delete layer with the given name.", 
		delete_layer);
	pythModules::AddCommand(i_ModuleName, 
		"renameLayer", 
		"Change name of layer."
		"	renameLayer(oldName, newName)", 
		rename_layer);

	pythModules::AddCommand(i_ModuleName, 
		"assignObjectToLayer", 
		"Assign object with given name to layer with given name."
		"	assignObjectToLayer(objectName, layerName)"
		"	assignObjectToLayer(objectName, \"\") - removes from layer", 
		assign_object_to_layer);

	pythModules::AddCommand(i_ModuleName, 
		"getLayers", 
		"Return list of layer names.", 
		get_layers);
	pythModules::AddCommand(i_ModuleName, 
		"getObjectsInLayer", 
		"Return list of objects in the given layer.", 
		get_objects_in_layer);
	pythModules::AddCommand(i_ModuleName, 
		"getAllObjectsForLayers", 
		"Return list of all objects that could be put into a layer.", 
		get_all_objects);

	pythModules::AddCommand(i_ModuleName, 
		"getLayerForObject", 
		"Return name of layer that contains the given object.", 
		get_layer_for_object);

	pythModules::AddCommand(i_ModuleName, "getLayerVisible", 
		"Return visible state of layer.", get_layer_visible);
	pythModules::AddCommand(i_ModuleName, "setLayerVisible", 
		"Sets visible state of layer.", set_layer_visible);
	pythModules::AddCommand(i_ModuleName, "getLayerPickable", 
		"Return pickable state of layer.", get_layer_pickable);
	pythModules::AddCommand(i_ModuleName, "setLayerPickable", 
		"Sets pickable state of layer.", set_layer_pickable);
	pythModules::AddCommand(i_ModuleName, "getLayerWireframe", 
		"Return wireframe state of layer.", get_layer_wireframe);
	pythModules::AddCommand(i_ModuleName, "setLayerWireframe", 
		"Sets wireframe state of layer.", set_layer_wireframe);
	pythModules::AddCommand(i_ModuleName, "getLayerLowRes", 
		"Return low resolution state of layer.", get_layer_lowres);
	pythModules::AddCommand(i_ModuleName, "setLayerLowRes", 
		"Sets low resolution state of layer.", set_layer_lowres);
}

#endif
