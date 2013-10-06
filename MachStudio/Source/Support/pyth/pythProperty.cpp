/****************************************************************************\
**	pythProperty.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythProperty.hpp"

#include "Support/pyth/pythPropertyUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/name/nameMgr.hpp"

#include "Core/prty/prtyBoolean.hpp"
#include "Core/prty/prtyColor.hpp"
#include "Core/prty/prtyDirectory.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyFileName.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyInt32.hpp"
#include "Core/prty/prtyListChecked.hpp"
#include "Core/prty/prtyName.hpp"
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRotation.hpp"
#include "Core/prty/prtyText.hpp"
#include "Core/prty/prtyVector3d.hpp"

#include "Support/tmln/tmlnDriver.hpp"

#include <vector>


namespace pythProperty
{

	namespace
	{
#if defined(PYTHON_ENABLED)
		// Application provided ways of mapping from a string to a
		// property object. Like for "RenderPrefs" or "Fog"
		std::vector< shared_ptr<NameResolver> > l_Resolvers;

		//--------------------------------------------------------------------
		// Get property object by name
		//--------------------------------------------------------------------
		prtyObject* get_prty_object(const char *i_ObjectName)
		{
			// First try to use the nameMGr to resolve the name
			nameString name_str(i_ObjectName);
			nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
			if (pNameObj)
				return dynamic_cast<prtyObject*>(pNameObj);

			// Otherwise, use the NameResolvers
			std::string object_name(i_ObjectName);
			std::vector< shared_ptr<NameResolver> >::iterator it;
			for (it = l_Resolvers.begin(); it != l_Resolvers.end(); ++it)
			{
				if (prtyObject *pPrtyObject = (*it)->ResolveName(object_name))
					return pPrtyObject;
			}

			return NULL;
		}

		//--------------------------------------------------------------------
		// Get property object for driver with given id
		//--------------------------------------------------------------------
		prtyObject* get_driver_object(tmlnDriverId i_Id)
		{
			tmlnDriver *pDriver = tmlnDriverIdMgr::GetDriverById(i_Id);
			return pDriver;
		}

		//--------------------------------------------------------------------
		// template variation of setting a value of a property after
		// parsing it from the set_value Python tuple.
		//--------------------------------------------------------------------
		//template <class P, class V>
		//bool set_single_value(PyObject *args, P *i_pProperty, const std::string& i_Format)
		//{
		//	const char *objectName, *propertyName;
		//	V value;
		//	std::string format = "ss" + i_Format;
		//	if (!PyArg_ParseTuple(args, format.c_str(), &objectName, &propertyName, &value))
		//		return false;
		//	i_pProperty->SetValue(value, bSetDirty);
		//	return true;
		//}

		
		//============================================================================
		// Commands made available to python
		//============================================================================

		//--------------------------------------------------------------------
		// get list of all properties for given object
		//--------------------------------------------------------------------
		PyObject* get_properties(PyObject *self, PyObject *args)
		{
			const char *objectName;

			if (!PyArg_ParseTuple(args, "s", &objectName))
				return NULL;

			prtyObject *pPrtyObj = get_prty_object(objectName);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find object by name.");
				return NULL;
			}

			return pythPropertyUtil::get_properties(pPrtyObj);
		}

		//--------------------------------------------------------------------
		// get value of given property
		//--------------------------------------------------------------------
		PyObject* get_value(PyObject *self, PyObject *args)
		{
			const char *objectName, *propertyName;
			if (!PyArg_ParseTuple(args, "ss", &objectName, &propertyName))
				return NULL;

			prtyObject *pPrtyObj = get_prty_object(objectName);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find object by name.");
				return NULL;
			}
			return pythPropertyUtil::get_value(pPrtyObj, propertyName);
		}

		//--------------------------------------------------------------------
		// set value of given property on an object
		//--------------------------------------------------------------------
		PyObject* set_value(PyObject *self, PyObject *args)
		{
			int num_args = (int)PyTuple_Size( args );
			if (num_args != 3)
			{
				PyErr_SetString(PyExc_TypeError, "setValue needs 3 arguments: object name, property name, value (which can be a tuple)");
				return NULL;
			}

			char *objectName = PyString_AsString( PyTuple_GetItem( args, 0 ) );

			prtyObject *pPrtyObj = get_prty_object(objectName);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find object by name.");
				return NULL;
			}

			bool bPreserveNameUID = true;
			return pythPropertyUtil::set_property(pPrtyObj, args, bPreserveNameUID);
		}

		//--------------------------------------------------------------------
		// get list of all properties for given driver
		//--------------------------------------------------------------------
		PyObject* get_driver_properties(PyObject *self, PyObject *args)
		{
			tmlnDriverId driver_id;
			if (!PyArg_ParseTuple(args, "K", &driver_id))
				return NULL;

			prtyObject *pPrtyObj = get_driver_object(driver_id);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find driver from id.");
				return NULL;
			}

			return pythPropertyUtil::get_properties(pPrtyObj);
		}
		//--------------------------------------------------------------------
		// Get driver name by ID
		//--------------------------------------------------------------------
		PyObject* get_driver_name(PyObject *self, PyObject *args)
		{
			tmlnDriverId driver_id;
			if (!PyArg_ParseTuple(args, "K", &driver_id))
				return NULL;

			tmlnDriver *dDriverObj = tmlnDriverIdMgr::GetDriverById(driver_id);
			if (!dDriverObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find driver from id.");
				return NULL;
			}

			const std::string driverName = dDriverObj->GetName();
			return PyString_FromString( driverName.c_str() );

		}
		//--------------------------------------------------------------------
		// delete a driver
		//--------------------------------------------------------------------
		PyObject* delete_driver(PyObject *self, PyObject *args)
		{
			tmlnDriverId driver_id;
			if (!PyArg_ParseTuple(args, "K", &driver_id))
				return NULL;

			tmlnDriver *dDriverObj = tmlnDriverIdMgr::GetDriverById(driver_id);
			if (!dDriverObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find driver from id.");
				return NULL;
			}

			tmlnDriverIdMgr::RemoveDriver(dDriverObj, driver_id);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// get value of given property from a driver
		//--------------------------------------------------------------------
		PyObject* get_driver_value(PyObject *self, PyObject *args)
		{
			tmlnDriverId driver_id;
			const char *propertyName;
			if (!PyArg_ParseTuple(args, "Ks", &driver_id, &propertyName))
				return NULL;

			prtyObject *pPrtyObj = get_driver_object(driver_id);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find driver from id.");
				return NULL;
			}
			return pythPropertyUtil::get_value(pPrtyObj, propertyName);
		}

		//--------------------------------------------------------------------
		// set value of given property on a driver
		//--------------------------------------------------------------------
		PyObject* set_driver_value(PyObject *self, PyObject *args)
		{
			int num_args = (int)PyTuple_Size( args );
			if (num_args != 3)
			{
				PyErr_SetString(PyExc_TypeError, "setDriverValue needs 3 arguments: driver id, property name, value (which can be a tuple)");
				return NULL;
			}

			tmlnDriverId driver_id = PyInt_AsUnsignedLongLongMask( PyTuple_GetItem( args, 0 ) );
			if (PyErr_Occurred())
			{
				PyErr_SetString(PyExc_TypeError, "Error parsing driver id.");
				return NULL;
			}

			prtyObject *pPrtyObj = get_driver_object(driver_id);
			if (!pPrtyObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find driver from id.");
				return NULL;
			}

			// drivers use name properties to refer to other objects,
			// so when the name property changes through python, it means
			// go find another object by the string, not by the name id.
			bool bPreserveNameUID = false; 
			return pythPropertyUtil::set_property(pPrtyObj, args, bPreserveNameUID);
		}

#endif
	}	// end of namespace


	//--------------------------------------------------------------------
	// Add/Remove a name resolver.
	//--------------------------------------------------------------------
	void AddNameResolver(const shared_ptr<NameResolver> &i_Resolver)
	{
#if defined(PYTHON_ENABLED)
		l_Resolvers.push_back(i_Resolver);
#endif
	}
	void RemoveNameResolver(const shared_ptr<NameResolver> &i_Resolver)
	{
#if defined(PYTHON_ENABLED)
		envSTLHelpers::RemoveOneValue( l_Resolvers, i_Resolver );
#endif
	}

#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"getProperties", "Get list of properties for object.", get_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getValue", "Get value of named property for object.", get_value);
		pythModules::AddCommand(i_ModuleName, 
			"setValue", "Set value of named property for object.", set_value);

		pythModules::AddCommand(i_ModuleName, 
			"getDriverProperties", "Get list of properties for driver.", get_driver_properties);
		pythModules::AddCommand(i_ModuleName, 
			"getDriverValue", "Get value of named property for driver.", get_driver_value);
		pythModules::AddCommand(i_ModuleName, 
			"setDriverValue", "Set value of named property for driver.", set_driver_value);
		pythModules::AddCommand(i_ModuleName, 
			"deleteDriver", "Delete a driver.", delete_driver);
		pythModules::AddCommand(i_ModuleName, 
			"getDriverName", "Get the name of the driver.", get_driver_name);
	}
#endif

}	// end of namespace

