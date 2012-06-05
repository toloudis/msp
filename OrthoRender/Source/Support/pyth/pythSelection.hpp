/****************************************************************************\
**	pythSelection.hpp
**
**		Selection related python commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_SELECTION_HPP
#error pythSelection.hpp multiply included
#endif
#define PYTH_SELECTION_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythSelection
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
