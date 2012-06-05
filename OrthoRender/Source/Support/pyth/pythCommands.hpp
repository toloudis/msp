/****************************************************************************\
**	pythCommands.hpp
**
**		Connects cmaCommandMgr commands to python commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_COMMANDS_HPP
#error pythCommands.hpp multiply included
#endif
#define PYTH_COMMANDS_HPP

#include <string>
#include <vector>
//============================================================================
//============================================================================
namespace pythCommands
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void DeInitialize();
	
	void GetPyObjectNames( std::vector<std::string> &ObjectNameList);
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
