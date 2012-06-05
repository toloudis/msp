/*****************************************************************************
**  plbkModePlayback.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/plbkModePlayback.hpp"

#include "Features/Playback/plbkPlaybackControlsDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"

#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/name/nameString.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inVirtualJoystick.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Support/vis/visMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	bool	l_bShowTimeCode = true;
	bool	l_bLooping = false;
	bool	l_bPlaying = false;
	bool	l_bPlayingForward = true;
	bool	l_bLowRes = true;
	bool	l_bMute = false;
	float	l_TicksPerSecond = g3dConstants::c_fDefaultFrameRate;
	float	l_CurrentTick = 0.0f;
	float	l_fLastTime = 0.0f;
	float	l_fSecondsPerTick = 1.0f;
	float	l_fPlayScale = 1.0f;
	bool    l_appIsFusion = false;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void calc_time()
	{
		l_fSecondsPerTick = 1.0f / l_TicksPerSecond;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void set_lasttime(float i_SimTime)
	{
		l_fLastTime = i_SimTime;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void set_lasttime()
	{
		set_lasttime( appSimTime::GetTime() );
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void update_tick()
	{
		//	decide what to do based on state flags
		//
		if (l_bPlaying)
		{
			//bga - moved here from appMode::Think()
			appSimTime::IncrementTime();
			float simtime = appSimTime::GetTime();

			snSoundManager::Mute( l_bMute );

			//TIME - math in seconds, could be maTime?
			float min_tick = tmlnTimeLine::GetMinimum().AsSeconds() * l_TicksPerSecond;
			float max_tick = tmlnTimeLine::GetMaximum().AsSeconds() * l_TicksPerSecond;
			if ( (l_fLastTime + l_fSecondsPerTick) < simtime )
			{
				if ( l_bPlayingForward )
				{
					if((simtime - l_fLastTime) != 1)	
						l_CurrentTick += (((simtime - l_fLastTime) * l_fPlayScale ) * l_TicksPerSecond);
					else
						l_CurrentTick += (simtime - l_fLastTime) * l_fPlayScale;

					if ((l_bLooping) && (l_CurrentTick > max_tick))
					{
						tmlnTimeLine::SetLoopingToBeginning();

						l_CurrentTick = min_tick;
					}
				}
				else
				{
					if((simtime - l_fLastTime) != 1)	
						l_CurrentTick -= (((simtime - l_fLastTime) * l_fPlayScale) * l_TicksPerSecond);
					else
						l_CurrentTick -= (simtime - l_fLastTime) * l_fPlayScale;

					if ((l_bLooping) && (l_CurrentTick < min_tick))
					{
						l_CurrentTick = max_tick;
					}
				}

				set_lasttime(simtime);

				//DBG_LOG4( "      last(%6.3f) timeline(%6.3f) simtime(%6.3f)  ctick(%6.3f)", l_fLastTime, tmlnTimeLine::GetValue(), simtime, l_CurrentTick );
			}
		}
		//else
		//{
		//	set_lasttime(simtime);
		//}
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void set_timeline()
	{
		//TIME - math in seconds, could it be maTime?
		float time = l_CurrentTick * l_fSecondsPerTick;
		tmlnTimeUtil::AdjustTimeToFrame(time);

		maTime new_time = maTime::FromSeconds(time);
		if (new_time < tmlnTimeLine::GetMinimum())
		{
			new_time = tmlnTimeLine::GetMinimum();
		}
		else if (new_time > tmlnTimeLine::GetMaximum())
		{
			new_time = tmlnTimeLine::GetMaximum();
		}

		// Only update if different
		if (new_time != tmlnTimeLine::GetValue())
			tmlnTimeLine::SetValue( new_time );

		//DBG_LOG( "Times :  timeline=" << tmlnTimeLine::GetValue() << "  sim time=" << appSimTime::GetTime() << "  pback time=" << time );
	}

	//--------------------------------------------------------------------
	//	set the last time and the current tick (in case the user scratched
	//	while paused)
	//--------------------------------------------------------------------
	void confirm_current_time()
	{
		set_lasttime();
		l_CurrentTick = (tmlnTimeLine::GetTimeInSeconds() * l_TicksPerSecond);

		//DBG_LOG2( "PLAY: last(%6.3f) timeline(%6.3f)", l_fLastTime, tmlnTimeLine::GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void update_statusbar()
	{
		guiStatusBarMgr::SetText(mnmConstants::e_SBPanel_Mode, "Playback");
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
plbkModePlayback::plbkModePlayback()
:	m_bInitialized( false )
{
	SetMenuItemName( "Playback Controls" );

	calc_time();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
plbkModePlayback::~plbkModePlayback()
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
void plbkModePlayback::Initialize()
{
	//if ( !m_bInitialized )
	//{
	//}

	this->SetTerminateCondition(appMode::e_Continue);

	l_TicksPerSecond = tmlnTimeLine::GetFPS();

	PrefsData prefsdata = PrefsMgr::Data();
	l_bShowTimeCode = prefsdata.m_bPlaybackTimeCode.GetValue();
	l_bLooping = prefsdata.m_bPlaybackLoopAtEnd.GetValue();
	l_bLowRes = prefsdata.m_bPlaybackLowRes.GetValue();
	l_bMute = prefsdata.m_bMuteAudio.GetValue();

	if ( l_bShowTimeCode )
	{
		mnmTimeCodeUtil::SetWindow(0);		// render target 0
		mnmTimeCodeMgr::Initialize();
		mnmTimeCodeMgr::SetShowTimeCode(true);
	}
	else
	{
		mnmTimeCodeMgr::SetShowTimeCode(false);
	}

	m_bInitialized = true;

	update_statusbar();
	visMgr::ShowIcons(false);
	cmpsCompassMgr::SetRenderable( false );	// set all compasses invisible
	cmpsSelectMgr::SetRenderable(false);

	plbkPlaybackControlsDialogUtil::Show();

	//l_CurrentTick = (tmlnTimeLine::GetValue() * l_TicksPerSecond);
	//set_lasttime();
	//appSimTime::SetMaxTimeDelta(500.0f);	// added -- needed?
	//
	//Play();
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void plbkModePlayback::DeInitialize()
{
	if ( m_bInitialized )
	{
		appSimTime::SetMaxTimeDelta(1.0f);

		plbkPlaybackControlsDialogUtil::Hide();

		//	on leaving the mode don't remove the menu items for this mode,
		//	just disable them.
		//
		//guiMenuMgr::EnableMenuItem( "&Objects", "&Translate", false );

		rpnRenderingPrefs::RestoreDefaults();
		rpnRenderingPrefs::SetupRenderingHints();
	}

	mnmTimeCodeMgr::DeInitialize();
	mnmTimeCodeMgr::SetShowTimeCode(false);

	m_bInitialized = false;
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void plbkModePlayback::Think()
{
	//
	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// DEBUG
	//char text[64];
	//sprintf(text, "playing %s %s (%d) (time%6.2f)", (l_bPlaying ? "true":"false"), (l_bPlayingForward ? "forward":"backward"), l_CurrentTick, l_fLastTime );
	//mnmDebugInfo::SetDebugInfo(11, text);


	// Check for input
	//
	if ( pKeyboard )
	{
		if(!l_appIsFusion)
		{
			if ( pKeyboard->IsReleased(inKeys::e_SPACE) )
			{
				TogglePlayAndPause();
			}
		}
		//if ( pKeyboard->IsReleased(inKeys::e_F) )
		//{
		//	l_bPlayingForward = true;
		//	l_bPlaying = true;
		//}
		//else
		//if ( pKeyboard->IsReleased(inKeys::e_S) )
		//{
		//	l_CurrentTick = 0;
		//	l_bPlaying = false;
		//}
	}

	// Playback should only advance the frame time if some old
	// frame is not still rendering. It can only render as many frames
	// as the render frame rate will allow.
	if (!g3dThreadControl::IsRenderThreadActive())
	{
		if (l_bPlaying)
		{
			update_tick();

			//	set the timeline
			//
			set_timeline();
			this->CheckForEnd();
		}
		
		//	update the timecode
		//
		if ( l_bShowTimeCode )
		{
			mnmTimeCodeMgr::Update();
		}
	}

	// Set up rendering preferences based on our low-res state
	rpnRenderingPrefs::SetAllLowResolution(l_bLowRes);
	rpnRenderingPrefs::SetupRenderingHints();

	// By having the base class think go last, the drivers 
	// and other things that have registered Think() interests
	// get a chance to execute after the mode has changed the timeline.
	modeModeTime::Think();
}

//----------------------------------------------------------------------------
//	IsModal() - signals whether the mode should be pushed onto the mode stack
//	ON TOP of the current mode, or replace the current mode (via Pop)
//
//	true - the mode will be pushed on top of the current mode.
//	false- the mode will replace the current mode.
//----------------------------------------------------------------------------
//virtual
bool plbkModePlayback::IsModal()
{
	return true;
}

//----------------------------------------------------------------------------
//	Quit level
//----------------------------------------------------------------------------
void plbkModePlayback::ExitMode()
{
	this->SetTerminateCondition(appMode::e_TerminateAndRemove);
}

//----------------------------------------------------------------------------
/// Return if the mode is currently playing
//----------------------------------------------------------------------------
bool plbkModePlayback::IsPlaying()
{
	if ( m_bInitialized )
	{
		return l_bPlaying;
	}
	return false;
}

//--------------------------------------------------------------------
//	playback controls
//--------------------------------------------------------------------
void plbkModePlayback::Pause()
{
	l_bPlaying = false;
}
void plbkModePlayback::FastFwd()
{
	l_fPlayScale = 2.0f;
	l_bPlaying = true;
	l_bPlayingForward = true;
	confirm_current_time();
}
void plbkModePlayback::FastRev()
{
	l_fPlayScale = 2.0f;
	l_bPlaying = true;
	l_bPlayingForward = false;
	confirm_current_time();
}
void plbkModePlayback::Play()
{
	l_bPlaying = true;
	l_bPlayingForward = true;
	l_fPlayScale = 1.0f;
	confirm_current_time();
}
void plbkModePlayback::GoEnd()
{
	l_CurrentTick = (tmlnTimeLine::GetMaximum().AsSeconds() * l_TicksPerSecond);
	set_timeline();
}
void plbkModePlayback::GoBegin()
{
	//l_CurrentTick = 0.0f;
	l_CurrentTick = (tmlnTimeLine::GetMinimum().AsSeconds() * l_TicksPerSecond);
	set_timeline();
}
void plbkModePlayback::PlayRev()
{
	l_bPlaying = true;
	l_bPlayingForward = false;
	l_fPlayScale = 1.0f;

	set_lasttime();
}

void plbkModePlayback::SetLooping( bool i_bLoopAtEnd )
{
	l_bLooping = i_bLoopAtEnd;
}

void plbkModePlayback::SetLowRes( bool i_bLowRes )
{
	l_bLowRes = i_bLowRes;
}

void plbkModePlayback::SetMute( bool i_bMute )
{
	l_bMute = i_bMute;
}

void plbkModePlayback::SetSloMo( float i_slowMotionRate )
{
	appSimTime::SetMaxTimeDelta(i_slowMotionRate);
}

void plbkModePlayback::TogglePlayAndPause()
{
	if (l_bPlaying)
	{
		Pause();
	}
	else
	{
		Play();
	}
}

//----------------------------------------------------------------------------
// If looping is off and playback reaches the end of its play, turn off mode
//----------------------------------------------------------------------------
void plbkModePlayback::CheckForEnd()
{
	maTime time = maTime::FromSeconds(l_CurrentTick * l_fSecondsPerTick);
	if (time < tmlnTimeLine::GetMinimum())
	{
		if(!l_bLooping && !l_bPlayingForward)
		{
			l_bPlaying = false;
			ExitMode();
		}
	}
	else if (time > tmlnTimeLine::GetMaximum())
	{
		if(!l_bLooping && l_bPlayingForward)
		{
			l_bPlaying = false;
			ExitMode();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void plbkModePlayback::set_app_fusion(bool i_appIsFusion)
{
	l_appIsFusion = i_appIsFusion;
}
