/****************************************************************************\
**	pythEnvironments.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythEnvironments.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//--------------------------------------------------------------------
	// Utility for nameString to float "get" function
	//--------------------------------------------------------------------
	PyObject* get_float_value(float (*i_pFunction)(const nameString&), PyObject *args)
	{
		const char *environmentName;
		if (!PyArg_ParseTuple(args, "s", &environmentName))
			return NULL;

		return Py_BuildValue("f", i_pFunction(nameString(environmentName)) );
	}
	//--------------------------------------------------------------------
	// Utility for nameString and float "set" function
	//--------------------------------------------------------------------
	PyObject* set_float_value(void (*i_pFunction)(const nameString&, float), PyObject *args)
	{
		const char *environmentName;
		float val;
		if (!PyArg_ParseTuple(args, "sf", &environmentName, &val))
			return NULL;

		i_pFunction(nameString(environmentName), val);
		return pythFunctionUtil::ReturnNone();
	}	

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create a new environment
	//--------------------------------------------------------------------
	PyObject *
	create_environment(PyObject *self, PyObject *args)
	{
		const char *environmentName;
		if (!PyArg_ParseTuple(args, "s", &environmentName))
			return NULL;

		std::string set_name(environmentName);
		if (!evmtEnvironmentMgr::IsValidEnvironmentName(set_name))
		{
			PyErr_SetString(PyExc_NameError, "Environment already exists with given name.");
			return NULL;
		}

//		evmtEnvironmentMgr::CreateEnvironment( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Delete a environment by name
	//--------------------------------------------------------------------
	PyObject *
	delete_environment(PyObject *self, PyObject *args)
	{
		const char *environmentName;
		if (!PyArg_ParseTuple(args, "s", &environmentName))
			return NULL;

		std::string set_name(environmentName);
//		evmtEnvironmentMgr::DeleteEnvironment( set_name );
		
		return pythFunctionUtil::ReturnNone();
	}
	//--------------------------------------------------------------------
	// Rename a environment
	//--------------------------------------------------------------------
	//PyObject *
	//rename_environment(PyObject *self, PyObject *args)
	//{
	//	const char *oldName, *newName;
	//	if (!PyArg_ParseTuple(args, "ss", &oldName, &newName))
	//		return NULL;

	//	evmtEnvironmentMgr::RenameEnvironment( nameString(oldName), std::string(newName) );
	//	
	//	return pythFunctionUtil::ReturnNone();
	//}

	//--------------------------------------------------------------------
	// Add object to environment
	//--------------------------------------------------------------------
	PyObject *
	assign_object_to_environment(PyObject *self, PyObject *args)
	{
		const char *environmentName, *objectName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &environmentName))
			return NULL;

//		evmtEnvironmentMgr::AddObjectToEnvironment( nameString(environmentName), nameString(objectName) );
		
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Return list of environment names
	//--------------------------------------------------------------------
	PyObject *
	get_environments(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		evmtEnvironmentMgr::GetEnvironmentNames(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of objects in the given environment
	//--------------------------------------------------------------------
	PyObject *
	get_objects_in_environment(PyObject *self, PyObject *args)
	{
		const char *environmentName;
		if (!PyArg_ParseTuple(args, "s", &environmentName))
			return NULL;

		nameString set_name(environmentName);
		std::vector<nameString> name_list;
		evmtEnvironmentMgr::GetObjectsInSet(set_name,name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}
	//--------------------------------------------------------------------
	// Return list of all objects that could be put into a environment
	//--------------------------------------------------------------------
	PyObject *
	get_all_objects(PyObject *self, PyObject *args)
	{
		std::vector<nameString> name_list;
		evmtEnvironmentMgr::GetAllObjects(name_list);
		return pythFunctionUtil::ConvertNameList(name_list);
	}

	//--------------------------------------------------------------------
	// Return name of environment that contains the given object
	//--------------------------------------------------------------------
	PyObject *
	get_environment_for_object(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		nameString set_name;
		if (evmtEnvironmentMgr::GetEnvironmentNameFromObject(nameString(objectName), set_name))
		{
			return pythFunctionUtil::ConvertString(set_name.GetString());
		}
		else
		{
			return pythFunctionUtil::ConvertString("");
		}
	}

	//--------------------------------------------------------------------
	// Diffuse factor
	//--------------------------------------------------------------------
	PyObject* get_environment_diffuse_factor(PyObject *self, PyObject *args)
	{
		return NULL;//get_float_value(evmtEnvironmentMgr::GetDiffuseFactor, args);
	}
	PyObject* set_environment_diffuse_factor(PyObject *self, PyObject *args)
	{
		return NULL;//set_float_value(evmtEnvironmentMgr::SetDiffuseFactor, args);
	}	
	//--------------------------------------------------------------------
	// Specular factor
	//--------------------------------------------------------------------
	PyObject* get_environment_specular_factor(PyObject *self, PyObject *args)
	{
		return NULL;//get_float_value(evmtEnvironmentMgr::GetSpecularFactor, args);
	}
	PyObject* set_environment_specular_factor(PyObject *self, PyObject *args)
	{
		return NULL;//set_float_value(evmtEnvironmentMgr::SetSpecularFactor, args);
	}	

	//--------------------------------------------------------------------
	// Diffuse angle
	//--------------------------------------------------------------------
	PyObject* get_environment_diffuse_angle(PyObject *self, PyObject *args)
	{
		return NULL;//get_float_value(evmtEnvironmentMgr::GetDiffuseAngle, args);
	}
	PyObject* set_environment_diffuse_angle(PyObject *self, PyObject *args)
	{
		return NULL;//set_float_value(evmtEnvironmentMgr::SetDiffuseAngle, args);
	}	
	//--------------------------------------------------------------------
	// Specular angle
	//--------------------------------------------------------------------
	PyObject* get_environment_specular_angle(PyObject *self, PyObject *args)
	{
		return NULL;//get_float_value(evmtEnvironmentMgr::GetSpecularAngle, args);
	}
	PyObject* set_environment_specular_angle(PyObject *self, PyObject *args)
	{
		return NULL;//set_float_value(evmtEnvironmentMgr::SetSpecularAngle, args);
	}	

}	// end of namespace

//--------------------------------------------------------------------
// Add commands to given module
//--------------------------------------------------------------------
void pythEnvironments::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"createEnvironment", 
		"Create a new environment with the given name."
		"Name must be unique.", 
		create_environment);
	pythModules::AddCommand(i_ModuleName, 
		"deleteEnvironment", 
		"Delete environment with the given name.", 
		delete_environment);
	//pythModules::AddCommand(i_ModuleName, 
	//	"renameEnvironment", 
	//	"Change name of environment."
	//	"	renameEnvironment(oldName, newName)", 
	//	rename_environment);

	pythModules::AddCommand(i_ModuleName, 
		"assignObjectToEnvironment", 
		"Assign object with given name to environment with given name."
		"	assignObjectToEnvironment(objectName, environmentName)"
		"	assignObjectToEnvironment(objectName, \"Default\") - move to base environment", 
		assign_object_to_environment);

	pythModules::AddCommand(i_ModuleName, 
		"getEnvironments", 
		"Return list of environment names.", 
		get_environments);
	pythModules::AddCommand(i_ModuleName, 
		"getObjectsInEnvironment", 
		"Return list of objects in the given environment.", 
		get_objects_in_environment);
	pythModules::AddCommand(i_ModuleName, 
		"getAllObjectsForEnvironments", 
		"Return list of all objects that could be put into a environment.", 
		get_all_objects);

	pythModules::AddCommand(i_ModuleName, 
		"getEnvironmentForObject", 
		"Return name of environment that contains the given object.", 
		get_environment_for_object);


	pythModules::AddCommand(i_ModuleName, "getEnvironmentDiffuseFactor", 
		"Return diffuse factor of environment.", get_environment_diffuse_factor);
	pythModules::AddCommand(i_ModuleName, "setEnvironmentDiffuseFactor", 
		"Sets diffuse factor of environment.", set_environment_diffuse_factor);
	pythModules::AddCommand(i_ModuleName, "getEnvironmentSpecularFactor", 
		"Return specular factor of environment.", get_environment_specular_factor);
	pythModules::AddCommand(i_ModuleName, "setEnvironmentSpecularFactor", 
		"Sets specular factor of environment.", set_environment_specular_factor);

	pythModules::AddCommand(i_ModuleName, "getEnvironmentDiffuseAngle", 
		"Return diffuse angle of environment.", get_environment_diffuse_angle);
	pythModules::AddCommand(i_ModuleName, "setEnvironmentDiffuseAngle", 
		"Sets diffuse angle of environment.", set_environment_diffuse_angle);
	pythModules::AddCommand(i_ModuleName, "getEnvironmentSpecularAngle", 
		"Return specular angle of environment.", get_environment_specular_angle);
	pythModules::AddCommand(i_ModuleName, "setEnvironmentSpecularAngle", 
		"Sets specular angle of environment.", set_environment_specular_angle);
}
#endif
