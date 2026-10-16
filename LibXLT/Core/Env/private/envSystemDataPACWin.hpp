/********************************************************************
**  envSystemDataPACWin.hpp
**
**      envSystemDataPACWin is the declaration for the windows
**	version of the system data PAC.  It supplies, in addition to the
**	Init and CleanUp functions, a function to get the version of
**	Windows being used (at runtime).
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
	//------------------------------------------------------------------------
	//	OSVersion is the version information returned by GetOSVersion.
	//------------------------------------------------------------------------
	struct OSVersion
	{
		unsigned long	m_Major;
		unsigned long	m_Minor;
		unsigned long	m_Build;
		bool			m_bWorkstation;		// false for the server editions
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
