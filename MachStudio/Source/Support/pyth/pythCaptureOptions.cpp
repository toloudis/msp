/*****************************************************************************
**	pythCaptureOptions.hpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/
#include "Support/pyth/pythCaptureOptions.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/pyth/pythPropertyUtil.hpp"

#include "Core/name/nameString.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace pythCaptureOptions
{
	namespace
	{
		//--------------------------------------------------------------------
		// Return a list of all capture option properties
		//--------------------------------------------------------------------
		PyObject* get_capture_properties(PyObject *self, PyObject *args)
		{
			captRenderOutputObject capt_options;

			PyObject* PropList;
			std::string prop_name;

			PropertyUIIList::const_iterator it;
			it = capt_options.GetList().begin();

			//iterate through the property list and return the components
			PropList = PyList_New( capt_options.GetList().size() );
			for( int i = 0; i < capt_options.GetList().size(); ++i )
			{
				prop_name = (*it)->GetProperty(0)->GetPropertyName();
				PyList_SetItem(PropList, i, 
								PyString_FromString(prop_name.c_str()));
				++it;
			}

			return PropList;
		}

		//--------------------------------------------------------------------
		// Get the value of a specified capture property
		//--------------------------------------------------------------------
		PyObject* get_capture_value(PyObject *self, PyObject *args)
		{
			const char *capture_property;
			if (!PyArg_ParseTuple(args, "s", &capture_property))
				return Py_None;

			prtyObject* capt_prop = captRenderOutputDataUtil::GetDataObject();
			if(!capt_prop)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(capture_property);
			const prtyProperty* pProperty = capt_prop->GetProperty(property_name);

			return pythPropertyUtil::get_value( const_cast<prtyProperty*>(pProperty) );
			
		}

		//--------------------------------------------------------------------
		// Get the value of a specified capture property
		//--------------------------------------------------------------------
		PyObject* set_capture_value(PyObject *self, PyObject *args)
		{
			int num_args = PyTuple_Size( args );
			if (num_args != 2)
			{
				PyErr_SetString(PyExc_TypeError, "setCaptureValue needs 2 arguments: property name, value (which can be a tuple)");
				return NULL;
			}

			const char *capture_property;
			capture_property = PyString_AsString( PyTuple_GetItem( args, 0 ) );

			prtyObject* capt_prop = captRenderOutputDataUtil::GetDataObject();
			if(!capt_prop)
				return Py_None;

			//get the specified property of the property object
			const std::string property_name(capture_property);
			const prtyProperty* pProperty = capt_prop->GetProperty(property_name);

			pythPropertyUtil::set_value( const_cast<prtyProperty*>(pProperty), capt_prop, PyTuple_GetItem( args, 1 ) );
			return Py_None;
		}

	} //end anon namespace

	//------------------------------------------------------------------------
	// Add commands related to the capture options
	//------------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"getCaptureProperties", "Get a list of all capture properties", get_capture_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getCaptureValue", "Get the value of a specified capture property", get_capture_value);
		pythModules::AddCommand(i_ModuleName, 
			"setCaptureValue", "Set the value of a specified capture property", set_capture_value);
	}

}  //end pythCaptureOptions namespace

#endif