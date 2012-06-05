/*****************************************************************************
**  tmlnPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnPackage.hpp"

#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

//#include "Core/gf/gfErrorHandler.hpp"
//#include "lvlParserMgr.hpp"
//#include "lvlPackage.hpp"

namespace
{
	int l_RefCount = 0;
}


//--------------------------------------------------------------------
// Init -- i_Parser, the user defined level parser.  NULL indicates
//			to the package that the default parsing system be used.
//--------------------------------------------------------------------
// static
void tmlnPackage::Init()
{
	if ( 0 == l_RefCount )
	{
		// initialize packages we depend on
		//

		// initialize our internal stuff
		tmlnCreator::Initialize();
		tmlnParser::Initialize();
		tmlnTimelineMgr::Initialize();
	}
	++l_RefCount;
}

//--------------------------------------------------------------------
// CleanUp -- cleans up all relevent package data.  It also destroys
// all waypoint lists that may have been filled when the data was read
// from the level file and/or added by the user.
// Something the user may do manually if desired.
//--------------------------------------------------------------------
// static
void tmlnPackage::CleanUp()
{
	--l_RefCount;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( 0 == l_RefCount )
	{
		tmlnCreator::DeInitialize();
		tmlnParser::DeInitialize();
		tmlnTimelineMgr::DeInitialize();

		// clean up packages we depend on, in reverse order
		//
		//lvlPackage::CleanUp();
	}
}

