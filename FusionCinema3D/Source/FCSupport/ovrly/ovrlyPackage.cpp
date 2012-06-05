/*****************************************************************************
**	ovrlyPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/ovrly/ovrlyPackage.hpp"

#include "FCSupport/ovrly/ovrlyMgr.hpp"

#include <stdio.h>


//============================================================================
//============================================================================
namespace ovrlyPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		if ( ovrlyMgr::Instance == NULL )
		{
//#define USE_OVERLAYS
#ifdef USE_OVERLAYS
			ovrlyMgr::Instance = new ovrlyMgr();
			ovrlyMgr::Instance->Initialize();
#endif
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if ( ovrlyMgr::Instance != NULL )
		{
			ovrlyMgr::Instance->DeInitialize();
			delete ovrlyMgr::Instance;
			ovrlyMgr::Instance = NULL;
		}
	}
}
