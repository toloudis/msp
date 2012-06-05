/*****************************************************************************
**	fioPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fio/fioPackage.hpp"

//#include "FCSupport/fio/fioXXXXMgr.hpp"

#include <stdio.h>


//============================================================================
//============================================================================
namespace fioPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		//if ( fioXXXXMgr::Instance == NULL )
		//{
		//	fioXXXXMgr::Instance = new fioXXXXMgr();
		//	fioXXXXMgr::Instance->Initialize();
		//}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//if ( fioXXXXMgr::Instance != NULL )
		//{
		//	fioXXXXMgr::Instance->DeInitialize();
		//	delete fioXXXXMgr::Instance;
		//	fioXXXXMgr::Instance = NULL;
		//}
	}
}
