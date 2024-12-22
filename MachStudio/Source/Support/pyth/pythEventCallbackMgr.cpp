/****************************************************************************\
**	pythEventCallbackMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Support/pyth/pythEventCallbackMgr.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)


//============================================================================
//============================================================================
namespace pythEventCallbackMgr
{
	namespace
	{
		class pythPythonEvent
		{
		public:
			//--------------------------------------------------------------------
			// constructor
			//--------------------------------------------------------------------
			pythPythonEvent( PyObject *i_pFunction)
				: m_pFunction(i_pFunction)
			{
				// Add a reference to callback
				Py_XINCREF(m_pFunction);        
			}

			//--------------------------------------------------------------------
			// destructor
			//--------------------------------------------------------------------
			virtual ~pythPythonEvent()
			{}

			//--------------------------------------------------------------------
			// Remove reference to callback
			//--------------------------------------------------------------------
			void DeRef()
			{
				if (m_pFunction)
					Py_XDECREF(m_pFunction); 
				m_pFunction = NULL;
			}

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			void ExecuteEvent()
			{
				if (!m_pFunction) 
					return;
					
				pythThreadLock thread_lock;
				PyObject *arglist = Py_BuildValue("()"); //? ("");
				PyObject *result = PyEval_CallObject(m_pFunction, arglist);
				Py_DECREF(arglist);

				// If result is NULL, then we may need to do something to 
				// communicate the error back to the user?
				if (result == NULL)
				{
					//	error! - need a NON-DEBUG ERROR message.  throw an error?
					//
					std::string err_msg = "print \"Python Script Error ";
					err_msg += "\"";
					PyRun_SimpleString(err_msg.c_str());
					DBG_ERROR("Python error: Python event callback failed");
				}
				else
				{
					Py_DECREF(result);
				}
			}
		public:
			PyObject *m_pFunction;
		};

		std::vector<pythPythonEvent*> l_Events;

		//--------------------------------------------------------------------
		// add a python command to the manager
		//--------------------------------------------------------------------
		PyObject* add_event(PyObject *self, PyObject *args)
		{
			PyObject *function = NULL;
			if (PyArg_ParseTuple(args, "O", &function)) 
			{
				if (!PyCallable_Check(function)) 
				{
					PyErr_SetString(PyExc_TypeError, "Function parameter must be callable");
					return Py_None;
				}
				pythPythonEvent *pEventObject = new pythPythonEvent(function);
				l_Events.push_back(pEventObject);
			}
			return Py_None;
		}

		//--------------------------------------------------------------------
		// remove a python command
		//--------------------------------------------------------------------
		PyObject* remove_event(PyObject *self, PyObject *args)
		{
			PyObject *function = NULL;
			if (PyArg_ParseTuple(args, "O", &function)) 
			{
				if (!PyCallable_Check(function)) 
				{
					PyErr_SetString(PyExc_TypeError, "Function parameter must be callable");
					return Py_None;
				}

				std::vector<pythPythonEvent*>::iterator it, end = l_Events.end();
				for( it = l_Events.begin(); it != end; ++it )
				{
					if( (*it)->m_pFunction == function )
					{
						(*it)->DeRef();
						//envSTLHelpers::RemoveOneValue(l_Events, (*it));
						break;
					}
				}
			}
			return Py_None;
		}
	} //end anonymous namespace

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
		
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		envSTLHelpers::ForAll(l_Events, std::mem_fn(&pythPythonEvent::DeRef));
		envSTLHelpers::DeleteContainer(l_Events);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void InvokeEvents()
	{		
		std::vector<pythPythonEvent*>::iterator it, end = l_Events.end();
		for( it = l_Events.begin(); it != end; ++it )
		{
			(*it)->ExecuteEvent();
		}
	}

	//------------------------------------------------------------------------
	// Add commands related to the selection list
	//------------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"addEventCallback", 
			"Add a python function to be executed each frame", 
			add_event);
		pythModules::AddCommand(i_ModuleName, 
			"removeEventCallback", 
			"Remove a python function from the callback list", 
			remove_event);
	}
} //end pythEventCallbackMgr
#endif