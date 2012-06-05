/****************************************************************************\
**	pythUserScriptUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythUserScriptUtil.hpp"

#include "Support/pyth/pythPython.hpp"
#include "Support/pyth/pythLog.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
namespace pythUserScriptUtil
{ 
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	namespace
	{
		std::string l_ErrorMessage;

		//----------------------------------------------------------------
		//----------------------------------------------------------------
		void UserScriptLog(const char* i_pMessage)
		{
			l_ErrorMessage += i_pMessage;
		}

#if defined(PYTHON_ENABLED)
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		void prepare_user_log()
		{
			l_ErrorMessage.clear();
			pythLog::SetTemporaryLogFunctions(UserScriptLog, UserScriptLog);
		}
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		void restore_log()
		{
			pythLog::RestoreLogFunctions();
		}

		//----------------------------------------------------------------
		//----------------------------------------------------------------
		PyObject* run_user_script(const std::string& i_Command)
		{
			//PyRun_String( const char *str, int start, PyObject *globals, PyObject *locals) 

			// We want these scripts to run in the main module so that they
			// have access to the mach API and any global the user has made
			//PyObject* vars = Py_BuildValue("{}"); // empty dictionary
			
			PyObject *main_module = PyImport_AddModule("__main__");
			if (main_module)
			{
				PyObject *main_dict = PyModule_GetDict(main_module);

				// The following line using "Py_eval_input" will evaluate a single line.
				// This was not useful because you couldn't construct an expression over
				// a couple of lines
				//PyObject* ret_val = PyRun_String( i_Command.c_str(), Py_eval_input, main_dict, main_dict );

				// So, the next attempt is to let the user define a variable "value"
				// in the script and have that value be interpreted as the result
				// of the expression.
				PyObject* local_vars = Py_BuildValue("{}"); // empty dictionary
				PyObject* value_obj = NULL;
				PyObject* ret_val = PyRun_String( i_Command.c_str(), Py_file_input, main_dict, local_vars );
				if (ret_val)
				{
					// If the script was correct, the user should have defined
					// a local variable "value". Here we try to get the value from 
					// the local variables dictionary.
					//int num_vals = PyDict_Size(local_vars);
					value_obj = PyDict_GetItemString(local_vars, "value");
					if (value_obj)
					{
						// PyDict_GetItemString returns a borrowed reference,
						// so we increment it so that it stay around
						// when we destroy the local dictionary.
						Py_INCREF( value_obj );
					}
					else
					{
						PyErr_SetString(PyExc_SyntaxError, "User script should set a \"value\" variable.");
					}
				}
				Py_XDECREF( local_vars );
				Py_XDECREF( ret_val );
				return value_obj;
			}
			return NULL;
		}
#endif

	} // end of namespace

//========================================================================
// Execute the following code in the python interpretor.
// If successful, returns true and sets the return value of
// the expression in the second argument.
//========================================================================

	//--------------------------------------------------------------------
	// Boolean
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   bool &o_Value,
						   std::string& o_ErrorMessage)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		prepare_user_log();
		PyObject* pReturnObj = run_user_script(i_Command);
		if (pReturnObj != NULL)
		{
			if (pReturnObj == Py_True)
			{
				o_Value = true;
				bReturnValue = true;
			}
			else if (pReturnObj == Py_False)
			{
				o_Value = false;
				bReturnValue = true;
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "Expected True or False");
				PyErr_Print();
			}

			Py_DECREF( pReturnObj );
			PyErr_Clear();
		}
		else
		{
			PyErr_Print();
		}
		o_ErrorMessage = l_ErrorMessage;
		restore_log();
	#endif
		return bReturnValue;
	}

	//--------------------------------------------------------------------
	// Float
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   float &o_Value,
						   std::string& o_ErrorMessage)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		prepare_user_log();
		PyObject* pReturnObj = run_user_script(i_Command);
		if (pReturnObj != NULL)
		{
			float value = (float) PyFloat_AsDouble( pReturnObj );
			if (!PyErr_Occurred())
			{
				o_Value = value;
				bReturnValue = true;
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "Expected floating point number.");
				PyErr_Print();
			}

			Py_DECREF( pReturnObj );
			PyErr_Clear();
		}
		else
		{
			PyErr_Print();
		}
		o_ErrorMessage = l_ErrorMessage;
		restore_log();
	#endif
		return bReturnValue;
	}

	//--------------------------------------------------------------------
	// Color
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   maFloatRGBA &o_Color,
						   std::string& o_ErrorMessage)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		prepare_user_log();
		PyObject* pReturnObj = run_user_script(i_Command);
		if (pReturnObj != NULL)
		{
			float r,g,b,a;
			if (PyArg_ParseTuple(pReturnObj, "ffff", &r, &g, &b, &a))
			{
				o_Color.Set(r,g,b,a);
				bReturnValue = true;
			}
			else
			{
				PyErr_Print();
			}
			Py_DECREF( pReturnObj );
			PyErr_Clear();
		}
		else
		{
			PyErr_Print();
		}
		o_ErrorMessage = l_ErrorMessage;
		restore_log();
	#endif
		return bReturnValue;
	}

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   maPoint3d &o_Position,
						   std::string& o_ErrorMessage)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		prepare_user_log();
		PyObject* pReturnObj = run_user_script(i_Command);
		if (pReturnObj != NULL)
		{
			float x,y,z;
			if (PyArg_ParseTuple(pReturnObj, "fff", &x, &y, &z))
			{
				o_Position.Set(x,y,z);
				bReturnValue = true;
			}
			else
			{
				PyErr_Print();
			}
			Py_DECREF( pReturnObj );
			PyErr_Clear();
		}
		else
		{
			PyErr_Print();
		}
		o_ErrorMessage = l_ErrorMessage;
		restore_log();
	#endif
		return bReturnValue;
	}

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   float &o_EulerX,
						   float &o_EulerY,
						   float &o_EulerZ,
						   std::string& o_ErrorMessage)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		prepare_user_log();
		PyObject* pReturnObj = run_user_script(i_Command);
		if (pReturnObj != NULL)
		{
			float x,y,z;
			if (PyArg_ParseTuple(pReturnObj, "fff", &x, &y, &z))
			{
				o_EulerX = x;
				o_EulerY = y;
				o_EulerZ = z;
				bReturnValue = true;
			}
			else
			{
				PyErr_Print();
			}
			Py_DECREF( pReturnObj );
			PyErr_Clear();
		}
		else
		{
			PyErr_Print();
		}
		o_ErrorMessage = l_ErrorMessage;
		restore_log();
	#endif
		return bReturnValue;
	}

	//--------------------------------------------------------------------
	// Execute a property callback as a python script, passing in
	//	object name and property name.
	//--------------------------------------------------------------------
	bool ExecutePropertyCallback(const std::string& i_Command,
								 const std::string& i_ObjectName,
								 const std::string& i_PropertyName)
	{
		bool bReturnValue = false;
	#if defined(PYTHON_ENABLED)
		// thread lock object ensures the thread state on constructor
		// and releases on destructor
		pythThreadLock thread_lock;
		PyObject *main_module = PyImport_AddModule("__main__");
		if (main_module)
		{
			PyObject *main_dict = PyModule_GetDict(main_module);

			// construct local variables so user can get at object and
			// property name
			PyObject* local_vars = PyDict_New(); 
			PyObject* object_name = Py_BuildValue("s", i_ObjectName.c_str());
			PyObject* property_name = Py_BuildValue("s", i_PropertyName.c_str());
			PyDict_SetItemString(local_vars, "this_object", object_name);
			PyDict_SetItemString(local_vars, "this_property", property_name);
			PyObject* ret_val = PyRun_String( i_Command.c_str(), Py_file_input, main_dict, local_vars );
			Py_XDECREF( ret_val );
			Py_XDECREF( local_vars );
			Py_XDECREF( object_name );
			Py_XDECREF( property_name );
		}
	#endif
		return bReturnValue;

	}

} // end of namespace pythUserScriptUtil
