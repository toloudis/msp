/****************************************************************************\
**	pythFgmtProperty.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythFgmtProperty.hpp"

#include "Support/pyth/pythPropertyUtil.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"

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
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRotation.hpp"
//#include "Core/prty/prtyText.hpp"
#include "Core/prty/prtyVector3d.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace pythFgmtProperty
{
	std::list< shared_ptr<prtyPropertyUIInfo> > m_PropertyList;
	PyObject* NameValue;
	PyObject* PropList;

	//--------------------------------------------------------------------
	// Get script object by name
	//--------------------------------------------------------------------
	fgmtScriptObject* get_fgmt_script_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		fgmtScriptObject* script_obj = dynamic_cast<fgmtScriptObject*>(pNameObj);
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
				script_obj = dynamic_cast<fgmtScriptObject*>(cur_obj);
			}
		}

		return script_obj;
	}

	
	//.......................................................
	//Return the fgmtPropertyObject when given the script object and fragment name
	//.......................................................
	fgmtPropertyObject* get_FgmtPropByName( fgmtScriptObject* i_FgmtObj, const char* i_FgmtName )
	{
		std::string fgmt_name(i_FgmtName);
		//override the fragments if not previously done so.
		if( i_FgmtObj->GetNumFragments() == 0 )
			i_FgmtObj->GatherFragments();
		fgmtPropertyObject* prop_obj = i_FgmtObj->GetFragmentUI( fgmt_name );
		if( prop_obj )
		{
			return prop_obj;
		}

		return NULL;
	}

	
	void combine_and_display(PyObject *fgmtPropName, PyObject *fgmtPropValue)
	{
		NameValue = PyList_New( 2 );
		PyList_SetItem(NameValue, 0, fgmtPropName);
		PyList_SetItem(NameValue, 1, fgmtPropValue);
		//DBG_WARNING0( "     *****combined fragment name and value*******"  );
	}


	PyObject* property_list(PyObject *self, PyObject *args)
	{
		return NameValue;
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
					PyString_FromString(prop_name.c_str()));
			++it;
		}
	}

	//.......................................................
	//Go through each property and output the value
	//.......................................................
	void display_fgmt_properties()
	{
		//set up the shader list as a python object
		//DBG_WARNING0( "*****Enter display function*******" );
		PropertyUIIList::const_iterator it;
		
		for( it = m_PropertyList.begin(); it != m_PropertyList.end(); ++it )
		{
			std::string prop_name = (*it)->GetProperty(0)->GetPropertyName();
			//DBG_WARNING1( "     *****Property Name: %s *******", prop_name.c_str()  );
			PyObject* fgmtValue = pythPropertyUtil::get_value( (*it)->GetProperty(0) );    
			//DBG_WARNING0( "     *****Got the value of the fragment*******"  );
			combine_and_display( PyString_FromString(prop_name.c_str()), fgmtValue );
			PyRun_SimpleString("fgmt = mach.displayFgmtProperty()");
			PyRun_SimpleString("print fgmt[0], '     ', fgmt[1]");	
		}
		
	}
	//.......................................................
	//Take in an object name and fragment name and return the values for the fragment
	//.......................................................
	PyObject* print_fragment_properties(PyObject *self, PyObject *args)
	{
		//DBG_WARNING0( "*****Getting Object Surfaces*******" );
		const char *object_name, *fragment_name;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &fragment_name))
			return NULL;

		//return the fgmtScriptObject based on the name given
		
		fgmtScriptObject* script_obj = get_fgmt_script_object(object_name);
		if( !script_obj )
			return Py_None;
		//DBG_WARNING0( "*****recognized object returned as fgmtScriptObject*******" );

		//DBG_WARNING1( "*****Getting fragment Name, %s, ***", fragment_name );
		fgmtPropertyObject* prop_obj = get_FgmtPropByName(script_obj, fragment_name);
		if( !prop_obj )
			return Py_None;
		//DBG_WARNING0( "*****recognized fragment returned as fgmtPropertyObject*******" );
		
		
		//run through each property type and display the information
		m_PropertyList.clear();
				
		m_PropertyList = prop_obj->GetList();
		display_fgmt_properties();
		


		return Py_None;

	}

	//.......................................................
	//Take in an object name and fragment name and return the values for the fragment
	//.......................................................
	PyObject* get_fragment_properties(PyObject *self, PyObject *args)
	{
		//DBG_WARNING0( "*****Getting Object Surfaces*******" );
		const char *object_name, *fragment_name;
		int prop_list_size = 0;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &fragment_name))
			return NULL;

		//return the fgmtScriptObject based on the name given
		
		fgmtScriptObject* script_obj = get_fgmt_script_object(object_name);
		if( !script_obj )
			return Py_None;
		//DBG_WARNING0( "*****recognized object returned as fgmtScriptObject*******" );

		//DBG_WARNING1( "*****Getting fragment Name, %s, ***", fragment_name );
		fgmtPropertyObject* prop_obj = get_FgmtPropByName(script_obj, fragment_name);
		if( !prop_obj )
			return Py_None;
		//DBG_WARNING0( "*****recognized fragment returned as fgmtPropertyObject*******" );
		
		
		//run through each property type and display the information
		m_PropertyList.clear();
				
		m_PropertyList = prop_obj->GetList();
		prop_list_size = m_PropertyList.size();
		PropList = PyList_New( prop_list_size );
		add_to_list();
		


		return PropList;

	}

	//.......................................................
	//Take an object name, fragment name and a specific fragment property and return its value
	//.......................................................

	PyObject* get_fragment_value(PyObject *self, PyObject *args)
	{
		const char *object_name, *fragment_name, *fragment_property;

		if (!PyArg_ParseTuple(args, "sss", &object_name, &fragment_name, &fragment_property))
			return NULL;

		//get the script object
		fgmtScriptObject* script_obj = get_fgmt_script_object(object_name);
		if( !script_obj )
			return NULL;

		//get the property object
		fgmtPropertyObject* prop_obj = get_FgmtPropByName(script_obj, fragment_name);
		if( !prop_obj )
			return NULL;

		//get the specified property of the property object
		const std::string fgmt_property(fragment_property);
		const prtyProperty* pProperty = prop_obj->GetProperty(fgmt_property);

		//get its value and store it in the PyObject
		NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
		//PyRun_SimpleString("print mach.displayFgmtProperty()");

		return NameValue;

	}

	//.......................................................
	//Take an object name, fragment name and a specific fragment property and set its value to a specified value
	//.......................................................
	PyObject* set_fragment_value(PyObject *self, PyObject *args)
	{
		int num_args = (int)PyTuple_Size( args );
		if (num_args != 4)
		{
			PyErr_SetString(PyExc_TypeError, "setfragmentValue needs 4 arguments: object name, property name, value (which can be a tuple)");
			return NULL;
		}
		const char *object_name, *fragment_name, *fragment_property;
		object_name = PyString_AsString( PyTuple_GetItem( args, 0 ) );
		fragment_name = PyString_AsString( PyTuple_GetItem( args, 1 ) );
		fragment_property = PyString_AsString( PyTuple_GetItem( args, 2 ) );

		//get the script object
		fgmtScriptObject* script_obj = get_fgmt_script_object(object_name);
		if( !script_obj )
			return NULL;
		
		//get the property object
		fgmtPropertyObject* prop_obj = get_FgmtPropByName(script_obj, fragment_name);
		if( !prop_obj )
			return NULL;

		//get the specified property of the property object
		const std::string fgmt_property(fragment_property);
		const prtyProperty* pProperty = prop_obj->GetProperty(fgmt_property);

		pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), prop_obj, PyTuple_GetItem( args, 3 ) );
		return Py_None;
	}
	//.......................................................
	//Return all fragments belonging to an object
	//.......................................................
	PyObject* get_all_fragments(PyObject *self, PyObject *args)
	{
		const char *object_name;
		if (!PyArg_ParseTuple(args, "s", &object_name))
			return NULL;

		//return the fgmtScriptObject based on the name given
		
		fgmtScriptObject* script_obj = get_fgmt_script_object(object_name);
		if( !script_obj )
			return NULL;

		//make sure all fragments have been grabbed for the object
		if( script_obj->GetNumFragments() == 0 )
			script_obj->GatherFragments();
		int numFgmt = script_obj->GetNumFragments();
		NameValue = PyList_New(numFgmt);
		std::string next_fgmt;
		for( unsigned int i = 0; i < numFgmt; ++i )
		{
			next_fgmt = script_obj->GetFragmentName(i);
			PyList_SetItem(NameValue, i, PyString_FromString(next_fgmt.c_str()));
		}
		return NameValue;
	}



	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"printSurfaceProperties", "print a list of fragment properties and their values for an object.", print_fragment_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getSurfaceProperties", "Get list of fragment properties for an object.", get_fragment_properties);
		pythModules::AddCommand(i_ModuleName, 
			"displayFgmtProperty", "Displays the value of a property", property_list);
		pythModules::AddCommand(i_ModuleName, 
			"getSurfaceValue", "Returns the value of a specified fragment property of an object", get_fragment_value);
		pythModules::AddCommand(i_ModuleName, 
			"setSurfaceValue", "Returns the value of a specified fragment property of an object", set_fragment_value);
		pythModules::AddCommand(i_ModuleName, 
			"getAllSurfaces", "Returns all surfaces of an object", get_all_fragments);
	}

}  //end pythfgmtProperty namespace

#endif