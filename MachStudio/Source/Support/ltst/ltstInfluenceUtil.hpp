/*****************************************************************************
**	ltstInfluenceUtil.hpp
**
**	Provides functions for determining the lights that influence
**	geometry and the geometry that is influenced by lights.
**
**	This information can be used by context menu commands to 
**	isolate lights and hide geometry outside of the influence.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef LTST_INFLUENCEUTIL_HPP
#error ltstInfluenceUtil.hpp multiply included
#endif
#define LTST_INFLUENCEUTIL_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <set>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace ltstInfluenceUtil
{
	//--------------------------------------------------------------------
	// Look for all pieces of geometry in the selection list
	// and then figure out which light sets influence these pieces of
	// geometry. Return the light sets in o_InfluenceLightSets.
	//--------------------------------------------------------------------
	void  GetLightSetsForSelectedObjects(std::set<nameString>& o_InfluenceLightSets);

}	// end of namespace
