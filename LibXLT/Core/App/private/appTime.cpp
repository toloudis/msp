/****************************************************************************\
**  appTime.hpp
**
**      appTime.hpp supplies the basic application class.  To create
**	a Terawatt application, with a window and message handling, clients can
**	inherit an object from this class and use the protected interface
**	to implement application specific behaviors.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appTime.hpp"

#include "Core/app/private/appTimePAC.hpp"


//============================================================================
//============================================================================
namespace appTime
{
namespace 
{
	float l_BeginTime = 0;
}

//--------------------------------------------------------------------
//	GetTime returns a floating point number representing the time
//	in seconds since the beginning of the application run.
//--------------------------------------------------------------------
float GetTime()
{
	return appTimePAC::GetTime() - l_BeginTime;
}

//--------------------------------------------------------------------
//	GetDate returns the year, month and day based on the system date.
//--------------------------------------------------------------------
void GetDate(unsigned short &o_Year, unsigned short &o_Month, unsigned short &o_Day )
{
	appTimePAC::GetDate( o_Year, o_Month, o_Day );
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void Init()
{
	appTimePAC::Init();
	l_BeginTime = appTimePAC::GetTime();
}

void CleanUp() throw()
{
	appTimePAC::CleanUp();
}

}
