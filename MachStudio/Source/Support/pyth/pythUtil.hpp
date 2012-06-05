/****************************************************************************\
**	pythUtil.hpp
**
**		Functions for executing python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_UTIL_HPP
#error pythUtil.hpp multiply included
#endif
#define PYTH_UTIL_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
namespace pythUtil
{
	//python locators
	enum PythonDirectories
	{
		e_Python,
		e_UserPython,
		e_PythonObjects
	};

	//--------------------------------------------------------------------
	// Execute the following code in the python interpretor
	//--------------------------------------------------------------------
	bool ExecuteCommand(const std::string& i_Command);
	bool ExecuteCommand(const char *i_Command);

	//--------------------------------------------------------------------
	// Execute the following python script file.
	//--------------------------------------------------------------------
	void ScriptFile(fsLocator &i_ScriptLoc, std::string& i_Arguments);
	void ScriptFile(fsLocator &i_ScriptLoc);

	//--------------------------------------------------------------------
	// Convert a general string to a token without whitespace
	//	that can be used as a Python command or argument.
	//--------------------------------------------------------------------
	std::string MakeToken(const std::string &i_String);
};
