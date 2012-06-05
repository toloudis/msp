/****************************************************************************\
**  appTimePACXbox.cpp
**
**      appTimePACXbox.cpp defines the appTime component
**	PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "appTimePACXbox.hpp"

//#include <windows.h>
#include <xtl.h>
#include <time.h>

#include "dbgAssert.hpp"
#include "dbgLog.hpp"

namespace appTimePAC
{

namespace
{

double l_CounterRate = 0.0f;
LARGE_INTEGER l_InitTime;

double last_time = 0.0f;

DWORD init_ticks;

}

//====================================================================
//	GetTime returns a floating point number representing the time
//	in seconds.  The 0 point for this time can be anywhere; the appTime
//	component corrects for different time conventions.
//====================================================================
float GetTime()
{
	double double_time;
	LARGE_INTEGER int_time;
	::QueryPerformanceCounter(&int_time);
	double_time = double( int_time.QuadPart - l_InitTime.QuadPart );
	double_time /= l_CounterRate;

	if( (double_time - last_time) > 0.5f )
	{
		//	time skip; must compensate using the lo-res GetTickCount
		//	QueryPerformanceCounter skips several seconds sometimes
		//	see http://support.microsoft.com/support/kb/articles/Q274/3/23.ASP
		//	PRB: Performance Counter Value May Unexpectedly Leap Forward
		//	Article: Q274323
		double actual_time = double( ::GetTickCount() - init_ticks ) / 1000;
		// we don't want to go back in time, so if we would, just figure on 
		// not moving forward at all this loop.

		// correct the actual_time if it would time warp us
		if ( actual_time < last_time )
		{
			DBG_LOG1("Noting appTime time warp of %f seconds.", (float)(actual_time-last_time) );
			// this will correct on next pass
			actual_time = last_time;
		}

		// This will advance the Init time by the perf counter jump discrepency, 
		// this will get us close to something accurate for the next pass
		double time_error = double_time - actual_time;
		l_InitTime.QuadPart += __int64(l_CounterRate * time_error);

		//	correct the return value
		double_time = actual_time;
	}

	last_time = double_time;

	return float(double_time);
}


//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
	//
	LARGE_INTEGER frequency;
	::QueryPerformanceFrequency(&frequency);
	l_CounterRate = double(frequency.QuadPart);
	::QueryPerformanceCounter(&l_InitTime);

	last_time = 0.0f;
	init_ticks = ::GetTickCount();
}

void CleanUp() throw()
{
}


}

