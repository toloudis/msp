/****************************************************************************\
**	pythControlProperty.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythFgmtProperty.hpp"

#include "Support/dyn/dynScriptObject.hpp"
#include "Support/dyn/GUI/dynPropertyObject.hpp"
#include "Support/pyth/pythPropertyUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/prty/prtyColor.hpp"
#include "Core/prty/prtyDirectory.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyFileName.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyListChecked.hpp"
#include "Core/prty/prtyName.hpp"
#include "Core/prty/prtyRotation.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/gui/guiMessageBox.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace pythControlProperty
{
	PyObject* controlPyObject;
	std::list< shared_ptr<prtyPropertyUIInfo> > m_PropertyList;
	PyObject* NameValue;
	PyObject* PropList;
	
	//--------------------------------------------------------------------
	// Get script object by name
	//--------------------------------------------------------------------
	dynScriptObject* get_script_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(pNameObj);
		sel3dObject* pPickObject = dynamic_cast<sel3dObject*>(pNameObj);
		if (pPickObject && !script_obj)
		{
			// If this is not a scripted object, it may be associated 
			// with a script object through the "parent object" relationship
			relObject* cur_obj = pPickObject;
			while (!script_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
			}
		}

		return dynamic_cast<dynScriptObject*>(script_obj);
	}
	
	//--------------------------------------------------------------------
	// Display our python object, used in other functions to print out a formatted python list
	//--------------------------------------------------------------------
	PyObject* display_control_object(PyObject *self, PyObject *args)
	{
		return controlPyObject;
	}

	//.......................................................
	//Return the fgmtPropertyObject when given the script object and fragment name
	//.......................................................
	dynPropertyObject* get_DynPropByName( dynScriptObject* i_DynObj, const char* i_controlName )
	{
		std::string control_name(i_controlName);
		//override the fragments if not previously done so.
		if( i_DynObj->GetNumControls() == 0 )
			return NULL;

		dynPropertyObject* prop_obj = i_DynObj->GetControlUI(control_name);
		if( prop_obj )
		{
			return prop_obj;
		}

		return NULL;
	}
	//------------------------------------------
	//------------------------------------------
	void add_to_list()
	{
		//add the name of the properties to the pyObject property list
		if( PropList == NULL )
			return;
		PropertyUIIList::const_iterator it;
		it = m_PropertyList.begin();
		for( int i = 0; i < m_PropertyList.size(); ++i )
		{
			std::string prop_name = (*it)->GetProperty(0)->GetPropertyName();
			PyList_SetItem(PropList, i, 
					PyUnicode_FromString(prop_name.c_str()));
			++it;
		}
	}
	
	//------------------------------------------
	//------------------------------------------
	bool isValidJoint(std::string i_JointName, std::vector<std::string> i_JointList)
	{
		for(int i = 0; i < i_JointList.size(); ++i)
		{
			if(i_JointList[i] == i_JointName)
				return true;
		}
		return false;
	}
	//--------------------------------------------------------------------
	// reads from a vector of the objects joints and places them in a python list
	//--------------------------------------------------------------------
	void gather_joints( std::vector<std::string> i_RefNames )
	{
		controlPyObject = PyList_New(i_RefNames.size());
		for( int i = 0; i < i_RefNames.size(); ++i )
		{
			std::string jointName = i_RefNames[i];
			PyList_SetItem(controlPyObject, i, PyUnicode_FromString(jointName.c_str()));
		}
	}
	//--------------------------------------------------------------------
	// reads from a vector of the objects controls and places them in a python list
	//--------------------------------------------------------------------
	void gather_controls( std::vector<dynControlData> i_ControlData )
	{
		controlPyObject = PyList_New( i_ControlData.size() );
		for( int i = 0; i < i_ControlData.size(); ++i )
		{
			std::string controlName = i_ControlData[i].m_Name.GetValue();
			PyList_SetItem(controlPyObject, i, PyUnicode_FromString(controlName.c_str()));
		}

	}

	//--------------------------------------------------------------------
	// Given an object, the control name, and a joint name, create a new control
	//--------------------------------------------------------------------
	PyObject* create_control(PyObject *self, PyObject *args)
	{
		const char* objectName, *controlName, *jointName;
		if (!PyArg_ParseTuple(args, "sss", &objectName, &controlName, &jointName))
			return NULL;
		
		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;

		std::vector<std::string> ref_names;
		script_obj->GetReferenceList(ref_names);
		if (ref_names.empty())
		{
			guiMessageBox::Show("No joints available to attach to.", "No nodes for controls");
			return Py_None;
		}

		std::string cName(controlName);
		std::string jName(jointName);		
		
		if(!isValidJoint(jName, ref_names))
		{
			std::string errMsg = "The joint: " + jName + " does not exist";
			guiMessageBox::Show(errMsg.c_str(), "Node does not exist");
			return Py_None;
		}
		prtyText prtyControlName(cName, cName);
		prtyText prtyJointName(jName, jName);
		
		dynControlData control_obj;
		control_obj.m_Name = prtyControlName;
		control_obj.m_Node = prtyJointName;
		
		script_obj->AddControl(control_obj);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// delete the specified control of an object
	//--------------------------------------------------------------------
	PyObject* delete_control(PyObject *self, PyObject *args)
	{
		const char* objectName, *controlName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &controlName))
			return NULL;
		
		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;

		std::string cName(controlName);

		script_obj->DeleteControl(cName, true);

		return Py_None;
	}
	//--------------------------------------------------------------------
	// get the properties of an object's control
	//--------------------------------------------------------------------
	PyObject* get_control_properties(PyObject *self, PyObject *args)
	{
		const char* objectName, *controlName;
		int prop_list_size = 0;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &controlName))
			return NULL;
		
		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;

		dynPropertyObject* prop_obj = get_DynPropByName(script_obj, controlName);
		if( !prop_obj )
			return Py_None;
		

		m_PropertyList = prop_obj->GetList();
		prop_list_size = m_PropertyList.size();
		PropList = PyList_New( prop_list_size );
		add_to_list();


		return PropList;

	}

	//--------------------------------------------------------------------
	// get the value of a specified control property
	//--------------------------------------------------------------------
	PyObject* get_control_value(PyObject *self, PyObject *args)
	{
		const char* objectName, *controlName, *controlProperty;
		if (!PyArg_ParseTuple(args, "sss", &objectName, &controlName, &controlProperty))
			return NULL;
		
		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;
		
		dynPropertyObject* prop_obj = get_DynPropByName(script_obj, controlName);
		if( !prop_obj )
			return Py_None;

		//get the specified property of the property object
		const std::string control_property(controlProperty);
		const prtyProperty* pProperty = prop_obj->GetProperty(control_property);

		//get its value and store it in the PyObject
		NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
		//PyRun_SimpleString("print mach.displayFgmtProperty()");

		return NameValue;

	}

	//--------------------------------------------------------------------
	// set the value of a specified control property
	//--------------------------------------------------------------------
	PyObject* set_control_value(PyObject *self, PyObject *args)
	{
		int num_args = (int)PyTuple_Size( args );
		if (num_args != 4)
		{
			PyErr_SetString(PyExc_TypeError, "setfragmentValue needs 4 arguments: object name, property name, value (which can be a tuple)");
			return NULL;
		}
		const char *objectName, *controlName, *controlProperty;
		objectName = PyUnicode_AsUTF8( PyTuple_GetItem( args, 0 ) );
		controlName = PyUnicode_AsUTF8( PyTuple_GetItem( args, 1 ) );
		controlProperty = PyUnicode_AsUTF8( PyTuple_GetItem( args, 2 ) );

		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;
		
		dynPropertyObject* prop_obj = get_DynPropByName(script_obj, controlName);
		if( !prop_obj )
			return Py_None;

		//get the specified property of the property object
		const std::string control_property(controlProperty);
		const prtyProperty* pProperty = prop_obj->GetProperty(control_property);

		pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), prop_obj, PyTuple_GetItem( args, 3 ) );
		return Py_None;

	}
	//--------------------------------------------------------------------
	// Get the available joints of an object
	//--------------------------------------------------------------------
	PyObject* get_joint_list(PyObject *self, PyObject *args)
	{
		const char* objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;
		
		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;

		//retrieve joints
		std::vector<std::string> ref_names;
	 	script_obj->GetReferenceList(ref_names);

		gather_joints(ref_names);
		//PyRun_SimpleString("for joint in mach.displayControlObject(): print joint");
		return controlPyObject;
	}

	//--------------------------------------------------------------------
	// Get all controls belonging to an object
	//--------------------------------------------------------------------
	PyObject* get_control_list(PyObject *self, PyObject *args)
	{
		const char* objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return Py_None;

		dynScriptObject* script_obj = get_script_object( objectName );
		if( !script_obj )
			return Py_None;

		std::vector<dynControlData> control_data;
		script_obj->GetControlData( control_data );

		gather_controls(control_data);
		//PyRun_SimpleString("for control in mach.displayControlObject(): print control");
		return controlPyObject;
	}



	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"createControl", "Creates a new control for an object, on a specified joint", create_control);
		pythModules::AddCommand(i_ModuleName, 
			"deleteControl", "Delete a specified control of an object", delete_control);
		pythModules::AddCommand(i_ModuleName, 
			"getControlProperties", "Gets all control properties of a specified object", get_control_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getControlValue", "Gets the value of a specified control property", get_control_value);
		pythModules::AddCommand(i_ModuleName, 
			"setControlValue", "Sets the value of a specified control property", set_control_value);
		pythModules::AddCommand(i_ModuleName, 
			"getAttachNodeList", "Returns a list of all possible joints for an object", get_joint_list);
		pythModules::AddCommand(i_ModuleName, 
			"getControlList", "Returns a list of all controls for an object", get_control_list);

		pythModules::AddCommand(i_ModuleName, 
			"displayControlObject", "returns this modules python object", display_control_object);
	}
}  //end pythControlProperty namespace
#endif  //end if python enabled 