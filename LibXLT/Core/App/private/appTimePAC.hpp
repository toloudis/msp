/****************************************************************************\
**  appTimePAC.hpp
**
**      appTimePAC.hpp forwards calls from the appTime component to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_TIMEPAC_HPP
#error appTimePAC.hpp multiply included
#endif
#define APP_TIMEPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*namespace appTimePAC
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
}*/

#if ENV_WINDOWS
	#include "Core/app/private/appTimePACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "Core/app/private/appTimePACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "Core/app/private/appTimePACXbox.hpp"
		#else
			#error appTimePAC not defined for this platform
		#endif
	#endif
#endif
