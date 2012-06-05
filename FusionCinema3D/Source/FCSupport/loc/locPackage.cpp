/*****************************************************************************
**	locPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/loc/locPackage.hpp"

//#include "FCSupport/loc/locXXXXMgr.hpp"

#include <stdio.h>


//============================================================================
//============================================================================
namespace locPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		//if ( locXXXXMgr::Instance == NULL )
		//{
		//	locXXXXMgr::Instance = new locXXXXMgr();
		//	locXXXXMgr::Instance->Initialize();
		//}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//if ( locXXXXMgr::Instance != NULL )
		//{
		//	locXXXXMgr::Instance->DeInitialize();
		//	delete locXXXXMgr::Instance;
		//	locXXXXMgr::Instance = NULL;
		//}
	}
}
