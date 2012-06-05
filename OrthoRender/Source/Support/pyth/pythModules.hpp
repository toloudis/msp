/****************************************************************************\
**	pythModules.hpp
**
**		Gathers commands grouped into modeuls in order to submit 
**	them to the python interpretor.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_MODULES_HPP
#error pythModules.hpp multiply included
#endif
#define PYTH_MODULES_HPP

#ifndef PYTH_PYTHON_HPP
#include "Support/pyth/pythPython.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythModules
{
#if defined(PYTHON_ENABLED)
	typedef PyObject* (*CommandFunctionPtr)(PyObject* /*self*/, PyObject* /*pArgs*/);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	// Add a command to the given module.
	//--------------------------------------------------------------------
	void AddCommand(const std::string &i_ModuleName,
					const std::string &i_CommandName,
					const std::string &i_CommandDescription,
					CommandFunctionPtr i_FunctionPtr);

#endif
	//--------------------------------------------------------------------
	// Submit commands that have been gathered for each module.
	//--------------------------------------------------------------------
	void SubmitModules();
};

