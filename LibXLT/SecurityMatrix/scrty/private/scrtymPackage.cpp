/*****************************************************************************
**  scrtymPackage.cpp
**
**      see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "SecurityMatrix/scrty/scrtymPackage.hpp"

#include "SecurityMatrix/scrty/scrtymMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
	scrtymMgr* l_pSecurityMgrImpl = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtymPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pSecurityMgrImpl = new scrtymMgr;
		scrtyMgr::SetImplementation( l_pSecurityMgrImpl );

		scrtyMgr::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scrtymPackage::DeInitialize()
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

