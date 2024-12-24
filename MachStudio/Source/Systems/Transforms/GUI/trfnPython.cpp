/****************************************************************************\
**	trfnPython.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/GUI/trfnPython.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/xfrm/xfrmTransformGroup.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Add object to parent
	//--------------------------------------------------------------------
	PyObject *
	add_object_to_parent(PyObject *self, PyObject *args)
	{
		const char *parentName, *objectName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &parentName))
			return NULL;

		trfnOperations::AddNodeToTransform( nameString(parentName), nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Remove object from its parent
	//--------------------------------------------------------------------
	PyObject *
	remove_object_from_parent(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		trfnOperations::RemoveNodeFromParent( nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}
	
	//--------------------------------------------------------------------
	// return list of all parent node names
	//--------------------------------------------------------------------
	PyObject *
	get_parents(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		xfrmTransformMgr::GetTransformNames(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// return list of all root parent node names
	//--------------------------------------------------------------------
	PyObject *
	get_root_parents(PyObject *self, PyObject *args)
	{
		std::vector<xfrmTransformGroup*> xfrm_list;
		xfrmTransformMgr::GetRootTransforms(xfrm_list);

		std::vector<nameString> name_list(xfrm_list.size());
		for (int i=0; i<xfrm_list.size(); ++i)
		{
			name_list[i] = xfrm_list[i]->m_pNameObject->GetName();
		}

		return pythFunctionUtil::ConvertNameList(name_list);
	}


	//--------------------------------------------------------------------
	// return list of all objects (not parents) without a parent.
	//--------------------------------------------------------------------
	PyObject *
	get_parentless_objects(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		xfrmTransformMgr::GetUngroupedObjects(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// Return list of children of the given node
	//--------------------------------------------------------------------
	PyObject *
	get_children(PyObject *self, PyObject *args)
	{
		const char *transformName;
		if (!PyArg_ParseTuple(args, "s", &transformName))
			return NULL;

		nameString transform_name(transformName);
		std::vector<nameString> name_list;
		xfrmTransformMgr::GetNodesInTransform(transform_name, name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// Return list of children of the given node
	//--------------------------------------------------------------------
	PyObject *
	get_parent(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		std::string parent_name;
		nameString object_name(objectName);
		nameString transformName;
		if (xfrmTransformMgr::GetParentForNode(object_name, transformName))
		{
			parent_name = transformName.GetString();
		}
		return pythFunctionUtil::ConvertString(parent_name);
	}

}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void trfnPython::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"addObjectToParent", 
		"Add object with given name to parent transform node with given name."
		"	addObjectToParent(objectName, parentName)", 
		add_object_to_parent);
	pythModules::AddCommand(i_ModuleName, 
		"removeObjectFromParent", 
		"Remove object from its parent."
		"	removeObjectFromParent(objectName)", 
		remove_object_from_parent);

	pythModules::AddCommand(i_ModuleName, 
		"getParents", 
		"Return list of all parent node names.", 
		get_parents);
	pythModules::AddCommand(i_ModuleName, 
		"getRootParents", 
		"Return list of all root node parent names - parent nodes without a parent.", 
		get_root_parents);
	pythModules::AddCommand(i_ModuleName, 
		"getParentlessObjects", 
		"Return list of all objects (not parents) without a parent.", 
		get_parentless_objects);
	pythModules::AddCommand(i_ModuleName, 
		"getChildrenOfNode", 
		"Return list of child nodes of the given parent node." 
		"	getChildrenOfNode(parentName)", 
		get_children);
	pythModules::AddCommand(i_ModuleName, 
		"getParentOfNode", 
		"Return parent node of the given object, empty string if no parent."
		"	getParentOfNode(objectName)", 
		get_parent);

}

#else 
void trfnPython::AddCommands(const std::string& i_ModuleName)
{
}
#endif
