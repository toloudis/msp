/*****************************************************************************
**	pythRenderLayers.hpp
**
**		Set of python operations that will give access to render layers
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/

#ifdef PYTH_RENDERLAYERS_HPP
#error pythRenderLayers.hpp multiply included
#endif
#define PYTH_RENDERLAYERS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythRenderLayers
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

};