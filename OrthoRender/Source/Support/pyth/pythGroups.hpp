/****************************************************************************\
**	pythGroups.hpp
**
**		Groups related python commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_GROUPS_HPP
#error pythGroups.hpp multiply included
#endif
#define PYTH_GROUPS_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythGroups
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
