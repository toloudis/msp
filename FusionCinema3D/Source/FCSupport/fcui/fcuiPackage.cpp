/*****************************************************************************
**	fcuiPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/fcuiPackage.hpp"

#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"


//============================================================================
//============================================================================
namespace fcuiPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		if ( fcuiTimelineMgr::Instance == NULL )
		{
			fcuiTimelineMgr::Instance = new fcuiTimelineMgr();
		}

		fcuiFormMgr::Init();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		fcuiFormMgr::CleanUp();

		if ( fcuiTimelineMgr::Instance != NULL )
		{
			delete fcuiTimelineMgr::Instance;
			fcuiTimelineMgr::Instance = NULL;
		}
	}
}