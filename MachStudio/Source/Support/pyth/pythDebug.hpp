/*****************************************************************************
**	pythDebug.hpp
**
**		Set of python operations write to the debug log file
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/

#ifdef PYTH_DEBUG_HPP
#error pythDebug.hpp multiply included
#endif
#define PYTH_DEBUG_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythDebug
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

};