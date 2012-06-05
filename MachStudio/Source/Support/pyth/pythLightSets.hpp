/****************************************************************************\
**	pythLightSets.hpp
**
**		LightSets related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_LIGHTSETS_HPP
#error pythLightSets.hpp multiply included
#endif
#define PYTH_LIGHTSETS_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythLightSets
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
