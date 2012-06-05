/********************************************************************
**  appInstanceUtilXbox.hpp
**
**      appInstanceUtilXbox contains functions for detecting and
**	dealing with previously running instances of the application.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#ifdef APP_INSTANCEUTILXBOX_HPP
#error appInstanceUtilXbox.hpp multiply included
#endif
#define APP_INSTANCEUTILXBOX_HPP

namespace appInstanceUtilWin
{

//========================================================================
// Init
//========================================================================
void Init();

//========================================================================
// CleanUp()
//========================================================================
void CleanUp();

//========================================================================
//	Returns true if another application with the same i_IDString is
//	running.
//========================================================================
bool IsAppRunning(const char* i_IDString);

//========================================================================
//	HandleAppRunning tests to see if another version of the application
//	is running, and if so brings it to the foreground.  It returns
//	true if this was done, and application should exit.
//========================================================================
bool HandleAppRunning(const char* i_IDString, const char* i_WindowName);

}
