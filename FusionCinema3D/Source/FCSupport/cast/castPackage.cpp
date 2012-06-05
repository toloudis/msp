/*****************************************************************************
**	castPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/cast/castPackage.hpp"

//#include "FCSupport/cast/castXXXXMgr.hpp"

#include <stdio.h>


//============================================================================
//============================================================================
namespace castPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		//if ( castXXXXMgr::Instance == NULL )
		//{
		//	castXXXXMgr::Instance = new castXXXXMgr();
		//	castXXXXMgr::Instance->Initialize();
		//}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//if ( castXXXXMgr::Instance != NULL )
		//{
		//	castXXXXMgr::Instance->DeInitialize();
		//	delete castXXXXMgr::Instance;
		//	castXXXXMgr::Instance = NULL;
		//}
	}
}
