/****************************************************************************\
**	pythMtrlProperty.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythMtrlProperty.hpp"

#include "Support/pyth/pythPropertyUtil.hpp"

#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/dbg/dbgLog.hpp"

#include "Core/prty/prtyAngle.hpp"
#include "Core/prty/prtyColor.hpp"
#include "Core/prty/prtyDirectory.hpp"
#include "Core/prty/prtyFileName.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyInt32.hpp"
#include "Core/prty/prtyListChecked.hpp"
#include "Core/prty/prtyName.hpp"
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRotation.hpp"
#include "Core/prty/prtyText.hpp"
#include "Core/prty/prtyVector3d.hpp"



// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace pythMtrlProperty
{

	std::list< shared_ptr<prtyPropertyUIInfo> > m_PropertyList;
	PyObject* NameValue;
	PyObject* PropList;

	//--------------------------------------------------------------------
	// Get script object by name
	//--------------------------------------------------------------------
	mtrlScriptObject* get_mtrl_script_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		mtrlScriptObject* script_obj = dynamic_cast<mtrlScriptObject*>(pNameObj);
		pick3dPickObject* pPickObject = dynamic_cast<pick3dPickObject*>(pNameObj);
		if (pPickObject && !script_obj)
		{
			// If this is not a scripted object, it may be associated 
			// with a script object through the "parent object" relationship
			pick3dPickObject* cur_obj = pPickObject;
			while (!script_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				script_obj = dynamic_cast<mtrlScriptObject*>(cur_obj);
			}
		}

		return script_obj;
	}


	//.......................................................
	//Return the mtrlPropertyObject when given the script object and material name
	//.......................................................
	mtrlPropertyObject* get_MatPropByName( mtrlScriptObject* i_MatObj, const char* i_MatName )
	{
		std::string mat_name(i_MatName);
		if( i_MatObj->GetNumMaterials() == 0 )
			i_MatObj->GatherMaterials();
		mtrlPropertyObject* prop_obj = i_MatObj->GetMaterialUI( mat_name );
		if( prop_obj )
		{
			return prop_obj;
		}

		return NULL;
	}


	void combine_and_display(PyObject *mtrlPropName, PyObject *mtrlPropValue)
	{
		NameValue = PyList_New( 2 );
		PyList_SetItem(NameValue, 0, mtrlPropName);
		PyList_SetItem(NameValue, 1, mtrlPropValue);
		//DBG_WARNING0( "     *****combined material name and value*******"  );
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
	void display_mtrl_properties()
	{
		//set up the shader list as a python object
		//DBG_WARNING0( "*****Enter display function*******" );
		PropertyUIIList::const_iterator it;
		
		for( it = m_PropertyList.begin(); it != m_PropertyList.end(); ++it )
		{
			std::string prop_name = (*it)->GetProperty(0)->GetPropertyName();
			//DBG_WARNING1( "     *****Property Name: %s *******", prop_name.c_str()  );
			PyObject* mtrlValue = pythPropertyUtil::get_value( (*it)->GetProperty(0) );
			//DBG_WARNING0( "     *****Got the value of the material*******"  );
			combine_and_display( PyString_FromString(prop_name.c_str()), mtrlValue );

			PyRun_SimpleString("mtrl = mach.displayMtrlProperty()");
			PyRun_SimpleString("print mtrl[0], '     ', mtrl[1]");					
		}
		
	}
	//.......................................................
	//Go through each property and output the value
	//.......................................................
	bool list_has_property( const std::string i_PropertyName )
	{
		//Checking to see if the property exists in the current property list
		PropertyUIIList::const_iterator it;
		
		for( it = m_PropertyList.begin(); it != m_PropertyList.end(); ++it )
		{
			std::string cur_name = (*it)->GetProperty(0)->GetPropertyName();
			if( i_PropertyName == cur_name )
				return true;
		}
		return false;
	}
	//.......................................................
	//Take in an object name and material name and return the values for the material
	//.......................................................
	PyObject* print_material_properties(PyObject *self, PyObject *args)
	{
		//DBG_WARNING0( "*****Getting Object Materials*******" );
		const char *object_name, *material_name;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &material_name))
			return NULL;

		//return the mtrlScriptObject based on the name given
		
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;
		//DBG_WARNING0( "*****recognized object returned as mtrlScriptObject*******" );

		//DBG_WARNING1( "*****Getting Material Name, %s, ***", material_name );
		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;
		//DBG_WARNING0( "*****recognized material returned as mtrlPropertyObject*******" );
		
		//get the material data for the material
		mtrlShaderObject* shader_obj = prop_obj->GetShaderDataObject();
		mtrlShaderObject* glow_obj = prop_obj->GetGlowDataObject();
		mtrlShaderObject* fur_obj = prop_obj->GetFurDataObject();
		mtrlShaderObject* outline_obj = prop_obj->GetOutlineDataObject();
		
		//run through each property type and display the information
		PyRun_SimpleString("print '****Shader Data****'");
		m_PropertyList.clear();
		if(shader_obj)
		{
			m_PropertyList = shader_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Glow Data****'");
		m_PropertyList.clear();
		if(glow_obj)
		{
			m_PropertyList = glow_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Fur Data****'");
		m_PropertyList.clear();
		if(fur_obj)
		{
			m_PropertyList = fur_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Outline Data****'");
		m_PropertyList.clear();
		if(outline_obj)
		{
			m_PropertyList = outline_obj->GetList();
			display_mtrl_properties();
		}

		
		//DBG_WARNING0( "*****Printed shade info*******" );

		return Py_None;

	}
	//.......................................................
	//Take in an object name and material name and return the values for the material
	//.......................................................
	PyObject* get_material_properties(PyObject *self, PyObject *args)
	{
		//DBG_WARNING0( "*****Getting Object Materials*******" );
		const char *object_name, *material_name;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &material_name))
			return NULL;

		//return the mtrlScriptObject based on the name given
		
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;
		//DBG_WARNING0( "*****recognized object returned as mtrlScriptObject*******" );

		//DBG_WARNING1( "*****Getting Material Name, %s, ***", material_name );
		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;
		//DBG_WARNING0( "*****recognized material returned as mtrlPropertyObject*******" );
		
		//get the material data for the material
		mtrlShaderObject* shader_obj = prop_obj->GetShaderDataObject();
		mtrlShaderObject* glow_obj = prop_obj->GetGlowDataObject();
		mtrlShaderObject* fur_obj = prop_obj->GetFurDataObject();
		mtrlShaderObject* outline_obj = prop_obj->GetOutlineDataObject();
		
		//run through each property type and display the information
		int prop_list_size = 0; 
		std::list< shared_ptr<prtyPropertyUIInfo> > tempList;
		m_PropertyList.clear();
		if(shader_obj)
		{
			tempList = shader_obj->GetList();
			m_PropertyList.merge(tempList);
		}
		if(glow_obj)
		{
			tempList = glow_obj->GetList();
			m_PropertyList.merge(tempList);
		}
		
		if(fur_obj)
		{
			tempList = fur_obj->GetList();
			m_PropertyList.merge(tempList);
		}
		
		if(outline_obj)
		{
			tempList = outline_obj->GetList();
			m_PropertyList.merge(tempList);
		}
		//first, get the size of the list to set up the python list
		prop_list_size = m_PropertyList.size();
		PropList = PyList_New( prop_list_size );
		add_to_list();
		
		//DBG_WARNING0( "*****Printed shader info*******" );

		return PropList;

	}

	PyObject* get_material_value(PyObject *self, PyObject *args)
	{
		const char *object_name, *material_name, *material_property;

		if (!PyArg_ParseTuple(args, "sss", &object_name, &material_name, &material_property))
			return NULL;

		//get the script object
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//get the property object
		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;

		//get the specified property of the property object
		const std::string mtrl_property(material_property);
		const prtyProperty* pProperty;// = prop_obj->GetProperty(mtrl_property);

		mtrlShaderObject* shader_obj = prop_obj->GetShaderDataObject();
		mtrlShaderObject* glow_obj = prop_obj->GetGlowDataObject();
		mtrlShaderObject* fur_obj = prop_obj->GetFurDataObject();
		mtrlShaderObject* outline_obj = prop_obj->GetOutlineDataObject();

		//determine which property list the requested property belongs to
		if(shader_obj)
		{
			m_PropertyList = shader_obj->GetList();
			if( list_has_property( mtrl_property ) )
			{
				pProperty = shader_obj->GetProperty(mtrl_property);
				NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
				return NameValue;
			}

		}
		
		if(glow_obj)
		{
			m_PropertyList = glow_obj->GetList();
			if( list_has_property( mtrl_property ) )
			{
				pProperty = glow_obj->GetProperty(mtrl_property);
				NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
				return NameValue;
			}

		}

		if(fur_obj)
		{
			m_PropertyList = fur_obj->GetList();
			if( list_has_property( mtrl_property ) )
			{
				pProperty = fur_obj->GetProperty(mtrl_property);
				NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
				return NameValue;
			}

		}

		if(outline_obj)
		{
			m_PropertyList = outline_obj->GetList();
			if( list_has_property( mtrl_property ) )
			{
				pProperty = outline_obj->GetProperty(mtrl_property);
				NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
				return NameValue;
			}
		}

		return Py_None;
	}

	PyObject* set_material_value(PyObject *self, PyObject *args)
	{
		int num_args = PyTuple_Size( args );
		if (num_args != 4)
		{
			PyErr_SetString(PyExc_TypeError, "setMaterialValue needs 4 arguments: object name, property name, value (which can be a tuple)");
			return NULL;
		}
		const char *object_name, *material_name, *material_property;
		object_name = PyString_AsString( PyTuple_GetItem( args, 0 ) );
		material_name = PyString_AsString( PyTuple_GetItem( args, 1 ) );
		material_property = PyString_AsString( PyTuple_GetItem( args, 2 ) );

		//get the script object
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//get the property object
		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;

		//get the specified property of the property object
		const std::string mtrl_property(material_property);
		const prtyProperty* pProperty = prop_obj->GetProperty(mtrl_property);

		pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), PyTuple_GetItem( args, 3 ) );
		return Py_None;
	}

	//.......................................................
	//Return all materials belonging to an object
	//.......................................................
	PyObject* get_all_materials(PyObject *self, PyObject *args)
	{
		const char *object_name;
		if (!PyArg_ParseTuple(args, "s", &object_name))
			return NULL;

		//return the fgmtScriptObject based on the name given
		
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//make sure all fragments have been grabbed for the object
		if( script_obj->GetNumMaterials() == 0 )
			script_obj->GatherMaterials();
		int numMtrl = script_obj->GetNumMaterials();
		NameValue = PyList_New(numMtrl);
		std::string next_mtrl;
		for( unsigned int i = 0; i < numMtrl; ++i )
		{
			next_mtrl = script_obj->GetMaterialName(i);
			PyList_SetItem(NameValue, i, PyString_FromString(next_mtrl.c_str()));
		}
		return NameValue;
	}

	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"printMaterialProperties", "Print formatted list of material properties for an object.", print_material_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getMaterialProperties", "Get list of material properties for an object.", get_material_properties);
		pythModules::AddCommand(i_ModuleName, 
			"displayMtrlProperty", "Displays the value of a property", property_list);
		pythModules::AddCommand(i_ModuleName, 
			"getMaterialValue", "Returns the value of a specified material property of an object", get_material_value);
		pythModules::AddCommand(i_ModuleName, 
			"setMaterialValue", "Returns the value of a specified material property of an object", set_material_value);
		pythModules::AddCommand(i_ModuleName, 
			"getAllMaterials", "Returns all materials of an object", get_all_materials);
	}

}  //end pythMtrlProperty namespace

#endif
