/*****************************************************************************
**	cptrPythonCommands.hpp
**
**		Set of python operations that will give access to the capture commands
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/

#ifdef CPTR_PYTHONCOMMANDS_HPP
#error cptrPythonCommands.hpp multiply included
#endif
#define CPTR_PYTHONCOMMANDS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
namespace cptrPythonCommands
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

};