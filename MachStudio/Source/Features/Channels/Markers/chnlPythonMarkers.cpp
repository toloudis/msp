/****************************************************************************\
**	chnlPythonMarkers.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Markers/chnlPythonMarkers.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"



#include <vector>

namespace
{

#if defined(PYTHON_ENABLED)

	//--------------------------------------------------------------------
	// Convert from type string to integer code
	//--------------------------------------------------------------------
	bool convert_type(const char* i_Str, int &o_Code)
	{
		if (!::strcmp(i_Str, "Normal"))
			o_Code = 0;
		else if (!::strcmp(i_Str, "In"))
			o_Code = 1;
		else if (!::strcmp(i_Str, "Out"))
			o_Code = 2;
		else
			return false;
		return true;
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return number of markers in trax editor
	//--------------------------------------------------------------------
	PyObject *
	get_num_markers(PyObject *self, PyObject *args)
	{
		return pythFunctionUtil::ConvertInt( chnlMarkerMgr::GetNumMarkers() );
	}

	//--------------------------------------------------------------------
	// Return list of strings to use to get and set marker properties
	//--------------------------------------------------------------------
	PyObject *
	get_marker_properties(PyObject *self, PyObject *args)
	{
		// Markers don't really use properties, 
		// we need to fake it with a string list

		PyObject* NameList = PyList_New(3);
		if (NameList != NULL)
		{
			// Type, Time and Note
			PyList_SetItem(NameList, 0, PyString_FromString("type"));
			PyList_SetItem(NameList, 1, PyString_FromString("time"));
			PyList_SetItem(NameList, 2, PyString_FromString("note"));

		//? Py_DECREF(NameList);
		}
		
		return NameList;
	}

	//--------------------------------------------------------------------
	// Get value of marker property
	//--------------------------------------------------------------------
	PyObject *
	get_marker_value(PyObject *self, PyObject *args)
	{
		int index;
		const char *propertyName;
		if (!PyArg_ParseTuple(args, "ls", &index, &propertyName))
			return NULL;

		if (index < 0 || index >= chnlMarkerMgr::GetNumMarkers())
		{
			PyErr_SetString(PyExc_TypeError, "getMarkerValue first argument should be index of marker, starting at 0");
			return NULL;
		}

		const chnlMarkerDataItem& marker_data = chnlMarkerMgr::GetMarkerData(index);
		if (!::strcmp(propertyName, "type"))
		{
			switch (marker_data.m_TimeMarkerType.GetValue())
			{
			default:
				PyErr_SetString(PyExc_TypeError, "Unexpected type value for marker");
				return NULL;
			case 0:
				return pythFunctionUtil::ConvertString("Normal");
			case 1:
				return pythFunctionUtil::ConvertString("In");
			case 2:
				return pythFunctionUtil::ConvertString("Out");
			}
		}
		else if (!::strcmp(propertyName, "time"))
		{
			return pythFunctionUtil::ConvertFloat(marker_data.m_Time.GetValue().AsSeconds());
		}
		else if (!::strcmp(propertyName, "note"))
		{
			return pythFunctionUtil::ConvertString(marker_data.m_Note.GetValue());
		}
			
		PyErr_SetString(PyExc_TypeError, "getMarkerValue second arg property name can be 'type', 'time' or 'note'");
		return NULL;
	}

	//--------------------------------------------------------------------
	// Set value of marker property
	//--------------------------------------------------------------------
	PyObject *
	set_marker_value(PyObject *self, PyObject *args)
	{
		int num_args = (int)PyTuple_Size( args );
		if (num_args != 3)
		{
			PyErr_SetString(PyExc_TypeError, "setMarkerValue needs 3 arguments: marker index, property name, value");
			return NULL;
		}

		int index = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
		if (index < 0 || index >= chnlMarkerMgr::GetNumMarkers())
		{
			PyErr_SetString(PyExc_TypeError, "getMarkerValue first argument should be index of marker, starting at 0");
			return NULL;
		}

		chnlMarkerDataItem data = chnlMarkerMgr::GetMarkerData(index);

		char *propertyName = PyString_AsString( PyTuple_GetItem( args, 1 ) );
		PyObject *pValueArg = PyTuple_GetItem( args, 2 );
		if (!::strcmp(propertyName, "type"))
		{
			char *type = PyString_AsString( pValueArg );
			int conv_type;
			if ((type != NULL) && (convert_type(type, conv_type)))
			{
				data.m_TimeMarkerType.SetValue(conv_type);
				chnlMarkerOperations::SetMarkerData(index, data);
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "setMarkerValue type value should be 'Normal' 'In' or 'Out'");
				return NULL;
			}
		}
		else if (!::strcmp(propertyName, "time"))
		{
			data.m_Time = maTime::FromSeconds((float) PyFloat_AsDouble( pValueArg ));
			chnlMarkerOperations::SetMarkerData(index, data);
		}
		else if (!::strcmp(propertyName, "note"))
		{
			data.m_Note = PyString_AsString( pValueArg );
			chnlMarkerOperations::SetMarkerData(index, data);
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "setMarkerValue second arg property name can be 'type', 'time' or 'note'");
			return NULL;
		}

		return pythFunctionUtil::ReturnNone();
	}
	
	//--------------------------------------------------------------------
	// Return list of strings to use to get and set marker properties
	//--------------------------------------------------------------------
	PyObject *
	add_marker(PyObject *self, PyObject *args)
	{
		float time;
		const char *type, *note;
		if (!PyArg_ParseTuple(args, "fss", &time, &type, &note))
			return NULL;

		int type_code = 0;
		if (!convert_type(type, type_code))
		{
			PyErr_SetString(PyExc_TypeError, "addMarker second argument defines type, should be 'Normal' 'In' or 'Out'");
			return NULL;
		}

		chnlMarkerOperations::AddMarker(maTime::FromSeconds(time), type_code, note);

		//Note: If the marker list is sorted, then it might be useful to return the index
		// that this marker sorted into.
		return pythFunctionUtil::ReturnNone();
	}

	PyObject *
	add_marker_at_frame(PyObject *self, PyObject *args)
	{
		int frame;
		const char *type, *note;
		if (!PyArg_ParseTuple(args, "iss", &frame, &type, &note))
			return NULL;

		int type_code = 0;
		if (!convert_type(type, type_code))
		{
			PyErr_SetString(PyExc_TypeError, "addMarkerAtFrame second argument defines type, should be 'Normal' 'In' or 'Out'");
			return NULL;
		}

		chnlMarkerOperations::AddMarker(tmlnTimeUtil::GetTimeFromFrames(frame), type_code, note);

		//Note: If the marker list is sorted, then it might be useful to return the index
		// that this marker sorted into.
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Return list of strings to use to get and set marker properties
	//--------------------------------------------------------------------
	PyObject *
	delete_marker(PyObject *self, PyObject *args)
	{
		float time;
		if (!PyArg_ParseTuple(args, "f", &time))
			return NULL;

		chnlMarkerOperations::DeleteMarker(maTime::FromSeconds(time));

		return pythFunctionUtil::ReturnNone();
	}

	PyObject *
	delete_marker_at_frame(PyObject *self, PyObject *args)
	{
		int frame;
		if (!PyArg_ParseTuple(args, "i", &frame))
			return NULL;
		
		chnlMarkerOperations::DeleteMarkerAtFrame(frame);
		
		return pythFunctionUtil::ReturnNone();
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void chnlPythonMarkers::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "getNumMarkers", 
		"Get number of markers in trax editor.", 
		get_num_markers);
	pythModules::AddCommand(i_ModuleName, "getMarkerProperties", 
		"Get list of properties for a marker. "
		"Same for all markers, just added for consistency with other property functions.", 
		get_marker_properties);
	pythModules::AddCommand(i_ModuleName, "getMarkerValue", 
		"Get value of a marker property. A Marker is identified by its index starting at 0.\n"
		"	value = getMarkerValue(index, property)", 
		get_marker_value);
	pythModules::AddCommand(i_ModuleName, "setMarkerValue", 
		"Set value of a marker property. A Marker is identified by its index starting at 0.\n"
		"	getMarkerValue(index, property, value)", 
		set_marker_value);

	pythModules::AddCommand(i_ModuleName, "addMarker", 
		"Create marker of given type at given time."
		"Time should be in seconds, type can be Normal, In or Out, note is a string.\n"
		"	addMarker(time, type, note)", 
		add_marker);
	pythModules::AddCommand(i_ModuleName, "addMarkerAtFrame", 
		"Create marker of given type at given frame."
		"Frame should be in integer, type can be Normal, In or Out, note is a string.\n"
		"	addMarkerAtFrame(frame, type, note)", 
		add_marker_at_frame);
	pythModules::AddCommand(i_ModuleName, "deleteMarker", 
		"Delete a marker. A Marker is identified by its index starting at 0.\n"
		"	deleteMarker(index)", 
		delete_marker);
	pythModules::AddCommand(i_ModuleName, "deleteMarkerAtFrame", 
		"Delete a marker by using frame index. A Marker is identified by its index starting at 0.\n"
		"	deleteMarker(index)", 
		delete_marker_at_frame);

#endif

}
