/****************************************************************************\
**	cmmPython.hpp
**
**		Python commands related to Available and Placed lists
**	in System dialog.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_PYTHON_HPP
#error cmmPython.hpp multiply included
#endif
#define CMM_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace cmmPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
