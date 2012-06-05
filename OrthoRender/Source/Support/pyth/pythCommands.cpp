/****************************************************************************\
**	pythCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythCommands.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <vector>

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//============================================================================
	// Commands made available to python
	//============================================================================
	PyObject *
	list_commands(PyObject *self, PyObject *args)
	{
		const std::vector< cmaCommand* >& cmd_list = cmaCommandMgr::GetCommandList();

		const int num_cmds = cmd_list.size();
		PyObject* CmdList = PyList_New(num_cmds);
		if (CmdList != NULL)
		{
			for (int i=0; i<num_cmds; ++i)
			{
				std::string cmd_name = pythUtil::MakeToken(cmd_list[i]->GetTag());
				PyList_SetItem(CmdList, i, 
					PyString_FromString(cmd_name.c_str()));
			}

		//? Py_DECREF(CmdList);
		}
		
		return CmdList;
	}

	PyObject *
	set_verbose(PyObject *self, PyObject *args)
	{
		const std::vector< cmaCommand* >& cmd_list = cmaCommandMgr::GetCommandList();
		
		const int num_cmds = cmd_list.size();
		PyObject* CmdList = PyList_New(num_cmds);
		if (CmdList != NULL)
		{
			std::string n_line = "\n";
			for (int i=0; i<num_cmds; ++i)
			{
				std::string cmd_name = pythUtil::MakeToken(cmd_list[i]->GetTag());
				std::string cmd_descr = cmd_list[i]->GetDescription();
				std::string cmd_full = cmd_name + " - " + cmd_descr;
				PyList_SetItem(CmdList, i, 
					PyString_FromString(cmd_full.c_str()));
				
			}

		//? Py_DECREF(CmdList);
		}
		
		return CmdList;
	}

	PyObject *
	list_verbose(PyObject *self, PyObject *args)
	{
		PyRun_SimpleString("for cmd in mach.setVerbose(): print cmd");
		
		return NULL;
	}


	PyObject *
	exec_command(PyObject *self, PyObject *args)
	{
		const char *commandName;
		if (!PyArg_ParseTuple(args, "s", &commandName))
			return NULL;

		const std::string cmdStr(commandName);

		const std::vector< cmaCommand* >& cmd_list = cmaCommandMgr::GetCommandList();

		const int num_cmds = cmd_list.size();
		for (int i=0; i<num_cmds; ++i)
		{
			std::string cmd_name = pythUtil::MakeToken(cmd_list[i]->GetTag());
			if (cmdStr == cmd_name)
			{
				cmd_list[i]->Execute();

				Py_INCREF(Py_None);
				return Py_None;
			}
		}

		PyErr_SetString(PyExc_NameError, "Could not find command by name.");
		return NULL;
	}

	//============================================================================
	// Creating cmaCommands that execute python functions
	//============================================================================
	class pythPythonCommand : public cmaCommand
	{
		public:
			//--------------------------------------------------------------------
			// constructor
			//--------------------------------------------------------------------
			pythPythonCommand(const std::string& i_CommandName,
							  PyObject *i_pFunction)
				: cmaCommand(  i_CommandName,
							   pythPythonCommand::PythonExecuteHandler,
							   NULL ),
				  m_pFunction(i_pFunction)
			{
				// Add a reference to callback
				Py_XINCREF(m_pFunction);        
	
				this->SetCategory("Python commands");
			}

			//--------------------------------------------------------------------
			// destructor
			//--------------------------------------------------------------------
			virtual ~pythPythonCommand()
			{
				// These cmaCommands are never really deleted and the
				// is no way to remove them from the command manager, 
				// so moving the de-reference into a separate function.
			}

			//--------------------------------------------------------------------
			// Remove reference to callback
			//--------------------------------------------------------------------
			void DeRef()
			{
				if (m_pFunction)
					Py_XDECREF(m_pFunction); 
				m_pFunction = NULL;
			}

		private:
			PyObject *m_pFunction;

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			static void PythonExecuteHandler(cmaCommand* pCmd)
			{
				pythPythonCommand* pCom = dynamic_cast<pythPythonCommand*>(pCmd);
				DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to PythonExecute command" );

				if (!pCom->m_pFunction) return;
					
				pythThreadLock thread_lock;
				PyObject *arglist = Py_BuildValue("()"); //? ("");
				PyObject *result = PyEval_CallObject(pCom->m_pFunction, arglist);
				Py_DECREF(arglist);

				// If result is NULL, then we may need to do something to 
				// communicate the error back to the user?
				if (result == NULL)
				{
					//	error! - need a NON-DEBUG ERROR message.  throw an error?
					//
					DBG_ERROR1("Python error (%s)", pCom->GetTag().c_str());
				}
				else
				{
					Py_DECREF(result);
				}
			}
	};

	std::vector<pythPythonCommand*> l_Commands;
	std::vector< std::string > l_ObjectNames;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void menu_command(const char* i_CommandName, PyObject* i_Function, const char* i_MainMenu, const char* i_SubMenu = NULL)
	{
		if (!PyCallable_Check(i_Function)) 
		{
			PyErr_SetString(PyExc_TypeError, "Function parameter must be callable");
			return;
		}

		if( i_MainMenu == NULL )
		{
			PyErr_SetString(PyExc_ValueError, "Main menu string cannot be NULL");
			return;	
		}

		int menu_id;

		//	create the command
		pythPythonCommand *pCommand = new pythPythonCommand(i_CommandName, i_Function);
		l_Commands.push_back(pCommand);
		
		//determine if there is a sub menu where the command will be placed
		if (i_SubMenu != NULL)
		{
			guiMenuMgr::AddMenu( i_MainMenu, i_SubMenu );
			menu_id = guiMenuMgr::AddMenuItem( i_SubMenu, i_CommandName);
			guiCommandMgr::Add(pCommand, i_CommandName, menu_id);
		}
		//if not just place the command into the main menu
		else
		{
			guiMenuMgr::AddMenu( i_MainMenu, "");
			menu_id = guiMenuMgr::AddMenuItem( i_MainMenu, i_CommandName);
			guiCommandMgr::Add(pCommand, i_CommandName, menu_id);
		}
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	PyObject * create_command(PyObject *dummy, PyObject *args)
	{
		const char *commandName = NULL;
		PyObject *function = NULL;

		if (PyArg_ParseTuple(args, "sO:createCommand", &commandName, &function)) 
		{
			menu_command(commandName, function, "Tools", "Python commands");
			
			Py_INCREF(Py_None);
			return Py_None;
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	PyObject * create_menu_command(PyObject *dummy, PyObject *args)
	{
		const char *mainMenu = NULL;
		const char *subMenu = NULL;
		const char *commandName = NULL;
		PyObject *function = NULL;

		if (PyArg_ParseTuple(args, "sOss:createMenuCommand", &commandName, &function, &mainMenu, &subMenu)) 
		{
			menu_command(commandName, function, mainMenu, subMenu);
			
			Py_INCREF(Py_None);
			return Py_None;

		}
		else if (PyArg_ParseTuple(args, "sOs:createMenuCommand", &commandName, &function, &mainMenu ))
		{
			menu_command(commandName, function, mainMenu);

			Py_INCREF(Py_None);
			return Py_None;
		}
		return NULL;
	}

	

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	PyObject * create_object(PyObject *dummy, PyObject *args)
	{
		const char *objectName = NULL;
		PyObject *function = NULL;

		if (PyArg_ParseTuple(args, "sO:createScriptObject", &objectName, &function)) 
		{
			if (!PyCallable_Check(function)) 
			{
				PyErr_SetString(PyExc_TypeError, "Function parameter must be callable");
				return NULL;
			}

			pythPythonCommand *pObject = new pythPythonCommand(objectName, function);
			l_ObjectNames.push_back( std::string(objectName) );

			cmaCommandMgr::Add(pObject, objectName, 0);
			Py_INCREF(Py_None);
			return Py_None;
		}

		return Py_None;
	}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythCommands::Initialize()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythCommands::DeInitialize()
{
	envSTLHelpers::ForAll(l_Commands, std::mem_fun(&pythPythonCommand::DeRef));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythCommands::GetPyObjectNames(std::vector<std::string> &ObjectNameList)
{
	ObjectNameList = l_ObjectNames;
}

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythCommands::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, "listCommands", "Get list of commands to execute.", list_commands);
	pythModules::AddCommand(i_ModuleName, "listVerbose", "Print list of commands, formatted with descriptions.", list_verbose);
	pythModules::AddCommand(i_ModuleName, "setVerbose", "Set up the list of commands and descriptions.", set_verbose);
	pythModules::AddCommand(i_ModuleName, "execCommand", "Execute command by name.", exec_command);
	pythModules::AddCommand(i_ModuleName, "createCommand", "Map menu command to python function", create_command);
	pythModules::AddCommand(i_ModuleName, "createMenuCommand", "Map menu command to python function, specify the menu, also submenu if necessary", create_menu_command);
	pythModules::AddCommand(i_ModuleName, "createScriptObject", "Map python commands that will appear in the available object list", create_object);
}
#endif
