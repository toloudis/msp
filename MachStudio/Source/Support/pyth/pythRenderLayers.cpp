/*****************************************************************************
**	pythRenderLayers.hpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/
#include "Support/pyth/pythRenderLayers.hpp"

#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/pyth/pythPropertyUtil.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/fs/fsFileUtil.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace pythRenderLayers
{
	namespace
	{
		//--------------------------------------------------------------------
		// Add a new render layer to the system, returns name of new layer
		//--------------------------------------------------------------------
		PyObject* add_render_layer(PyObject *self, PyObject *args)
		{
			nameString layer_name;
			rlyrRenderLayerMgr::Update();
			layer_name = rlyrRenderLayerMgr::AddRenderLayer();

			return pythFunctionUtil::ConvertString(layer_name.GetString());
		}

		//--------------------------------------------------------------------
		// Delete a render layer from the system by name
		//--------------------------------------------------------------------
		PyObject* delete_render_layer(PyObject *self, PyObject *args)
		{
			const char *rlyr;
			if (!PyArg_ParseTuple(args, "s", &rlyr))
				return NULL;

			rlyrRenderLayerMgr::Update();
			//delete the layer
			if(std::string(rlyr) != "Master")
				rlyrRenderLayerMgr::DeleteRenderLayer(nameString(std::string(rlyr)));

			return Py_None;
		}

		//--------------------------------------------------------------------
		// Duplicate a render layer to the system.  Return the name of the new layer
		//--------------------------------------------------------------------
		PyObject* duplicate_render_layer(PyObject *self, PyObject *args)
		{
			const char *rlyr;
			if (!PyArg_ParseTuple(args, "s", &rlyr))
				return NULL;

			rlyrRenderLayerMgr::Update();
			nameString dup_layer_name;
			dup_layer_name = rlyrRenderLayerMgr::CopyRenderLayer(nameString(std::string(rlyr)));

			return pythFunctionUtil::ConvertString(dup_layer_name.GetString());
		}

		//--------------------------------------------------------------------
		// Rename a render layer
		//--------------------------------------------------------------------
		PyObject* rename_render_layer(PyObject *self, PyObject *args)
		{
			const char *old_name, *new_name;
			if (!PyArg_ParseTuple(args, "ss", &old_name, &new_name))
				return Py_None;

			rlyrRenderLayerMgr::Update();
			if(std::string(old_name) == "Master")
			{
				PyErr_SetString(PyExc_NameError, "Cannot rename the Master layer");
				return Py_None;
			}
			if(!rlyrRenderLayerMgr::ChangeLayerName(nameString(std::string(old_name)),
												nameString(std::string(new_name))))
			{
				PyErr_SetString(PyExc_NameError, "New layer name is not valid");
				return Py_None;
			}

			return pythFunctionUtil::ConvertString(std::string(new_name));
		}

		//--------------------------------------------------------------------
		// return a list of all render layers in the system
		//--------------------------------------------------------------------
		PyObject* get_render_layers(PyObject *self, PyObject *args)
		{
			rlyrRenderLayerMgr::Update();
			std::vector<nameString> name_list;
			rlyrRenderLayerMgr::GetRenderLayerNames(name_list);
			return pythFunctionUtil::ConvertNameList(name_list);
		}

		//--------------------------------------------------------------------
		// Activate or deactivate a render layer in the system
		//--------------------------------------------------------------------
		PyObject* set_render_layer_active(PyObject *self, PyObject *args)
		{
			const char *layer_name;
			bool bActive;
			if (!PyArg_ParseTuple(args, "sb", &layer_name, &bActive))
				return Py_None;

			rlyrRenderLayerMgr::Update();
			rlyrRenderLayerMgr::SetLayerActive(nameString(std::string(layer_name)), bActive);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Activate or deactivate a render layer in the system
		//--------------------------------------------------------------------
		PyObject* set_render_layer_object(PyObject *self, PyObject *args)
		{
			const char *layer_name, *object_name;
			bool bActive;
			if (!PyArg_ParseTuple(args, "ssb", &layer_name, &object_name, &bActive))
				return Py_None;

			rlyrRenderLayerMgr::Update();
			//activate/deactivate the object for the given render layer
			rlyrRenderLayerMgr::SetLayerObjectState(nameString(std::string(layer_name)),
													nameString(std::string(object_name)),
													bActive);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// set the render layer's render pref value
		//--------------------------------------------------------------------
		PyObject* set_render_layer_pref(PyObject *self, PyObject *args)
		{
			int num_args = (int)PyTuple_Size( args );
			if (num_args != 3)
			{
				PyErr_SetString(PyExc_TypeError, "setRenderLayerPref needs 3 arguments: render layer name, property name, value (which can be a tuple)");
				return NULL;
			}

			rlyrRenderLayerMgr::Update();
			const char *layer_name,*pref_property;
			layer_name = PyString_AsString( PyTuple_GetItem( args, 0 ) );
			pref_property = PyString_AsString( PyTuple_GetItem( args, 1 ) );

			rprfPrefsObject* layer_prefs = rlyrRenderLayerMgr::GetLayerRenderPrefs(nameString(std::string(layer_name)));
			if(!layer_prefs)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(pref_property);
			const prtyProperty* pProperty = layer_prefs->GetProperty(property_name);

			pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), layer_prefs, PyTuple_GetItem( args, 2 ) );
			return Py_None;
		}

		//--------------------------------------------------------------------
		// get the render layer's render pref value for the given property
		//--------------------------------------------------------------------
		PyObject* get_render_layer_pref(PyObject *self, PyObject *args)
		{
			const char *layer_name, *pref_property;
			if (!PyArg_ParseTuple(args, "ss", &layer_name, &pref_property))
				return Py_None;
			
			rlyrRenderLayerMgr::Update();
			rprfPrefsObject* layer_prefs = rlyrRenderLayerMgr::GetLayerRenderPrefs(nameString(std::string(layer_name)));
			if(!layer_prefs)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(pref_property);
			const prtyProperty* pProperty = layer_prefs->GetProperty(property_name);

			return pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
		}

		//--------------------------------------------------------------------
		// get the list of render pref properties
		//--------------------------------------------------------------------
		PyObject* get_render_pref_properties(PyObject *self, PyObject *args)
		{
			rprfPrefsObject prefs;

			PyObject* PropList;
			std::string prop_name;

			PropertyUIIList::const_iterator it;
			it = prefs.GetList().begin();

			rlyrRenderLayerMgr::Update();
			//iterate through the property list and return the components
			PropList = PyList_New( prefs.GetList().size() );
			for( int i = 0; i < prefs.GetList().size(); ++i )
			{
				prop_name = (*it)->GetProperty(0)->GetPropertyName();
				PyList_SetItem(PropList, i, 
								PyString_FromString(prop_name.c_str()));
				++it;
			}

			return PropList;
		}

		//--------------------------------------------------------------------
		// set the render layer's render pref value
		//--------------------------------------------------------------------
		PyObject* set_render_layer_pass(PyObject *self, PyObject *args)
		{
			int num_args = (int)PyTuple_Size( args );
			if (num_args != 3)
			{
				PyErr_SetString(PyExc_TypeError, "setRenderLayerPref needs 3 arguments: render layer name, pass name, boolean value");
				return NULL;
			}

			rlyrRenderLayerMgr::Update();
			const char *layer_name,*pass_property;
			layer_name = PyString_AsString( PyTuple_GetItem( args, 0 ) );
			pass_property = PyString_AsString( PyTuple_GetItem( args, 1 ) );

			rlyrPassesObject* layer_passes = rlyrRenderLayerMgr::GetLayerRenderPasses(nameString(std::string(layer_name)));
			if(!layer_passes)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(pass_property);
			const prtyProperty* pProperty = layer_passes->GetProperty(property_name);
			if (!pProperty)
			{
				PyErr_SetString(PyExc_NameError, "setRenderLayerPass, pass name not recognized");
				return NULL;
			}
			
			pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), layer_passes, PyTuple_GetItem( args, 2 ) );
			return Py_None;
		}

		//--------------------------------------------------------------------
		// get the render layer's render pref value for the given property
		//--------------------------------------------------------------------
		PyObject* get_render_layer_pass(PyObject *self, PyObject *args)
		{
			const char *layer_name, *pass_property;
			if (!PyArg_ParseTuple(args, "ss", &layer_name, &pass_property))
				return Py_None;
			
			rlyrRenderLayerMgr::Update();
			rlyrPassesObject* layer_passes = rlyrRenderLayerMgr::GetLayerRenderPasses(nameString(std::string(layer_name)));
			if(!layer_passes)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(pass_property);
			const prtyProperty* pProperty = layer_passes->GetProperty(property_name);
			if (!pProperty)
			{
				PyErr_SetString(PyExc_NameError, "getRenderLayerPass, pass name not recognized");
				return NULL;
			}
			return pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
		}

		//--------------------------------------------------------------------
		// get the list of render layer passes
		//--------------------------------------------------------------------
		PyObject* get_render_layer_passes(PyObject *self, PyObject *args)
		{
			rlyrPassesObject passes;

			PyObject* PropList;
			std::string pass_name;

			PropertyUIIList::const_iterator it;
			it = passes.GetList().begin();

			rlyrRenderLayerMgr::Update();

			//iterate through the property list and return the names
			int np = passes.GetList().size();
			PropList = PyList_New( np );
			for( int i = 0; i < np; ++i )
			{
				pass_name = (*it)->GetProperty(0)->GetPropertyName();
				PyList_SetItem(PropList, i, 
								PyString_FromString(pass_name.c_str()));
				++it;
			}

			return PropList;
		}

		//--------------------------------------------------------------------
		// set the render layer's output option value
		//--------------------------------------------------------------------
		PyObject* set_render_layer_output(PyObject *self, PyObject *args)
		{
			int num_args = (int)PyTuple_Size( args );
			if (num_args != 3)
			{
				PyErr_SetString(PyExc_TypeError, "setRenderLayerOutput needs 3 arguments: render layer name, property name, value (which can be a tuple)");
				return Py_None;
			}
			
			rlyrRenderLayerMgr::Update();
			const char *layer_name,*output_property;
			layer_name = PyString_AsString( PyTuple_GetItem( args, 0 ) );
			output_property = PyString_AsString( PyTuple_GetItem( args, 1 ) );

			if(std::string(layer_name) != "Master" && std::string(output_property) == "Sound File")
			{
				PyErr_SetString(PyExc_ValueError, "Only the Master layer has the Sound File parameter");
				return Py_None;
			}

			captRenderOutputObject* layer_output = rlyrRenderLayerMgr::GetLayerCaptureOptions(nameString(std::string(layer_name)));
			if(!layer_output)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(output_property);
			const prtyProperty* pProperty = layer_output->GetProperty(property_name);

			pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), layer_output, PyTuple_GetItem( args, 2 ) );
			return Py_None;
		}

		//--------------------------------------------------------------------
		// get the render layer's output option value for the given property
		//--------------------------------------------------------------------
		PyObject* get_render_layer_output(PyObject *self, PyObject *args)
		{
			const char *layer_name, *output_property;
			if (!PyArg_ParseTuple(args, "ss", &layer_name, &output_property))
				return Py_None;

			rlyrRenderLayerMgr::Update();
			captRenderOutputObject* layer_output = rlyrRenderLayerMgr::GetLayerCaptureOptions(nameString(std::string(layer_name)));
			if(!layer_output)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(output_property);
			const prtyProperty* pProperty = layer_output->GetProperty(property_name);

			return pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
		}

		//--------------------------------------------------------------------
		// get the list of render pref properties
		//--------------------------------------------------------------------
		PyObject* get_render_output_options(PyObject *self, PyObject *args)
		{
			//show all options even sound file, which is only available to the Master layer
			//but should still be listed.
			captRenderOutputObject output_options(true, true);

			PyObject* PropList;
			std::string prop_name;

			PropertyUIIList::const_iterator it;
			it = output_options.GetList().begin();

			rlyrRenderLayerMgr::Update();
			//iterate through the property list and return the components
			PropList = PyList_New( output_options.GetList().size() );
			for( int i = 0; i < output_options.GetList().size(); ++i )
			{
				prop_name = (*it)->GetProperty(0)->GetPropertyName();
				PyList_SetItem(PropList, i, 
								PyString_FromString(prop_name.c_str()));
				++it;
			}

			return PropList;
		}

		//--------------------------------------------------------------------
		// get a list of tuples containing render path and render times
		//--------------------------------------------------------------------
		PyObject* get_render_output_data(PyObject *self, PyObject *args)
		{
			const char *layer_name;
			if (!PyArg_ParseTuple(args, "s", &layer_name))
				return Py_None;

			rlyrRenderLayerMgr::Update();
			std::vector<RenderData> data_list;
			rlyrRenderLayerMgr::GetLayerRenderDataList(nameString(std::string(layer_name)), data_list);
			
			//define python objects, our main list, the tuple to hold each pair, and the individual data objects
			PyObject* render_list = PyList_New( data_list.size() );
			PyObject* data_pair; 
			PyObject* file_path;
			PyObject* frame_count;
			std::string path;
			
			int i = 0;
			std::vector<RenderData>::iterator it, end = data_list.end();
			for( it = data_list.begin(); it != end; ++it )
			{
				//create a new tuple for the current pair
				data_pair = PyTuple_New( 2 );

				//get the values for each pair from the vector
				fsFileUtil::LocatorToANSIFilename((*it).m_RenderLocation, path);
				file_path = pythFunctionUtil::ConvertString(path);
				frame_count = pythFunctionUtil::ConvertInt((*it).m_RenderTime);

				//set the items into the tuple
				PyTuple_SetItem(data_pair, 0, file_path);
				PyTuple_SetItem(data_pair, 1, frame_count);
				
				//now add the tuple to our main list
				PyList_SetItem(render_list, i, data_pair);
				++i;
			}

			return render_list;
		}
	} //end  anon namespace
	
	//------------------------------------------------------------------------
	// Add commands related to the render layers
	//------------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
	#if(SGPU_APP != MS_CORE)
		pythModules::AddCommand(i_ModuleName, 
			"addRenderLayer", "Add a new Render Layer to the system", add_render_layer);
		pythModules::AddCommand(i_ModuleName, 
			"deleteRenderLayer", "Delete a Render Layer by name", delete_render_layer);
		pythModules::AddCommand(i_ModuleName, 
			"duplicateRenderLayer", "Duplicate a Render Layer by name", duplicate_render_layer);
		pythModules::AddCommand(i_ModuleName, 
			"renameRenderLayer", "Rename a Render Layer", rename_render_layer);
	#endif
		pythModules::AddCommand(i_ModuleName, 
			"getRenderLayers", "Return a list of all render layers", get_render_layers);
		pythModules::AddCommand(i_ModuleName, 
			"setRenderLayerState", "Activate or Deactivate a render layer", set_render_layer_active);
		pythModules::AddCommand(i_ModuleName,
			"setRenderLayerObject", "Acivate or Deactivate an object for this render layer.", set_render_layer_object);
		pythModules::AddCommand(i_ModuleName,
			"setRenderLayerPref", "Set the render pref property of a render layer", set_render_layer_pref);
		pythModules::AddCommand(i_ModuleName,
			"getRenderLayerPref", "Set the render pref property of a render layer", get_render_layer_pref);
		pythModules::AddCommand(i_ModuleName,
			"getRenderPrefProperties", "Get the list of render pref properties", get_render_pref_properties);
		pythModules::AddCommand(i_ModuleName,
			"setRenderLayerOutput", "Set the render output property of a render layer", set_render_layer_output);
		pythModules::AddCommand(i_ModuleName,
			"getRenderLayerOutput", "Set the render output property of a render layer", get_render_layer_output);
		pythModules::AddCommand(i_ModuleName,
			"getRenderOutputOptions", "Get the list of Output Options for the render layers", get_render_output_options);
		pythModules::AddCommand(i_ModuleName,
			"getRenderLayerOutputData", "Get the list of render files and their render times for each render layer", get_render_output_data);
		pythModules::AddCommand(i_ModuleName,
			"setRenderLayerPass", "Enable or Disable the named render pass of a render layer", set_render_layer_pass);
		pythModules::AddCommand(i_ModuleName,
			"getRenderLayerPass", "Get checked state of the named render pass for a render layer", get_render_layer_pass);
		pythModules::AddCommand(i_ModuleName,
			"getRenderLayerPasses", "Return a list of allowed render layer pass names", get_render_layer_passes);
	}

}  //end pythRenderLayers namespace

#endif