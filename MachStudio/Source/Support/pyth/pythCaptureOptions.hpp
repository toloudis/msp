/*****************************************************************************
**	pythCaptureOptions.hpp
**
**		Set of python operations that will give access to the capture options
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/

#ifdef PYTH_CAPTUREOPTIONS_HPP
#error pythCaptureOptions.hpp multiply included
#endif
#define PYTH_CAPTUREOPTIONS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythCaptureOptions
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

};