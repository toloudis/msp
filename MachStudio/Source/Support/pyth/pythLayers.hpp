/****************************************************************************\
**	pythLayers.hpp
**
**		Layers related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_LAYERS_HPP
#error pythLayers.hpp multiply included
#endif
#define PYTH_LAYERS_HPP

#include <string>

//============================================================================
//============================================================================
namespace pythLayers
{
	//--------------------------------------------------------------------
	// Add commands to given module
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
