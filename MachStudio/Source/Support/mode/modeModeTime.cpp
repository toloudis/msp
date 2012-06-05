/*****************************************************************************
**	modeModeTime.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mode/modeModeTime.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

// tool
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeLineMgr.hpp"

// library
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/Sc/scBillboard.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "AudioDS/sn/snSoundManager.hpp"

#if(SGPU_APP == MS_FUSION) //#ifdef FUSION
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#endif


//============================================================================
//============================================================================
namespace
{
	bool l_bTriggeredDriver = false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
modeModeTime::modeModeTime()
:	m_bInitialized( false )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
modeModeTime::~modeModeTime()
{
}

//----------------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of appMode should remember to call
//	appMode::Initialize() at the beginning of their Initialize
//	function.
//----------------------------------------------------------------------------
//virtual
void modeModeTime::Initialize()
{
	if ( !m_bInitialized )
	{
		l_bTriggeredDriver = false;
	}

	m_bInitialized = true;
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void modeModeTime::DeInitialize()
{
	if ( m_bInitialized )
	{
	}

	m_bInitialized = false;
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void modeModeTime::Think()
{
	modeMode::Think();

		// debug
	//char text[64];
	//sprintf(text, "timeline %d %5.2f", tmlnTimeLine::GetValue(), tmlnTimeLine::GetPercentage() );
	//mnmDebugInfo::SetDebugInfo(14, text);

	maTime timeline_time = tmlnTimeLine::GetValue();

	//	timeline mgr updates
	tmlnTimelineMgr::Update( timeline_time );

	// Sound mgr think - converting from maTime to seconds
	snSoundManager::Think(timeline_time.AsSeconds());
	//snSoundManager::SetSoundJobsListenerPosition(this->GetCamera().GetPosition());

	// Call Think() for all systems who have registered an interest
	mnmThinkMgr::Think();

	#if(SGPU_APP == MS_FUSION) //#ifdef FUSION
	if( fcuiFormMgr::MainHasFocus() )
		cam3dMgr::Think();
	#else
		cam3dMgr::Think();
	#endif

	//cam3dMgr::GetCamera().LookAt(maPoint3d(10,10,0), maPoint3d(0,0,0), maVector3d(0,1,0));

	//bga - these two calls that set up transforms based on camera positions
	// really need to be somewhere in a single viewport render´s steps.
	//
	// Orient the billboards to the camera
	scBillboard::SetCameraPosition( cam3dMgr::GetCamera() );
	// Update LOD based on camera position
	api3dScene::UpdateLOD( cam3dMgr::GetCamera().GetPosition() );

	//bga - this call is removed from the mode think 
	// in order to put it into the render thread
	//api3dScene::Think( timeline_time );
}

