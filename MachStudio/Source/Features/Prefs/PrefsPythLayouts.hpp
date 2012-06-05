/****************************************************************************\
**	PrefsPythLayouts.hpp
**
**		Layout related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PREFS_PYTH_LAYOUTS_HPP
#error PrefsPythLayouts.hpp multiply included
#endif
#define PREFS_PYTH_LAYOUTS_HPP

#include <string>

//============================================================================
//============================================================================
namespace prefsPython
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
