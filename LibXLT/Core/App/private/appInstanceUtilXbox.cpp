/********************************************************************
**  appInstanceUtilXbox.cpp
**
**      appInstanceUtilXbox contains functions for detecting and
**	dealing with previously running instances of the application.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#include "appInstanceUtilXbox.hpp"

#include "appApplicationPACXbox.hpp"
#include "dbgLog.hpp"
#include "envSystemDataPACXbox.hpp"

#include <string>
//#include <windows.h>
#include <xtl.h>

namespace appInstanceUtilXbox
{

namespace
{

HANDLE l_Mutex = NULL;

}

//========================================================================
// Init
//========================================================================
void Init()
{
	// right now IsAppRunning is called and creates the l_Mutex.  
	// There is probably a better way to do this.
	// Call CleanUp.
}

//========================================================================
// CleanUp -- call as your program is terminating
//========================================================================
void CleanUp()
{
/*
	if ( NULL != l_Mutex )
	{
		::CloseHandle(l_Mutex);
		l_Mutex = NULL;
	}
*/
}

//========================================================================
//	Returns true if another application with the same i_IDString is
//	running.
//========================================================================
bool IsAppRunning(const char* i_IDString)
{
	return false;
/*
	bool global_mutex = false;
	OSVERSIONINFO version_info;
	std::string mutex_name;

	version_info.dwOSVersionInfoSize = sizeof(version_info);

	switch( envSystemDataPAC::GetWindowsOSType() )
	{
		case envSystemDataPAC::e_Windows95:
		case envSystemDataPAC::e_Windows98:
		case envSystemDataPAC::e_WindowsNT40:
		case envSystemDataPAC::e_WindowsME:
		case envSystemDataPAC::e_WindowsCE:
			global_mutex = false;
		break;

		case envSystemDataPAC::e_Windows2000:
		case envSystemDataPAC::e_WindowsXP:
			global_mutex = true;
		break;
	}

	if( global_mutex )
		mutex_name = "Global\\";

	mutex_name += i_IDString;

	l_Mutex = ::CreateMutexA(NULL, FALSE, mutex_name.c_str());

	if( (l_Mutex != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS) )
	{
		::CloseHandle(l_Mutex);
		l_Mutex = NULL;
		return true;
	}
*/
}

//========================================================================
//	HandleAppRunning tests to see if another version of the application
//	is running, and if so brings it to the foreground.  It returns
//	true if this was done, and application should exit.
//========================================================================
bool HandleAppRunning(const char* i_IDString, const char* i_WindowName)
{
  	return false;
/*
	if( IsAppRunning(i_IDString) )
	{
		HWND my_window = NULL;

		envSystemDataPAC::WindowsOS os_type = envSystemDataPAC::GetWindowsOSType();

		if (		(os_type == envSystemDataPAC::e_Windows95)
				||	(os_type == envSystemDataPAC::e_Windows98)
				||	(os_type == envSystemDataPAC::e_WindowsME) )
		{
			my_window = ::FindWindowA(appApplicationPAC::GetWindowClassName(), i_WindowName);
		}
		else
		{
			itString class_name(appApplicationPAC::GetWindowClassName());
			itString window_name(i_WindowName);
			class_name += 0;
			window_name += 0;
			my_window = ::FindWindowW(class_name.GetString(), window_name.GetString());
		}

		if( my_window != NULL )
		{
			DBG_LOG0("Switching to existing application");
			if( ::IsIconic(my_window) )
			{
				::ShowWindow(my_window, SW_RESTORE);
			}

			::SetForegroundWindow(my_window);
		}
		else
		{
			::MessageBoxA(	NULL, 
							"Another User is using this application", 
							"Application can't start",
							MB_OK);
		}

		return true;
	}
	else
		return false;
*/
}

}
