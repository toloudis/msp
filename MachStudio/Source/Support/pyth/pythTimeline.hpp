/****************************************************************************\
**	pythTimeline.hpp
**
**		Timeline related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_TIMELINE_HPP
#error pythTimeline.hpp multiply included
#endif
#define PYTH_TIMELINE_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythTimeline
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
