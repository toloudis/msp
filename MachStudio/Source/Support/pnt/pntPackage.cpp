/*****************************************************************************
**  pntPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/pnt/pntPackage.hpp"

#include "Support/pnt/pntPickInterest.hpp"

#include "Tool/pick3d/pick3dMgr.hpp"


namespace
{
	int l_RefCount = 0;
	pntPickInterest*	l_pPointPI = 0;
}


//--------------------------------------------------------------------
// Init -- i_Parser, the user defined level parser.  NULL indicates
//			to the package that the default parsing system be used.
//--------------------------------------------------------------------
// static
void pntPackage::Init()
{
	if ( 0 == l_RefCount )
	{
		// initialize packages we depend on
		//

		// initialize our internal stuff
		//tmlnTimelineMgr::Initialize();

		// register the pick interest
		if ( l_pPointPI == 0 )
		{
			l_pPointPI = new pntPickInterest();
			pick3dMgr::RegisterPickInterest( l_pPointPI );
		}
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
void pntPackage::CleanUp()
{
	--l_RefCount;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( 0 == l_RefCount )
	{
		//	Unregister the Pick Interest
		if ( l_pPointPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pPointPI );
			delete l_pPointPI;
			l_pPointPI = 0;
		}

		//tmlnTimelineMgr::DeInitialize();

		// clean up packages we depend on, in reverse order
		//
		//lvlPackage::CleanUp();
	}
}

