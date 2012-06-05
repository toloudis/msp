/****************************************************************************\
**  appTimePACPS2.hpp
**
**      appTimePACPS2.hpp defines the PS2 PAC for the
**	appTime.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_TIMEPACPS2_HPP
#error appTimePACPS2.hpp multiply included
#endif
#define APP_TIMEPACPS2_HPP

namespace appTimePAC
{
	//====================================================================
	//	GetTime returns a floating point number representing the time
	//	in seconds.  The 0 point for this time can be anywhere; the appTime
	//	component corrects for different time conventions.
	//====================================================================
	float GetTime();

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void Init();
	void CleanUp();
}


