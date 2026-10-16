/********************************************************************
**  envSystemDataPACWin.hpp
**
**      envSystemDataPACWin is the declaration for the windows
**	version of the system data PAC.  It supplies, in addition to the 
**	Init and CleanUp functions, a function to detect which Windows
**	OS is being used (at runtime).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#ifdef ENV_SYSTEMDATAPACWIN_HPP
#error envSystemDataPACWin.hpp multiply included
#endif
#define ENV_SYSTEMDATAPACWIN_HPP

#include <string>


//============================================================================
//============================================================================
namespace envSystemDataPAC
{
	enum WindowsOS
	{
		e_WindowsNT40,
		e_Windows2000,
		e_WindowsCE,
		e_WindowsXP,
		e_WindowsVista,
		e_Windows7,
		e_Unknown
	};

	//------------------------------------------------------------------------
	//	As you might expect, this function finds which version of windows
	//	is being used.  The version is cached so it's reasonably fast.
	//------------------------------------------------------------------------
	WindowsOS GetWindowsOSType();

	//------------------------------------------------------------------------
	//	OSVersion is the version information returned by GetOSVersion.
	//------------------------------------------------------------------------
	struct OSVersion
	{
		unsigned long	m_Major;
		unsigned long	m_Minor;
		unsigned long	m_Build;
		bool			m_bWorkstation;		// false for the server editions
		std::wstring	m_ServicePack;		// empty if none is installed
	};

	//------------------------------------------------------------------------
	//	GetOSVersion fills in the real version of Windows that is running.
	//	It returns false if the version could not be determined.
	//------------------------------------------------------------------------
	bool GetOSVersion(OSVersion& o_Version);

	//------------------------------------------------------------------------
	//	GetExecutablePath returns the full path of the running executable.
	//------------------------------------------------------------------------
	std::wstring GetExecutablePath();

	//------------------------------------------------------------------------
	//	Don't call Init() yourself; it is called by the package Init().
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//	Don't call CleanUp() yourself; it is called by the package CleanUp().
	//------------------------------------------------------------------------
	void CleanUp() throw();
}
