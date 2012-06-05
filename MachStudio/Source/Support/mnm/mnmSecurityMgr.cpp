/****************************************************************************\
**  mnmSecurityMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmSecurityMgr.hpp"

#include "Tool/gui/guiMessageBox.hpp"
#include "Core/scrty/scrtyMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	bool l_bCheckSecurity = true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
long mnmSecurityMgr::ValidateLicense(std::string& i_LicenseStr)
{
	return scrtyMgr::ValidateLicense( i_LicenseStr );
}

//------------------------------------------------------------------------
//	Check for security and set a flag
//------------------------------------------------------------------------
bool mnmSecurityMgr::CheckSecurity1()
{
	return (l_bCheckSecurity = scrtyMgr::CheckSecurity1());
}
bool mnmSecurityMgr::CheckSecurity2()
{
	return (l_bCheckSecurity = scrtyMgr::CheckSecurity2());
}
bool mnmSecurityMgr::CheckSecurity3()
{
	return (l_bCheckSecurity = scrtyMgr::CheckSecurity3());
}
bool mnmSecurityMgr::CheckSecurity4()
{
	return (l_bCheckSecurity = scrtyMgr::CheckSecurity4());
}
bool mnmSecurityMgr::CheckSecurity5()
{
	return (l_bCheckSecurity = scrtyMgr::CheckSecurity5());
}

//------------------------------------------------------------------------
//	Pop-up a dialog to the user and returns false if security isn't there.
//------------------------------------------------------------------------
void mnmSecurityMgr::AnnounceNoSecurity()
{
	guiMessageBox::Show("License missing or invalid", "Critical Error", guiMessageBox::e_OKOnly);
}

