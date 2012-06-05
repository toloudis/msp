/****************************************************************************\
**	pythControlProperty.hpp
**
**		Control Property related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
****************************************************************************/
#ifdef PYTH_CONTROL_PROPERTY_HPP
#error pythControlProperty.hpp multiply included
#endif
#define PYTH_CONTROL_PROPERTY_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythControlProperty
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

	
};