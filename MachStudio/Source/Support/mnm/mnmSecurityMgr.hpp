/****************************************************************************\
**	mnmSecurityMgr.hpp
**
**		mnmSecurityMgr provides an interface to the security system
**
**	The point of this system is to obfuscate the checking and reporting of
**	a missing security system (dongle, etc) to make it harder to crack.
**
**	Multiple checks and delayed reporting are important tools.
**
**	The code can call any of the checks.
**	The Announcing of the security fail is just popping up a dialog
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_SECURITYMGR_HPP
#error mnmSecurityMgr.hpp multiply included
#endif
#define MNM_SECURITYMGR_HPP

#include <string>


//============================================================================
//	the idea behind this security mgr is to de-couple the checking for the 
//	security and the notification of it.
//============================================================================
namespace mnmSecurityMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	long ValidateLicense(std::string& i_LicenseStr);

	//------------------------------------------------------------------------
	//	Check for security and set a flag
	//------------------------------------------------------------------------
	bool CheckSecurity1();
	bool CheckSecurity2();
	bool CheckSecurity3();
	bool CheckSecurity4();
	bool CheckSecurity5();

	//------------------------------------------------------------------------
	//	Pop-up a dialog to the user and returns false if the security isn't there.
	//------------------------------------------------------------------------
	void AnnounceNoSecurity();
};

