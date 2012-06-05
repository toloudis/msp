/****************************************************************************\
**	xtraPython.hpp
**
**		Python commands for adding extra custom properties to object
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef XTRA_PYTHON_HPP
#error xtraPython.hpp multiply included
#endif
#define XTRA_PYTHON_HPP

#include <string>


//============================================================================
//============================================================================
namespace xtraPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
