/****************************************************************************\
**	chnlPython.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlPython.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include <vector>

namespace
{
	struct begin_time_sort
	{
		inline bool operator()(const tmlnDriver* lhs, const tmlnDriver* rhs) 
		{ 
			return (lhs->GetBeginTime() < rhs->GetBeginTime()); 
		}
	};

#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// Get script object by name
	//--------------------------------------------------------------------
	tmlnScriptObject* get_script_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(pNameObj);
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
				script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
			}
		}

		return script_obj;
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return list of channels for a given object
	//--------------------------------------------------------------------
	PyObject *
	get_channel_list(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		tmlnScriptObject* pScriptObj =  get_script_object(objectName);
		if (!pScriptObj)
		{
			PyErr_SetString(PyExc_NameError, "Script object with given name does not exist.");
			return NULL;
		}
		
		// Now gather list of the channels that are in this object
		const tmlnChannelSet &channel_set = pScriptObj->GetChannelSet();

		const int num_names = channel_set.GetNumChannels();
		PyObject* NameList = PyList_New(num_names);
		if (NameList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				const tmlnChannel& channel = channel_set.GetChannel(i);
				std::string tokenName = pythUtil::MakeToken(channel.GetName());
				PyList_SetItem(NameList, i, PyString_FromString(tokenName.c_str()));
			}

		//? Py_DECREF(NameList);
		}
		
		return NameList;
	}

	//--------------------------------------------------------------------
	// Create driver fo given name on object of given name
	//--------------------------------------------------------------------
	PyObject *
	create_driver(PyObject *self, PyObject *args)
	{
		const char *objectName, *driverName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &driverName))
			return NULL;

		tmlnScriptObject* pScriptObj =  get_script_object(objectName);
		if (!pScriptObj)
		{
			PyErr_SetString(PyExc_NameError, "Script object with given name does not exist.");
			return NULL;
		}
	
		// get the drivers and find if the driver exists
		//
		tmlnDriverNameList driver_names;
		tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
		if (driver_names.Empty())
		{
			PyErr_SetString(PyExc_TypeError, "No drivers available for an object of this type.");
			return NULL;
		}

		//	Find the index within this driver list
		for (int i=0; i< driver_names.m_DriverNames.size(); i++)
		{
			const tmlnDriverNameList::DriverName &driver_info = driver_names.m_DriverNames[i];
			if (_stricmp(driver_info.m_Name.c_str(), driverName) == 0)
			{
				// Create Driver
				tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( 
											driverName, pScriptObj );

				// Give driver to script object to own
				if (pDriver)
				{
					pScriptObj->AddDriver( pDriver );
					pScriptObj->NotifyDriverChanged();
		
					// Return id of driver in order to identify it
					tmlnDriverId driver_id = pDriver->GetDriverId();
					return pythFunctionUtil::ConvertInt( driver_id );
				}
				else
				{
					PyErr_SetString(PyExc_TypeError, "Could not create driver.");
					return NULL;
				}
			}
		}

		PyErr_SetString(PyExc_NameError, "Could not find driver of given name for this object.");
		return NULL;
	}

	//--------------------------------------------------------------------
	// Return list of drivers for a given object
	//--------------------------------------------------------------------
	PyObject *
	get_driver_list(PyObject *self, PyObject *args)
	{
		const char *objectName;
		if (!PyArg_ParseTuple(args, "s", &objectName))
			return NULL;

		tmlnScriptObject* pScriptObj =  get_script_object(objectName);
		if (!pScriptObj)
		{
			PyErr_SetString(PyExc_NameError, "Script object with given name does not exist.");
			return NULL;
		}
		
		// Now gather list of the drivers that are in this object
		const int num_names = pScriptObj->GetNumDrivers();
		PyObject* DriverList = PyList_New(num_names);
		if (DriverList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				tmlnDriverId driver_id = pScriptObj->GetDriver(i).GetDriverId();
				PyList_SetItem(DriverList, i, pythFunctionUtil::ConvertInt(driver_id));
			}

		//? Py_DECREF(DriverList);
		}
		
		return DriverList;
	}
	
	//--------------------------------------------------------------------
	// Get list of drivers on the channel with given name
	//--------------------------------------------------------------------
	PyObject *
	get_drivers_for_channel(PyObject *self, PyObject *args)
	{
		const char *objectName, *channelName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &channelName))
			return NULL;

		tmlnScriptObject* pScriptObj =  get_script_object(objectName);
		if (!pScriptObj)
		{
			PyErr_SetString(PyExc_NameError, "Script object with given name does not exist.");
			return NULL;
		}

		// Find channel by name
		std::list<const tmlnDriver*> sorted_drivers;
		bool found = false;
		int num_drivers = 0;
		tmlnChannelSet &channel_set = pScriptObj->ChannelSet();
		std::string desired_channel(channelName);
		for (int i=0; i<channel_set.GetNumChannels(); ++i)
		{
			tmlnChannel& channel = channel_set.Channel(i);
			std::string tokenName = pythUtil::MakeToken(channel.GetName());
			if (tokenName == desired_channel)
			{
				found = true;
				num_drivers = channel.GetNumDrivers();
				for (int i=0; i<num_drivers; ++i)
				{
					const tmlnDriver& driver = channel.GetDriver(i);
					sorted_drivers.push_back(&driver);
				}
				break;
			}
		}

		sorted_drivers.sort(begin_time_sort());

		if (found)
		{
			PyObject* DriverList = PyList_New(num_drivers);
			if (DriverList != NULL)
			{
				int i=0;
				std::list<const tmlnDriver*>::const_iterator it;
				for (it = sorted_drivers.begin(); it != sorted_drivers.end(); ++it, ++i)
				{
					tmlnDriverId driver_id = (*it)->GetDriverId();
					PyList_SetItem(DriverList, i, pythFunctionUtil::ConvertInt(driver_id));
				}

			//? Py_DECREF(DriverList);
			}
			return DriverList;
		}

		PyErr_SetString(PyExc_NameError, "Could not find channel of given name for this object.");
		return NULL;
	}

	//--------------------------------------------------------------------
	// Return list of drivers that are selected in the trax editor
	//--------------------------------------------------------------------
	PyObject *
	get_selected_drivers(bool i_bSkipLocked)
	{
		// Get set of selected drivers
		std::set<tmlnDriver*> drivers;
		chnlDialogUtil::GetSelectedDrivers(drivers, i_bSkipLocked);
		
		// Now gather list of the drivers that are in this object
		const int num_names = drivers.size();
		PyObject* DriverList = PyList_New(num_names);
		if (DriverList != NULL)
		{
			int i = 0;
			std::set<tmlnDriver*>::iterator it;
			for (it = drivers.begin(); it != drivers.end(); ++it)
			{
				tmlnDriverId driver_id = (*it)->GetDriverId();
				PyList_SetItem(DriverList, i++, pythFunctionUtil::ConvertInt(driver_id));
			}

		//? Py_DECREF(DriverList);
		}
		
		return DriverList;
	}
	PyObject *
	get_selected_drivers(PyObject *self, PyObject *args)
	{
		const bool skipLocked = false;
		return get_selected_drivers(skipLocked);
	}
	PyObject *
	get_selected_unlocked_drivers(PyObject *self, PyObject *args)
	{
		const bool skipLocked = true;
		return get_selected_drivers(skipLocked);
	}

	//--------------------------------------------------------------------
	// Clear selected drivers list
	//--------------------------------------------------------------------
	PyObject *
	clear_selected_drivers(PyObject *self, PyObject *args)
	{
		chnlDialogUtil::ClearSelection();

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// Delete selected drivers list
	//--------------------------------------------------------------------
	PyObject *
	delete_selected_drivers(PyObject *self, PyObject *args)
	{
		const bool skipLocked = false;
		std::set<tmlnDriver*> drivers;
		chnlDialogUtil::GetSelectedDrivers(drivers, skipLocked);

		int i = 0;
		std::set<tmlnDriver*>::iterator it;
		for (it = drivers.begin(); it != drivers.end(); ++it)
		{
			tmlnDriverId driver_id = (*it)->GetDriverId();
			tmlnDriver *dDriverObj =  tmlnDriverIdMgr::GetDriverById(driver_id);
			tmlnDriverIdMgr::RemoveDriver(dDriverObj, driver_id);
		}

		return NULL;
	}

	//--------------------------------------------------------------------
	// Append a single driver to the selection list
	//--------------------------------------------------------------------
	PyObject *
	append_select_driver(PyObject *self, PyObject *args)
	{
		tmlnDriverId driver_id;
		if (!PyArg_ParseTuple(args, "K", &driver_id))
			return NULL;

		tmlnDriver *pDriver = tmlnDriverIdMgr::GetDriverById(driver_id);
		if (!pDriver)
		{
			PyErr_SetString(PyExc_NameError, "Could not find driver by id.");
			return NULL;
		}

		chnlDialogUtil::AddToSelection( pDriver );
		
		Py_INCREF(Py_None);
		return Py_None;
	}
	
#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void chnlPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "getChannels", 
		"Get list of names of all channels for this object.", 
		get_channel_list);

	pythModules::AddCommand(i_ModuleName, "createDriver", 
		"Create driver of given type for object of given name.\n"
		"	createDriver(objectName, driverName)", 
		create_driver);

	pythModules::AddCommand(i_ModuleName, "getDrivers", 
		"Get list of drivers for this object.\n"
		"	The drivers will not be ordered by any criteria.", 
		get_driver_list);
	pythModules::AddCommand(i_ModuleName, "getDriversForChannel", 
		"Get list of drivers on the channel with given name for this object.\n"
		"	The drivers will be sorted by their begin times.\n"
		"	getDriversForChannel(objectName, channelName)",
		get_drivers_for_channel);

	pythModules::AddCommand(i_ModuleName, "getSelectedDrivers", 
		"Get list of drivers that are selected in trax editor.", 
		get_selected_drivers);
	pythModules::AddCommand(i_ModuleName, "getSelectedUnlockedDrivers", 
		"Get list of drivers from only unlocked channels that are selected in trax editor."
		"Useful if you want to edit the drivers while respecting the locked channel flag.", 
		get_selected_unlocked_drivers);

	pythModules::AddCommand(i_ModuleName, "clearSelectedDrivers", 
		"Clear list of selected drivers.", 
		clear_selected_drivers);
	pythModules::AddCommand(i_ModuleName, "appendSelectDriver", 
		"Append driver by id to the selected list.", 
		append_select_driver);
	pythModules::AddCommand(i_ModuleName, "deleteSelectedDrivers", 
		"Delete all selected drivers.", 
		delete_selected_drivers);
#endif

}