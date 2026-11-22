/********************************************************************
**  appInstanceUtilPAC.hpp
**
**      appInstanceUtilPAC forwards the application instance utility
**	calls to the correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#ifdef APP_INSTANCEUTILPAC_HPP
#error appInstanceUtilPAC.hpp multiply included
#endif
#define APP_INSTANCEUTILPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//	Here is the interface that must be supported by the PAC component:
//
//namespace appInstanceUtilPAC
//{
//	void Init();
//	void CleanUp();
//	bool IsAppRunning(const char* i_IDString);
//	bool HandleAppRunning(const char* i_IDString, const char* i_WindowName);
//}
//============================================================================

#if ENV_WINDOWS
	#include "Core/app/private/appInstanceUtilPACWin.hpp"
#else
	#error appInstanceUtilPAC not defined for this platform
#endif
