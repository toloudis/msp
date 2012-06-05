/*****************************************************************************
**  appSimTime.cpp
**
**      appSimTime maintains the simulation time for the app, which
**	is not the same as the real time - for instance, the game may be paused.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appSimTime.hpp"

#include "Core/app/appTime.hpp"


//============================================================================
//============================================================================
namespace appSimTime
{

namespace
{
float l_Time;
float l_LastRealTime;
float l_Delta;
float l_MaxDelta = 1.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float GetTime()
{
	return l_Time;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void IncrementTime(float i_Factor)
{
	float new_real_time = appTime::GetTime();
	float diff = new_real_time - l_LastRealTime;
	l_LastRealTime = new_real_time;
	l_Delta = diff * i_Factor;

	if( l_Delta > l_MaxDelta )
	{
		l_Delta = l_MaxDelta;
	}

	l_Time += l_Delta;
}

//----------------------------------------------------------------------------
//	GetDelta returns the difference between the current time and the
//	last (simulation) time.
//----------------------------------------------------------------------------
float GetDelta()
{
	return l_Delta;
}

//------------------------------------------------------------------------
//	Init() and CleanUp() should be called before and after the component
//	is being used.
//------------------------------------------------------------------------
void Init()
{
	l_LastRealTime = appTime::GetTime();
	l_Time = 0;
	l_Delta = 0;
}

void CleanUp() throw()
{
}


//------------------------------------------------------------------------
//	ResetTime() and SetTime() should be called ONLY if you want to manage
//	time yourself.
//------------------------------------------------------------------------
void 
ResetTime( float i_InitialTime/*=0.0f*/ )
{
	l_LastRealTime  = appTime::GetTime();
	l_Time			= i_InitialTime;
	l_Delta			= 0;
}

void 
SetTime( float i_TimeCurrent, float i_TimeInc )
{
	l_LastRealTime	= i_TimeCurrent;
	l_Time			= i_TimeCurrent + i_TimeInc;
	l_Delta			= l_Time - l_LastRealTime;
}

//----------------------------------------------------------------------------
//	SetMaxTimeDelta sets the maximum time delta which will be used for a 
//	frame.  This is useful if the game gets really slow for a second or
//	is being debugged, and you don't want simulation time to go forward
//	very much.
//----------------------------------------------------------------------------
void SetMaxTimeDelta(float i_Time)
{
	l_MaxDelta = i_Time;
}

//----------------------------------------------------------------------------
// GetPreciseTime is used to retrieve the exact simulation time based on the 
// current time.  This calculates the sim time as if IncrementTime was called,
// but the internal values are not modified.  That is, GetTime will still return
// the time set by IncrementTime.
// This is primarily used in multiplayer simulations and time syncs where the exact
// simulation time is required for accurate resolution of conflicts and players.
//----------------------------------------------------------------------------
float GetPreciseTime( float i_Factor/*=1.0f*/)
{
	float new_real_time = appTime::GetTime();
	float diff = new_real_time - l_LastRealTime;
	float delta = diff * i_Factor;

	if( delta > l_MaxDelta )
	{
		delta = l_MaxDelta;
	}
	return (l_Time + delta);
}

}
