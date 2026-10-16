/**********************************************************
**  envSystemDataPACWin.cpp
**
**      envSystemDataPACWin is the definition for the windows
**	version of the system data PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include <windows.h>

#include "Core/env/private/envSystemDataPACWin.hpp"

#include "Core/env/envInitX.hpp"

namespace envSystemDataPAC
{

namespace
{

WindowsOS l_CurWindowsVersion = e_Unknown;

}


//========================================================================
//	As you might expect, this function finds which version of windows
//	is being used.  The version is cached so it's reasonably fast.
//========================================================================
WindowsOS GetWindowsOSType()
{
	return l_CurWindowsVersion;
}

//========================================================================
//	GetOSVersion fills in the real version of Windows that is running.
//	GetVersionEx is deprecated and reports Windows 8 to applications that
//	are not manifested for later versions, so this asks ntdll's
//	RtlGetVersion instead.
//========================================================================
bool GetOSVersion(OSVersion& o_Version)
{
	typedef LONG (WINAPI* RtlGetVersionFn)(OSVERSIONINFOEXW*);

	HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
	RtlGetVersionFn rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersionFn>(::GetProcAddress(ntdll, "RtlGetVersion")) : NULL;
	if ( !rtlGetVersion )
		return false;

	OSVERSIONINFOEXW info;
	ZeroMemory(&info, sizeof(info));
	info.dwOSVersionInfoSize = sizeof(info);
	if ( rtlGetVersion(&info) != 0 )
		return false;

	o_Version.m_Major = info.dwMajorVersion;
	o_Version.m_Minor = info.dwMinorVersion;
	o_Version.m_Build = info.dwBuildNumber;
	o_Version.m_bWorkstation = (info.wProductType == VER_NT_WORKSTATION);
	o_Version.m_ServicePack = info.szCSDVersion;
	return true;
}

//========================================================================
//	GetExecutablePath returns the full path of the running executable.
//========================================================================
std::wstring GetExecutablePath()
{
	wchar_t exepath[MAX_PATH];
	DWORD length = ::GetModuleFileNameW(NULL, exepath, MAX_PATH);
	return std::wstring(exepath, length);
}

//========================================================================
//	Don't call Init() yourself; it is called by the package Init().
//========================================================================
void Init()
{
	// Deduce os version.
	OSVersion version;
	if ( !GetOSVersion(version) )
		throw envInitX("env");

	if ( version.m_Major <= 4 )
		l_CurWindowsVersion = e_WindowsNT40;
	else if ( version.m_Major == 5 )
		l_CurWindowsVersion = e_Windows2000;
	else if ( version.m_Major == 6 )
		l_CurWindowsVersion = e_WindowsXP;		// windows7 shows up as 6 also
	else
		l_CurWindowsVersion = e_Windows7;
}

//========================================================================
//	Don't call CleanUp() yourself; it is called by the package CleanUp().
//========================================================================
void CleanUp() throw()
{
}

}
