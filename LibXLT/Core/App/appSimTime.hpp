/*****************************************************************************
**  appSimTime.hpp
**
**      appSimTime maintains the simulation time for the app, which
**	is not the same as the real time - for instance, the game may be paused.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef	APP_SIMTIME_HPP
#error	appSimTime.hpp included recursively
#endif
#define	APP_SIMTIME_HPP


//============================================================================
//============================================================================
namespace appSimTime
{

//----------------------------------------------------------------------------
//	GetTime returns the current simulation time
//----------------------------------------------------------------------------
float GetTime();

//----------------------------------------------------------------------------
//	GetDelta returns the difference between the current time and the
//	last (simulation) time.
//----------------------------------------------------------------------------
float GetDelta();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void IncrementTime(float i_Factor = 1.0f);

//------------------------------------------------------------------------
//	Init() and CleanUp() should be called before and after the component
//	is being used.
//------------------------------------------------------------------------
void Init();
void CleanUp() throw();

//------------------------------------------------------------------------
//	ResetTime() and SetTime() should be called ONLY if you want to manage
//	time yourself.
//------------------------------------------------------------------------
void ResetTime( float i_InitialTime = 0.0f );
void SetTime( float i_TimeCurrent, float i_TimeInc );

//----------------------------------------------------------------------------
//	SetMaxTimeDelta sets the maximum time delta which will be used for a 
//	frame.  This is useful if the game gets really slow for a second or
//	is being debugged, and you don't want simulation time to go forward
//	very much.
//----------------------------------------------------------------------------
void SetMaxTimeDelta(float i_Time);

//----------------------------------------------------------------------------
// GetPreciseTime is used to retrieve the exact simulation time based on the 
// current time.  This calculates the sim time as if IncrementTime was called,
// but the internal values are not modified.  That is, GetTime will still return
// the time set by IncrementTime.
// This is primarily used in multiplayer simulations and time syncs where the exact
// simulation time is required for accurate resolution of conflicts and players.
//----------------------------------------------------------------------------
float GetPreciseTime( float i_Factor=1.0f);


}
