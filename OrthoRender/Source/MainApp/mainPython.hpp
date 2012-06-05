/****************************************************************************\
**	mainPython.hpp
**
**		Python commands related to the main form
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_PYTHON_HPP
#error mainPython.hpp multiply included
#endif
#define MAIN_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace mainPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
