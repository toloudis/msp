/*****************************************************************************
**  modeModeTime.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/mode/modeModeTime.hpp"

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
#include "InputDI/in/inDeviceMgr.hpp"
#include "AudioDS/sn/snSoundManager.hpp"


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

	float timeline_time = tmlnTimeLine::GetValue();

	//	timeline mgr updates
	tmlnTimelineMgr::Update( timeline_time );

	// Sound mgr think
	snSoundManager::Think(timeline_time);
	//snSoundManager::SetSoundJobsListenerPosition(this->GetCamera().GetPosition());

	// Call Think() for all systems who have registered an interest
	mnmThinkMgr::Think();

	cam3dMgr::Think();
	//cam3dMgr::GetCamera().LookAt(maPoint3d(10,10,0), maPoint3d(0,0,0), maVector3d(0,1,0));

	cmpsCompassMgr::Think( cam3dMgr::GetCamera().GetPosition() );

	api3dScene::UpdateLOD( cam3dMgr::GetCamera().GetPosition() );
	api3dScene::Think( timeline_time );
}

