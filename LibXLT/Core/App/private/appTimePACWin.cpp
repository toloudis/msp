/****************************************************************************\
**  appTimePACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/private/appTimePACWin.hpp"

#include <windows.h>
#include <time.h>

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace appTimePAC
{
namespace
{
	double l_CounterRate = 0.0f;
	LARGE_INTEGER l_InitTime;

	double last_time = 0.0f;

	DWORD init_ticks;
}


//--------------------------------------------------------------------
//	GetTime returns a floating point number representing the time
//	in seconds.  The 0 point for this time can be anywhere; the appTime
//	component corrects for different time conventions.
//--------------------------------------------------------------------
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
			DBG_LOG("Noting appTime time warp of " <<(float)(actual_time-last_time) << " seconds." );
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


//--------------------------------------------------------------------
//	GetDate returns the year, month and day based on the system date.
//--------------------------------------------------------------------
void GetDate(unsigned short &o_Year, unsigned short &o_Month, unsigned short &o_Day )
{
	//	One method:
	//
	//char dateStr [9];
	//char timeStr [9];
	//_strdate( dateStr);
	//printf( "The current date is %s \n", dateStr);
	//_strtime( timeStr );
	//printf( "The current time is %s \n", timeStr);

	//	Method #2:
	//
    // typedef struct _SYSTEMTIME {
    //   WORD wYear;
    //   WORD wMonth;
    //   WORD wDayOfWeek;
    //   WORD wDay;
    //   WORD wHour;
    //   WORD wMinute;
    //   WORD wSecond;
    //   WORD wMilliseconds;
    //} SYSTEMTIME;

     SYSTEMTIME st;
     GetSystemTime(&st);
     //printf("Year:%d\nMonth:%d\nDate:%d\nHour:%d\nMin:%d\nSecond:% d\n" ,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	 o_Year = st.wYear;
	 o_Month = st.wMonth;
	 o_Day = st.wDay;
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
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

