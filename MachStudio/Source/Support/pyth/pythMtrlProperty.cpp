/****************************************************************************\
**	pythMtrlProperty.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythMtrlProperty.hpp"

#include "Support/pyth/pythPropertyUtil.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
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
#include "Tool/gui/guiMessageBox.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
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
					PyUnicode_FromString(prop_name.c_str()));
			++it;
		}
	}

	//.......................................................
	//Go through each property and output the value
	//.......................................................
	void display_mtrl_properties()
	{
		//set up the shader list as a python object
		PropertyUIIList::const_iterator it;
		
		for( it = m_PropertyList.begin(); it != m_PropertyList.end(); ++it )
		{
			std::string prop_name = (*it)->GetProperty(0)->GetPropertyName();
			PyObject* mtrlValue = pythPropertyUtil::get_value( (*it)->GetProperty(0) );
			combine_and_display( PyUnicode_FromString(prop_name.c_str()), mtrlValue );

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
		const char *object_name, *material_name;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &material_name))
			return NULL;

		//return the mtrlScriptObject based on the name given
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;
		
		//get the material data for the material
		mtrlShaderObject* shader_obj = prop_obj->GetShaderDataObject();
		mtrlShaderObject* glow_obj = prop_obj->GetGlowDataObject();
		mtrlShaderObject* outline_obj = prop_obj->GetOutlineDataObject();
		mtrlShaderObject* uv_obj = prop_obj->GetUVTransformObject();
		mtrlShaderObject* reflection_obj = prop_obj->GetReflectionDataObject();
		mtrlShaderObject* displacement_obj = prop_obj->GetDisplacementDataObject();
		mtrlShaderObject* normals_obj = prop_obj->GetNormalsDataObject();
		

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

		PyRun_SimpleString("print '****Outline Data****'");
		m_PropertyList.clear();
		if(outline_obj)
		{
			m_PropertyList = outline_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****UV Data****'");
		m_PropertyList.clear();
		if(uv_obj)
		{
			m_PropertyList = uv_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Reflection Data****'");
		m_PropertyList.clear();
		if(reflection_obj)
		{
			m_PropertyList = reflection_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Displacement Data****'");
		m_PropertyList.clear();
		if(displacement_obj)
		{
			m_PropertyList = displacement_obj->GetList();
			display_mtrl_properties();
		}

		PyRun_SimpleString("print '****Normal Maps Data****'");
		m_PropertyList.clear();
		if(normals_obj)
		{
			m_PropertyList = normals_obj->GetList();
			display_mtrl_properties();
		}

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

		//DBG_WARNING1( "*****Getting Material Name, %s, ***", material_name );
		mtrlPropertyObject* prop_obj = get_MatPropByName(script_obj, material_name);
		if( !prop_obj )
			return NULL;
		
		//run through each property type and display the information
		int prop_list_size = 0; 
		std::list< shared_ptr<prtyPropertyUIInfo> > tempList;
		m_PropertyList.clear();

		//get all properties belonging to the material
		tempList = prop_obj->GetBaseUI().GetList();
		m_PropertyList.merge(tempList);

		//first, get the size of the list to set up the python list
		prop_list_size = m_PropertyList.size();
		PropList = PyList_New( prop_list_size );
		add_to_list();

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

		//m_PropertyList = prop_obj->GetBaseUI().GetList();
		//if( list_has_property( mtrl_property ) )
		{
			const prtyProperty* pProperty = prop_obj->GetBaseUI().GetProperty(mtrl_property);
			NameValue = pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
			return NameValue;
		}
		

		return Py_None;
	}

	PyObject* set_material_value(PyObject *self, PyObject *args)
	{
		int num_args = (int)PyTuple_Size( args );
		if (num_args != 4)
		{
			PyErr_SetString(PyExc_TypeError, "setMaterialValue needs 4 arguments: object name, material name, property name, value (which can be a tuple)");
			return NULL;
		}
		const char *object_name, *material_name, *material_property;
		object_name = PyUnicode_AsUTF8( PyTuple_GetItem( args, 0 ) );
		material_name = PyUnicode_AsUTF8( PyTuple_GetItem( args, 1 ) );
		material_property = PyUnicode_AsUTF8( PyTuple_GetItem( args, 2 ) );

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
		const prtyProperty* pProperty = prop_obj->GetBaseUI().GetProperty(mtrl_property);

		pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), prop_obj, PyTuple_GetItem( args, 3 ) );
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
			PyList_SetItem(NameValue, i, PyUnicode_FromString(next_mtrl.c_str()));
		}
		return NameValue;
	}

	//------------------------------------------------------------------------
	//Return the current index of a selected material, returns -1 if none
	//------------------------------------------------------------------------
	PyObject* get_selected_material_index(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("i", mtrlOperations::GetSelectedMaterialIndex());
	}

	//------------------------------------------------------------------------
	//Return the list of selected materials and their parent objects
	//------------------------------------------------------------------------
	PyObject* get_selected_material_list(PyObject *self, PyObject *args)
	{
		std::vector<mtrlOperations::MaterialParentPair> pair_list = mtrlOperations::GetSelectedMaterialPairList();
		//define python objects, our main list, the tuple to hold each pair, and the individual data objects
		PyObject* data_list = PyList_New( pair_list.size() );
		PyObject* data_pair; 
		PyObject* mat_name;
		PyObject* obj_name;
		for( int i = 0; i < pair_list.size(); ++i )
		{
			//create a new tuple for the current pair
			data_pair = PyTuple_New( 2 );

			//get the values for each pair from the vector
			obj_name = pythFunctionUtil::ConvertString(pair_list[i].m_ParentName);
			mat_name = pythFunctionUtil::ConvertString(pair_list[i].m_MaterialName);

			//set the items into the tuple
			PyTuple_SetItem(data_pair, 0, obj_name);
			PyTuple_SetItem(data_pair, 1, mat_name);
			
			//now add the tuple to our main list
			PyList_SetItem(data_list, i, data_pair);
		}
		return data_list;
	}

	//------------------------------------------------------------------------
	//Reload the material textures for the given object and material
	//------------------------------------------------------------------------
	PyObject* reload_material_texture(PyObject *self, PyObject *args)
	{
		//allow using the selected material or allow using specific object, material names
		const char *object_name, *material_name;
		int num_args = (int)PyTuple_Size( args );
		if (num_args == 2)
		{
			if (!PyArg_ParseTuple(args, "ss", &object_name, &material_name))
				return NULL;
			//return the mtrlScriptObject based on the name given
			mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
			if( !script_obj )
				return NULL;
			
			mtrlOperations::ReloadTextures(script_obj, nameString(std::string(material_name)));
		}
		else if (num_args == 0)
		{
			//use selected index
			mtrlOperations::ReloadTextures();
		}
		return Py_None;
	}

	//.......................................................
	//Return all materials belonging to an object
	//.......................................................
	PyObject* import_material(PyObject *self, PyObject *args)
	{
		const char *object_name, *material_location;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &material_location))
			return NULL;
		
		//return the mtrl script object based on the given name
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//create the fsLocator using the given path
		fsLocator matLocator;
		matLocator = fsLocator(	itString(material_location) );
		try
		{
			int num_imported = 	script_obj->ImportMaterials(matLocator);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem importing material data, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
		}
	
		return Py_None;
	}

	//.......................................................
	//save the materials of an object at the given file location
	//.......................................................
	PyObject* save_material(PyObject *self, PyObject *args)
	{
		const char *object_name, *material_location;
		if (!PyArg_ParseTuple(args, "ss", &object_name, &material_location))
			return NULL;
		
		//return the mtrl script object based on the given name
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//create the fsLocator using the given path
		fsLocator matLocator;
		matLocator = fsLocator(	itString(material_location) );
		try
		{
			script_obj->SaveMaterials(matLocator);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem saving material data, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
		}

		return Py_None;
	}

	//.......................................................
	//import a mtl material from the library
	//.......................................................
	PyObject* import_from_library(PyObject *self, PyObject *args)
	{
		const char *object_name, *material_name, *material_location;
		if (!PyArg_ParseTuple(args, "sss", &object_name, &material_name, &material_location))
			return NULL;
		
		//return the mtrl script object based on the given name
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//create the fsLocator using the given path
		fsLocator matLocator;
		matLocator = fsLocator(	itString(material_location) );

		//set the index of the current material
		mtrlOperations::SetSelectedMaterialIndex(script_obj, script_obj->GetIndexForName(material_name));

		//use the material locator given, but don't pop up a message box if there is an error
		mtrlOperations::ImportFromLibrary(matLocator, false);
	
		return Py_None;
	}

	//.......................................................
	//import a mtl material from the library
	//.......................................................
	PyObject* export_to_library(PyObject *self, PyObject *args)
	{
		const char *object_name, *material_name, *material_location;
		if (!PyArg_ParseTuple(args, "sss", &object_name, &material_name, &material_location))
			return NULL;
		
		//return the mtrl script object based on the given name
		mtrlScriptObject* script_obj = get_mtrl_script_object(object_name);
		if( !script_obj )
			return NULL;

		//get the material info for the current material object
		const mdlMaterialInfo &mat_info = script_obj->GetMaterialData(script_obj->GetIndexForName(material_name));

		//create the fsLocator using the given path
		fsLocator matLocator;
		matLocator = fsLocator(	itString(material_location) );

		//use the material locator given, but don't pop up a message box if there is an error
		try
		{
			mtrlOperations::ExportToLibrary(matLocator, mat_info);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem writing material data, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
		}
	
		return Py_None;
	}

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
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
		pythModules::AddCommand(i_ModuleName, 
			"getSelectedMaterialIndex", "Returns selected material index", get_selected_material_index);
		pythModules::AddCommand(i_ModuleName,
			"getSelectedMaterialList", "Get a list of selected materials with their Parent", get_selected_material_list);
		pythModules::AddCommand(i_ModuleName, 
			"reloadMaterialTexture", "Reloads a material's textures", reload_material_texture);
		pythModules::AddCommand(i_ModuleName, 
			"importMaterial", "Imports a material file into an object", import_material);
		pythModules::AddCommand(i_ModuleName, 
			"saveMaterial", "Save the materials of an object", save_material);
		pythModules::AddCommand(i_ModuleName, 
			"importFromLibrary", "Import a material from the material library", import_from_library);
		pythModules::AddCommand(i_ModuleName, 
			"exportToLibrary", "Export a material to the material library", export_to_library);
	}

}  //end pythMtrlProperty namespace

#endif
