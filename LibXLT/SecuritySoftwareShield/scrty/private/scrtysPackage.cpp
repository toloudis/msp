/*****************************************************************************
**	scrtysPackage.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "SecuritySoftwareShield/scrty/scrtysPackage.hpp"

#include "SecuritySoftwareShield/scrty/scrtysMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
	scrtysMgr* l_pSecurityMgrImpl = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtysPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pSecurityMgrImpl = new scrtysMgr;
		scrtyMgr::SetImplementation( l_pSecurityMgrImpl );

		scrtyMgr::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtysPackage::DeInitialize()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		scrtyMgr::CleanUp();

		// clean up our internal stuff, in reverse order
		//
		delete l_pSecurityMgrImpl;
		l_pSecurityMgrImpl = NULL;
		scrtyMgr::SetImplementation(NULL);

		// clean up packages we depend on
	}
}

