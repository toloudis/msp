/****************************************************************************\
**	prjPython.hpp
**
**		Python commands related to the project settings
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PRJ_PYTHON_HPP
#error prjPython.hpp multiply included
#endif
#define PRJ_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace prjPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
