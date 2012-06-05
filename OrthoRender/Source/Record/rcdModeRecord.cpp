/*****************************************************************************
**  rcdModeRecord.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Record/rcdModeRecord.hpp"

//	Mach Studio
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"

//	library
#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "InputDI/in/inDeviceMgr.hpp"
#include "InputDI/in/inVirtualJoystick.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Support/tmln/tmlnTimeline.hpp"

#include <windows.h>

namespace
{
	const float c_fRecordDelay = 3.0f; // delay between press record and playing

	bool	l_bPlaying = true;
	bool	l_bDelayed = true;
	float	l_TicksPerSecond = (int)tmlnTimeLine::GetFPS();
	int		l_CurrentTick = 0;
	float	l_fLastTime = 0.0f;
	float	l_fSecondsPerTick = 1.0f;
	float	l_fPlayScale = 1.0f;
	float	l_fBeginTime = 0.0f;
	float	l_fEndTime = 1.0f;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void calc_time()
	{
		l_fSecondsPerTick = 1.0f / l_TicksPerSecond;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void update_tick()
	{
		float simtime = appSimTime::GetTime();

		if (l_bPlaying)
		{
			if (simtime < l_fLastTime) return; // delayed start

			if (l_bDelayed)
			{
				// Beep again to notify starting recording
				MessageBeep(MB_OK);
				l_bDelayed = false;
			}

			float max_tick = l_fEndTime * l_TicksPerSecond;
			if ( (l_fLastTime + l_fSecondsPerTick) < simtime )
			{
				//float inc_ticks = ((simtime - l_fLastTime) * l_fPlayScale ) * l_TicksPerSecond;
				//l_CurrentTick += inc_ticks;
				l_CurrentTick++; // record every frame

				if (l_CurrentTick > max_tick)
				{
					l_CurrentTick = (int)max_tick;
					l_bPlaying = false;
				}

				l_fLastTime = simtime;
			}
		}
		else l_fLastTime = simtime;
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rcdModeRecord::rcdModeRecord()
:	m_bInitialized( false )
{
//	SetMenuItemName( "Record" ); // no menu item

	calc_time();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
rcdModeRecord::~rcdModeRecord()
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
void rcdModeRecord::Initialize()
{
	if ( !m_bInitialized )
	{
	}

	m_bInitialized = true;

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Record" );
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void rcdModeRecord::DeInitialize()
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
void rcdModeRecord::Think()
{
	modeModeTime::Think();

			// debug
	char text[64];
	sprintf(text, "recording %s (%d) (time%6.2f)", (l_bPlaying ? "true":"false"), l_CurrentTick, l_fLastTime );
	mnmDebugInfo::SetDebugInfo(11, text);


	update_tick();

	//	set the timeline
	//
	float time = l_CurrentTick * l_fSecondsPerTick;
	if (time > tmlnTimeLine::GetMaximum())
		time = tmlnTimeLine::GetMaximum();
	tmlnTimeLine::SetValue( time );
}

//--------------------------------------------------------------------
//	playback controls
//--------------------------------------------------------------------
void rcdModeRecord::Pause()
{
	l_bPlaying = false;
}
void rcdModeRecord::Play(float i_BeginTime, float i_EndTime)
{
	l_bPlaying = true;
	l_fPlayScale = 1.0f;

	l_fBeginTime = i_BeginTime;
	if (l_fBeginTime < tmlnTimeLine::GetMinimum())
		l_fBeginTime = tmlnTimeLine::GetMinimum();
	l_fEndTime = i_EndTime;
	if (l_fEndTime > tmlnTimeLine::GetMaximum())
		l_fEndTime = tmlnTimeLine::GetMaximum();

	l_CurrentTick = (int)(i_BeginTime * l_TicksPerSecond);


	l_fLastTime = appSimTime::GetTime() + c_fRecordDelay;
	l_bDelayed = true;

	// Beep once to notify starting countdown
	MessageBeep(MB_OK);
}
