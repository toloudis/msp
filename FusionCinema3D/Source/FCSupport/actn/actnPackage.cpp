/*****************************************************************************
**	actnPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnPackage.hpp"

#include "FCSupport/actn/actnLightMgr.hpp"
#include "FCSupport/actn/actnAudioMgr.hpp"
#include "FCSupport/actn/actnTitleCardMgr.hpp"
#include "FCSupport/actn/actnSceneMgr.hpp"
#include "FCSupport/actn/actnPostFXMgr.hpp"
#include <stdio.h>


//============================================================================
//============================================================================
namespace actnPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		if ( actnLightMgr::Instance == NULL )
		{
			actnLightMgr::Instance = new actnLightMgr();
			actnLightMgr::Instance->Initialize();

			actnSceneMgr::Instance = new actnSceneMgr();
			actnSceneMgr::Instance->Initialize();

			actnAudioMgr::Instance = new actnAudioMgr();
			actnAudioMgr::Instance->Initialize();

		}
		if ( actnPostFXMgr::Instance == NULL )
		{
			actnPostFXMgr::Instance = new actnPostFXMgr();
			actnPostFXMgr::Instance->Initialize();
		}
		if ( actnTitleCardMgr::Instance == NULL )
		{
			actnTitleCardMgr::Instance = new actnTitleCardMgr();
			actnTitleCardMgr::Instance->Initialize();
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if ( actnLightMgr::Instance != NULL )
		{
			actnLightMgr::Instance->DeInitialize();
			delete actnLightMgr::Instance;
			actnLightMgr::Instance = NULL;

			actnSceneMgr::Instance->DeInitialize();
			delete actnSceneMgr::Instance;
			actnSceneMgr::Instance = NULL;

			actnAudioMgr::Instance->DeInitialize();
			delete actnAudioMgr::Instance;
			actnAudioMgr::Instance = NULL;
		}
		if ( actnPostFXMgr::Instance != NULL )
		{

			actnPostFXMgr::Instance->DeInitialize();
			delete actnPostFXMgr::Instance;
			actnPostFXMgr::Instance = NULL;
		}

		if ( actnTitleCardMgr::Instance != NULL )
		{
			actnTitleCardMgr::Instance->DeInitialize();
			delete actnTitleCardMgr::Instance;
			actnTitleCardMgr::Instance = NULL;
		}
	}
}
