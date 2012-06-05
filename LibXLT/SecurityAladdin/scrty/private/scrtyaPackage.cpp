/*****************************************************************************
**  scrtyaPackage.cpp
**
**      see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "SecurityAladdin/scrty/scrtyaPackage.hpp"

#include "SecurityAladdin/scrty/scrtyaMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
	scrtyaMgr* l_pSecurityMgrImpl = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtyaPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pSecurityMgrImpl = new scrtyaMgr;
		scrtyMgr::SetImplementation( l_pSecurityMgrImpl );

		scrtyMgr::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtyaPackage::DeInitialize()
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

