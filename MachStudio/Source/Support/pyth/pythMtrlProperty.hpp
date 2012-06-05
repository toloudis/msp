/****************************************************************************\
**	pythMtrlProperty.hpp
**
**		Material Property related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_MTRL_PROPERTY_HPP
#error pythMtrlProperty.hpp multiply included
#endif
#define PYTH_MTRL_PROPERTY_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythMtrlProperty
{
	
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);

	
};
