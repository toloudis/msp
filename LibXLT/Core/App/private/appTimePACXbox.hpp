/****************************************************************************\
**  appTimePACXbox.hpp
**
**      appTimePACXbox.hpp defines the Xbox PAC for the
**	appTime.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_TIMEPACXBOX_HPP
#error appTimePACXbox.hpp multiply included
#endif
#define APP_TIMEPACXBOX_HPP

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
	void CleanUp() throw();
};


