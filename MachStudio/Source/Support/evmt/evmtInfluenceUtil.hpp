/*****************************************************************************
**	evmtInfluenceUtil.hpp
**
**	Provides functions for determining the environments that influence
**	geometry and the geometry that is influenced by environments.
**
**	This information can be used by context menu commands to 
**	isolate lights and hide geometry outside of the influence.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef EVMT_INFLUENCEUTIL_HPP
#error evmtInfluenceUtil.hpp multiply included
#endif
#define EVMT_INFLUENCEUTIL_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <set>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace evmtInfluenceUtil
{
	//--------------------------------------------------------------------
	// Look for all pieces of geometry in the selection list
	// and then figure out which environments influence these pieces of
	// geometry. Return the environments in o_InfluenceEnvironments.
	//--------------------------------------------------------------------
	void  GetEnvironmentsForSelection(std::set<nameString>& o_InfluenceEnvironments);

}	// end of namespace
