/********************************************************************
**  appInstanceUtilPACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/
#include "Core/app/private/appInstanceUtilPACWin.hpp"

#include "Core/app/private/appApplicationPACWin.hpp"

#include <string>
#include <windows.h>


//============================================================================
//============================================================================
namespace appInstanceUtilPAC
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
namespace
{
HANDLE l_Mutex = NULL;
}


//------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------
void Init()
{
	// right now IsAppRunning is called and creates the l_Mutex.  
	// There is probably a better way to do this.
	// Call CleanUp.
}

//------------------------------------------------------------------------
// CleanUp -- call as your program is terminating
//------------------------------------------------------------------------
void CleanUp()
{
	if ( NULL != l_Mutex )
	{
		::CloseHandle(l_Mutex);
		l_Mutex = NULL;
	}
}

//------------------------------------------------------------------------
//	Returns true if another application with the same i_IDString is
//	running.
//------------------------------------------------------------------------
bool IsAppRunning(const char* i_IDString)
{
	std::string mutex_name = "Global\\";
	mutex_name += i_IDString;

	l_Mutex = ::CreateMutexA(NULL, FALSE, mutex_name.c_str());

	if( (l_Mutex != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS) )
	{
		::CloseHandle(l_Mutex);
		l_Mutex = NULL;
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
//	HandleAppRunning tests to see if another version of the application
//	is running, and if so brings it to the foreground.  It returns
//	true if this was done, and application should exit.
//------------------------------------------------------------------------
bool HandleAppRunning(const char* i_IDString, const char* i_WindowName)
{
	if( IsAppRunning(i_IDString) )
	{
		HWND my_window = NULL;

		itString class_name(appApplicationPAC::GetWindowClassName());
		itString window_name(i_WindowName);
		my_window = ::FindWindowW(class_name.GetString(), window_name.GetString());

		if( my_window != NULL )
		{
			DBG_LOG("Switching to existing application");
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
}

}
