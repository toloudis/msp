/****************************************************************************\
**  appTimePACWin.hpp
**
**	appTimePACWin.hpp defines the windows PAC for the appTime.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_TIMEPACWIN_HPP
#error appTimePACWin.hpp multiply included
#endif
#define APP_TIMEPACWIN_HPP


//============================================================================
//============================================================================
namespace appTimePAC
{
	//--------------------------------------------------------------------
	//	GetTime returns a floating point number representing the time
	//	in seconds.  The 0 point for this time can be anywhere; the appTime
	//	component corrects for different time conventions.
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
};


