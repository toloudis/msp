/*****************************************************************************
**  splnPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/spln/splnPackage.hpp"

#include "Support/spln/splnPickInterest.hpp"

#include "Tool/pick3d/pick3dMgr.hpp"


namespace
{
	int l_RefCount = 0;
	splnPickInterest*	l_pSplinePI = 0;
}


//--------------------------------------------------------------------
// Init -- i_Parser, the user defined level parser.  NULL indicates
//			to the package that the default parsing system be used.
//--------------------------------------------------------------------
// static
void splnPackage::Init()
{
	if ( 0 == l_RefCount )
	{
		// initialize packages we depend on
		//

		// initialize our internal stuff
		//tmlnTimelineMgr::Initialize();

		// register the pick interest
		if ( l_pSplinePI == 0 )
		{
			l_pSplinePI = new splnPickInterest();
			pick3dMgr::RegisterPickInterest( l_pSplinePI );
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
void splnPackage::CleanUp()
{
	--l_RefCount;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( 0 == l_RefCount )
	{
		//	Unregister the Pick Interest
		if ( l_pSplinePI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pSplinePI );
			delete l_pSplinePI;
			l_pSplinePI = 0;
		}

		//tmlnTimelineMgr::DeInitialize();

		// clean up packages we depend on, in reverse order
		//
		//lvlPackage::CleanUp();
	}
}

