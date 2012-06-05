/*****************************************************************************
**  mnmApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "mnmApp.hpp"

//#include "muiDialogTabbedMgr.hpp"
//#include "muiMenuMgr.hpp"
//#include "tma3dCursorMgr.hpp"
//#include "tma3dScreenUtil.hpp"
//#include "tma3dViewerMgr.hpp"

//	library
#include "appApplicationPAC.hpp"
#include "appSimTime.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appPackage.hpp"
#include "appTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "snSoundManager.hpp"


//
//	namespace
//
namespace
{
	mnmApp * l_pAppInstance = NULL;
	bool l_bActive = false;
}


//
void initialize_objects()
{
}

//
void deinitialize_objects()
{
}


//
//	mnmApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mnmApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool mnmApp::IsActive()
{
	return l_bActive;

	//if (l_pAppInstance)
	//{
	//	return true;
	//}

	//return false;
}


//--------------------------------------------------------------------
//	set this app as "active"
//--------------------------------------------------------------------
//static 
void mnmApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::mnmApp()
{
	l_pAppInstance = this;

	mnmApp::SetActive( false );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::~mnmApp()
{
	if (l_pAppInstance == this)
	{
		l_pAppInstance = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Think()
{
	float simTime = appSimTime::GetTime();

	// think the cursor manager
//	tma3dCursorMgr::Think();

	// think the camera manager
	//mnmCameraMgr::Think();

	//
	//	input
	//
//	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
//	if ( pKeyboard )
	{
		//if ( pKeyboard->IsReleased( inKeys::e_F8 ))
		//{
		//	//cmraCueDialogUtil::Show();
		//}
	}

	snSoundManager::Think( simTime );

	//	Think the running mode
	//
//	if( mnmModeMgr::IsEmpty() )
//	{
//		this->Exit();
//	}
//	else
//	{
//		mnmModeMgr::Think();
//	}

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}



