/****************************************************************************\
**	chnlPythonNotes.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Notes/chnlPythonNotes.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include <vector>


namespace
{
#if defined(PYTHON_ENABLED)

	//--------------------------------------------------------------------
	// Convert from status string to integer code
	//--------------------------------------------------------------------
	bool convert_status(const char* i_Str, int &o_Code)
	{
		if (!::strcmp(i_Str, "Open"))
			o_Code = 0;
		else if (!::strcmp(i_Str, "Pending"))
			o_Code = 1;
		else if (!::strcmp(i_Str, "Closed"))
			o_Code = 2;
		else
			return false;
		return true;
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return number of notes in trax editor
	//--------------------------------------------------------------------
	PyObject *
	get_num_notes(PyObject *self, PyObject *args)
	{
		return pythFunctionUtil::ConvertInt( chnlNotesMgr::GetNumNotes() );
	}

	//--------------------------------------------------------------------
	// Return list of strings to use to get and set note properties
	//--------------------------------------------------------------------
	PyObject *
	get_note_properties(PyObject *self, PyObject *args)
	{
		// Notes don't really use properties, 
		// we need to fake it with a string list

		PyObject* NameList = PyList_New(3);
		if (NameList != NULL)
		{
			// Type, Time and Note
			PyList_SetItem(NameList, 0, PyString_FromString("status"));
			PyList_SetItem(NameList, 1, PyString_FromString("time"));
			PyList_SetItem(NameList, 2, PyString_FromString("note"));

		//? Py_DECREF(NameList);
		}
		
		return NameList;
	}

	//--------------------------------------------------------------------
	// Get value of note property
	//--------------------------------------------------------------------
	PyObject *
	get_note_value(PyObject *self, PyObject *args)
	{
		int index;
		const char *propertyName;
		if (!PyArg_ParseTuple(args, "ls", &index, &propertyName))
			return NULL;

		if (index < 0 || index >= chnlNotesMgr::GetNumNotes())
		{
			PyErr_SetString(PyExc_TypeError, "getNoteValue first argument should be index of note, starting at 0");
			return NULL;
		}

		const chnlNoteDataItem& note_data = chnlNotesMgr::GetNoteData(index);
		if (!::strcmp(propertyName, "status"))
		{
			switch (note_data.m_Status.GetValue())
			{
			default:
				PyErr_SetString(PyExc_TypeError, "Unexpected status value for note");
				return NULL;
			case 0:
				return pythFunctionUtil::ConvertString("Open");
			case 1:
				return pythFunctionUtil::ConvertString("Pending");
			case 2:
				return pythFunctionUtil::ConvertString("Closed");
			}
		}
		else if (!::strcmp(propertyName, "time"))
		{
			return pythFunctionUtil::ConvertFloat(note_data.m_Time.GetValue());
		}
		else if (!::strcmp(propertyName, "note"))
		{
			return pythFunctionUtil::ConvertString(note_data.m_Note.GetValue());
		}
			
		PyErr_SetString(PyExc_TypeError, "getNoteValue second arg property name can be 'status', 'time' or 'note'");
		return NULL;
	}

	//--------------------------------------------------------------------
	// Set value of note property
	//--------------------------------------------------------------------
	PyObject *
	set_note_value(PyObject *self, PyObject *args)
	{
		int num_args = PyTuple_Size( args );
		if (num_args != 3)
		{
			PyErr_SetString(PyExc_TypeError, "setNoteValue needs 3 arguments: note index, property name, value");
			return NULL;
		}

		int index = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
		if (index < 0 || index >= chnlNotesMgr::GetNumNotes())
		{
			PyErr_SetString(PyExc_TypeError, "getNoteValue first argument should be index of note, starting at 0");
			return NULL;
		}
		chnlNoteDataItem data = chnlNotesMgr::GetNoteData(index);

		char *propertyName = PyString_AsString( PyTuple_GetItem( args, 1 ) );
		PyObject *pValueArg = PyTuple_GetItem( args, 2 );
		if (!::strcmp(propertyName, "status"))
		{
			char *status = PyString_AsString( pValueArg );
			int conv_status;
			if ((status != NULL) && (convert_status(status, conv_status)))
			{
				data.m_Status.SetValue(conv_status);
				chnlNotesOperations::SetNoteData(index, data);
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "setNoteValue status value should be 'Open' 'Pending' or 'Closed'");
				return NULL;
			}
		}
		else if (!::strcmp(propertyName, "time"))
		{
			data.m_Time = (float) PyFloat_AsDouble( pValueArg );
			chnlNotesOperations::SetNoteData(index, data);
		}
		else if (!::strcmp(propertyName, "note"))
		{
			data.m_Note = PyString_AsString( pValueArg );
			chnlNotesOperations::SetNoteData(index, data);
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "setNoteValue second arg property name can be 'status', 'time' or 'note'");
			return NULL;
		}

		return pythFunctionUtil::ReturnNone();
	}
	
	//--------------------------------------------------------------------
	// Return list of strings to use to get and set note properties
	//--------------------------------------------------------------------
	PyObject *
	add_note(PyObject *self, PyObject *args)
	{
		float time;
		const char *status, *note;
		if (!PyArg_ParseTuple(args, "fss", &time, &status, &note))
			return NULL;

		int status_code = 0;
		if (!convert_status(status, status_code))
		{
			PyErr_SetString(PyExc_TypeError, "addNote second argument defines status, should be 'Open' 'Pending' or 'Closed'");
			return NULL;
		}

		chnlNotesOperations::AddNote(time, status_code, note);

		//Note: If the note list is sorted, then it might be useful to return the index
		// that this note sorted into.
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Return list of strings to use to get and set note properties
	//--------------------------------------------------------------------
	PyObject *
	delete_note(PyObject *self, PyObject *args)
	{
		float time;
		if (!PyArg_ParseTuple(args, "f", &time))
			return NULL;

		chnlNotesOperations::DeleteNote(time);

		return pythFunctionUtil::ReturnNone();
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void chnlPythonNotes::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "getNumNotes", 
		"Get number of notes in trax editor.", 
		get_num_notes);
	pythModules::AddCommand(i_ModuleName, "getNoteProperties", 
		"Get list of properties for a note. "
		"Same for all notes, just added for consistency with other property functions.", 
		get_note_properties);
	pythModules::AddCommand(i_ModuleName, "getNoteValue", 
		"Get value of a note property. A Note is identified by its index starting at 0.\n"
		"	value = getNoteValue(index, property)", 
		get_note_value);
	pythModules::AddCommand(i_ModuleName, "setNoteValue", 
		"Set value of a note property. A Note is identified by its index starting at 0.\n"
		"	getNoteValue(index, property, value)", 
		set_note_value);

	pythModules::AddCommand(i_ModuleName, "addNote", 
		"Create note of given type at given time."
		"Time should be in seconds, type can be Normal, In or Out, note is a string.\n"
		"	addNote(time, type, note)", 
		add_note);
	pythModules::AddCommand(i_ModuleName, "deleteNote", 
		"Delete a note. A Note is identified by its index starting at 0.\n"
		"	deleteNote(index)", 
		delete_note);

#endif

}
