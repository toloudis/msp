/****************************************************************************\
**	keyfPython.hpp
**
**		Python commands related to channels and keyframing.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef KEYF_PYTHON_HPP
#error keyfPython.hpp multiply included
#endif
#define KEYF_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace keyfPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
