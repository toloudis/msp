/*****************************************************************************
**  fcmdPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "FCSupport/fcmd/fcmdPackage.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"


//============================================================================
//============================================================================
namespace fcmdPackage
{
	namespace
	{
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		if( fcmdModeMgr::Instance == NULL )
		{
			fcmdModeMgr::Instance = new fcmdModeMgr();
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		delete fcmdModeMgr::Instance;
		fcmdModeMgr::Instance = NULL;
	}
}