/****************************************************************************\
**	trfnPython.hpp
**
**		Parent related python commands
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_PYTHON_HPP
#error trfnPython.hpp multiply included
#endif
#define TRFN_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace trfnPython
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
