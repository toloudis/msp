/****************************************************************************\
**  mnmSecurityMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmSecurityMgr.hpp"

#include "Tool/gui/guiMessageBox.hpp"
#include "SecurityMatrix/scrty/scrtyMgr.hpp"

//
namespace
{
	bool l_bDongleValid = true;
}

//------------------------------------------------------------------------
//	Check for dongle and set a flag
//------------------------------------------------------------------------
bool mnmSecurityMgr::CheckForDongle()
{
	return (l_bDongleValid = scrtyMgr::DongleValid());
}

//------------------------------------------------------------------------
//	Pop-up a dialog to the user and returns false if dongle isn't there.
//------------------------------------------------------------------------
void mnmSecurityMgr::AnnounceIfNoDongle()
{
	guiMessageBox::Show("Dongle missing or invalid", "Critical Error", guiMessageBox::e_OKOnly);
}
