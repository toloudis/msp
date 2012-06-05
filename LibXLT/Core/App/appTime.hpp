/****************************************************************************\
**  appTime.hpp
**
**      The appTime component is meant to provide the most basic time
**	service - namely that of getting a current time value.  This time value
**	is measured from the beginning of the application run.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_TIME_HPP
#error appTime.hpp multiply included
#endif
#define APP_TIME_HPP


//============================================================================
//============================================================================
namespace appTime
{
	//--------------------------------------------------------------------
	//	GetTime returns a floating point number representing the time
	//	in seconds since the beginning of the application run.
	//--------------------------------------------------------------------
	float GetTime();

	//--------------------------------------------------------------------
	//	GetDate returns the year, month and day based on the system date.
	//--------------------------------------------------------------------
	void GetDate(unsigned short &o_Year, unsigned short &o_Month, unsigned short &o_Day );

	//------------------------------------------------------------------------
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();
}

