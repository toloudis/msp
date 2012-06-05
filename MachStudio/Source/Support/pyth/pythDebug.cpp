/*****************************************************************************
**	pythDebug.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/

#include "Support/pyth/pythDebug.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Core/dbg/dbgMsg.hpp"

#include <sstream>
#include <string>

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace pythDebug
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		//--------------------------------------------------------------------
		// from the given arguments get the entire string
		//--------------------------------------------------------------------
		void get_log_string(PyObject *i_Args, std::string& o_Message)
		{
			std::ostringstream log_ss(std::ostringstream::out);

			int num_args = (int)PyTuple_Size( i_Args );
			PyObject* cur_object;
			
			std::string type;
			char* msg_string;
			float msg_float;
			int msg_int;

			//we need to check each entry's type and write it to the stringstream
			for( int i = 0; i < num_args; ++i)
			{
				cur_object = PyTuple_GetItem( i_Args, i );
				type = std::string(cur_object->ob_type->tp_name);
				if(type == "str")
				{	
					msg_string = PyString_AsString(cur_object);
					log_ss << msg_string;
				}	
				else if (type == "float")
				{	
					msg_float = (float)PyFloat_AsDouble(cur_object);
					log_ss << msg_float;
				}	
				else if (type == "int")
				{	
					msg_int = (int)PyInt_AsLong(cur_object);
					log_ss << msg_int;
				}	
				else
				{
					PyErr_SetString(PyExc_TypeError, "Unrecognized type passed in debug message");
				}
			}

			//now set the compelete message
			o_Message = log_ss.str();
		}

		//--------------------------------------------------------------------
		// Write a message to the debug log
		//--------------------------------------------------------------------
		PyObject* write_log(PyObject *self, PyObject *args)
		{
			std::string log_message;
			get_log_string(args, log_message);
			DBG_LOG(log_message);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Write a message to the debug warning
		//--------------------------------------------------------------------
		PyObject* write_warning(PyObject *self, PyObject *args)
		{
			std::string warning_message;
			get_log_string(args, warning_message);
			DBG_WARNING(warning_message);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Write a message to the debug error
		//--------------------------------------------------------------------
		PyObject* write_error(PyObject *self, PyObject *args)
		{
			std::string error_message;
			get_log_string(args, error_message);
			DBG_ERROR(error_message);
			return Py_None;
		}
	}

	//------------------------------------------------------------------------
	// Add commands related to the capture options
	//------------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"debugLog", "Write a log message to the debug log", write_log);
		pythModules::AddCommand(i_ModuleName, 
			"debugWarning", "Write a warning message to the debug log", write_warning);
		pythModules::AddCommand(i_ModuleName, 
			"debugError", "Write an error message to the debug log", write_error);
	}

} // end pythDebug namespace

#endif