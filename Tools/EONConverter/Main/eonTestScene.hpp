/*****************************************************************************
**	eonTestScene.hpp
**
**		Utilities for exporting geometry files to EON Reality file format
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef EON_TESTSCENE_HPP
#error eonTestScene.hpp multiply included
#endif
#define EON_TESTSCENE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>

//============================================================================
//	forward references
//============================================================================
class mdlFragInfo;

//============================================================================
//============================================================================
namespace eonTestScene
{
	//------------------------------------------------------------------------
	//  DoEONTest() - write sample EON file.
	//------------------------------------------------------------------------
	void  DoEONTest();

	//------------------------------------------------------------------------
	//  DoFragTest() - write sample EON file using one of our fragments
	//------------------------------------------------------------------------
	void  DoFragTest(const mdlFragInfo& i_Info);
	
	//------------------------------------------------------------------------
	//  DoFragTest() - write sample EON file using a set of fragments
	//------------------------------------------------------------------------
	void  DoFragTest(const std::vector< shared_ptr<mdlFragInfo> >& i_Infos);
}
