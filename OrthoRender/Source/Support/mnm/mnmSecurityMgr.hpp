/****************************************************************************\
**  mnmSecurityMgr.hpp
**
**      mnmSecurityMgr provides an interface to the security system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_SECURITYMGR_HPP
#error mnmSecurityMgr.hpp multiply included
#endif
#define MNM_SECURITYMGR_HPP


//============================================================================
//	the idea behind this security mgr is to de-couple the checking for the 
//	dongle and the notification of it.  This 
//============================================================================
namespace mnmSecurityMgr
{
	//------------------------------------------------------------------------
	//	Check for dongle and set a flag
	//------------------------------------------------------------------------
	bool CheckForDongle();

	//------------------------------------------------------------------------
	//	Pop-up a dialog to the user and returns false if dongle isn't there.
	//------------------------------------------------------------------------
	void AnnounceIfNoDongle();
};

