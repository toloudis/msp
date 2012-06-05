/****************************************************************************\
**	pythDialogs.hpp
**
**		Dialog related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_DIALOGS_HPP
#error pythDialogs.hpp multiply included
#endif
#define PYTH_DIALOGS_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythDialogs
{
	//--------------------------------------------------------------------
	// Add commands related to interface dialogs
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
