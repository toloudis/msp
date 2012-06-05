/****************************************************************************\
**	pythEnvironments.hpp
**
**		Environments related python commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_ENVIRONMENTS_HPP
#error pythEnvironments.hpp multiply included
#endif
#define PYTH_ENVIRONMENTS_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythEnvironments
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
