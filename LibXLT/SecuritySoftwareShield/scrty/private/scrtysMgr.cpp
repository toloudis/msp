/*****************************************************************************
**	scrtysMgr.cpp
**
**		see .hpp
**
**	this project includes the following libs:
**		libhasp_cpp_windows_mtd_msc8.lib libhasp_windows_87761.lib
**		One comes from HASP, the other gets built based on our corporate
**		ID given to us by HASP.
**
**	the define USE_SECURITYSYSTEM is set in the gold build of this library only.
**	
**	The main idea behind the security system is to not make it easy to debug.
**	When an dongle is looked for and not detected, a flag is set and the error
**	shows up elsewhere in the program making it harder to track down.
**	
**	
**		From Aladdin
**	
**	The Best Practices for protecting your application are listed in the HASP SRM Guide, chapter 6. http://aladdin.com/hs#docs
**	
**	Some tips off-hand would be the following:
**	
**	1) Use hasp_encrypt()/hasp_decrypt() for encrypting application variables to make sure the key is connected, since it uses the AES key stored on the HASP key.
**	2) Using hasp_get_info() doesn't log into the key, but will check for if the key is present, and also pull additional information about the key, license, etc. 
**	
**	(ftp://ftp.aladdin.com/pub/hasp/srm/Documentation/HASP_SRM_Guide.pdf) chapter 5 + 6
**	
**	Vary Behavior when Cracking Attempt is Detected
**	
**	When a cracking attempt is detected (for example, through using a
**	checksum—described later in the chapter), delay the reactive behavior
**	of your software, thus breaking the logical connection between
**	“cause” and “effect”. Delayed reaction confuses a software cracker by
**	obscuring the link between the cracking attempt and the negative
**	reaction of the software to that attempt.
**	Behavior such as impairing program functionality when a cracking
**	attempt is detected can be very effective. Additional behaviors could
**	include causing the program to crash, overwriting data files, or
**	deliberately causing the program to become inaccurate, causing the
**	program to become undependable.
**	
**	Insert Multiple Calls in your Code
**	
**	Inserting many calls, throughout the code, to the HASP SRM
**	protection key in order to check the presence of the key, and binding
**	data from the key with the software functionality, frustrates those
**	attempting to crack your software. Multiple calls increase the
**	difficulty in tracing a protection scheme.
**	You can also add obstacles to a potential software cracker’s progress
**	by encrypting data that has no bearing on the application. Similarly,
**	you can divert attention by generating “noise” through random
**	number generators, time values, intermediate results of calculations,
**	and other mechanisms that do not lead to meaningful results or
**	actions.
**	
**	Encrypt/Decrypt Data with a HASP SRM Protection Key
**	
**	Encryption and decryption processes are performed inside a
**	HASP SRM protection key, well beyond the reach of any debugging
**	utility.
**	Encrypting data with the HASP SRM AES based encryption engine
**	considerably enhances software security. By encrypting data used by
**	your application, the decryption process depends on both the
**	presence of a HASP SRM protection key and its internal intelligence.
**	By implementing a HASP SRM Runtime API scheme in which data is
**	decrypted by a HASP SRM protection key, the association between
**	the protected application and the HASP SRM protection key cannot
**	easily be removed. Cracking the software also necessitates the
**	software cracker decrypting the data.
**	Use a Checksum to Verify Integrity of Executable Files
**	Compare the value in the executable file with a checksum stored in
**	HASP SRM protection key memory. If the two values are not equal,
**	you can assume that someone has attempted to modify the files.
**	Repeat this check in various places in the code, varying it in each place
**	to make it more difficult for a software cracker to detect.
**	
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "SecuritySoftwareShield/scrty/scrtysMgr.hpp"

#include "Core/app/appTimeUtils.hpp"
#include "Core/scrty/scrtySecurityX.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

//#define USE_SECURITYSYSTEM
#ifdef USE_SECURITYSYSTEM
#ifdef USE_WXWIDGETS
#include "SecuritySoftwareShield/scrty/private/wxGUI/ProductActivationDialog.hpp"
#endif
#endif

#include <time.h>


//============================================================================
//	library pragmas and imports
//============================================================================
#ifdef USE_SECURITYSYSTEM
//#pragma comment(lib,"libhasp_windows_87761.lib")
#import "Core/Scrty/private/SSCProt.dll" no_namespace
#endif


//============================================================================
//============================================================================
namespace
{
	//	access and usage types
	const int lc_ACCESSTYPE_FULL	= 0;
	const int lc_ACCESSTYPE_LIMITED = 1;
	const int lc_USAGETYPE_FULL		= 0;
	const int lc_USAGETYPE_DAYS		= 1;
	const int lc_USAGETYPE_COUNT	= 2;
	const int lc_USAGETYPE_MINUTES	= 3;

#ifdef USE_SECURITYSYSTEM
	ISSCProtectorPtr spSSCProt;

//#ifdef USE_WXWIDGETS
#ifndef BATCH_MODE
	//	MachStudio Pro
	char * l_MainLicenseFileName = {"LicMachStudio.ini"};
	char * l_MainLicenseFilePassword = {"flAtironS_VENTRALS_SPROCKETS"};
	char * l_GlobalAuthorizationCodePassword = {"piratIcal___twinInG_`_fiefs"};
	int l_FingerPrintOptionsCode = 503332864;
#else
	//	MachStudio Render
	char * l_MainLicenseFileName = {"MachStudio Render.ini"};
	char * l_MainLicenseFilePassword = {"theorem_FREAKIER_1_fieRce"};
	char * l_GlobalAuthorizationCodePassword = {"pushiness_SUEDE_GRUFFLY"};
	int l_FingerPrintOptionsCode = 503332864;
#endif
#endif


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BSTR RequestSerialNumber()
{
#ifdef USE_SECURITYSYSTEM
#ifdef USE_WXWIDGETS	// this one doesn't work here
#ifndef BATCH_MODE
	DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
	ProductActivationDialog dialog( twxSystem::g_pMainForm );
	dialog.ShowModal();
	dialog.Hide();
	return dialog.GetSerial();
#endif
#endif
#endif
return BSTR();
}

};


//------------------------------------------------------------------------
//------------------------------------------------------------------------
scrtysMgr::scrtysMgr()
:	m_bSecurityValid(false)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtysMgr::Init()
{
#ifdef USE_SECURITYSYSTEM
	CoInitialize(NULL);		//Initialize COM system

	// lets make sure the ClientProtector COM server is still registered on this machine.
	HRESULT hr = spSSCProt.CreateInstance(_T("SSCProt.SSCprotector"));
	//IDispatch* pdisp = NULL;
	//CLSID cid;
	//ZeroMemory(&cid, sizeof(CLSID));
	//CLSIDFromProgID(L"SSCProt.SSCprotector", &cid);
	//HRESULT hr = CoCreateInstance(cid, NULL, CLSCTX_ACTIVATE_32_BIT_SERVER, IID_IDispatch, (void**)&pdisp);
	if (!SUCCEEDED(hr))
	{
		// if its not - inform the user of the problem and shut down.
		//AfxWin.h
		//AfxMessageBox("Unable to connect to the license server. Please contact your vendor.");
		DBG_ERROR("Can't connect to license server");
		PostQuitMessage(0);
		return;
	}
#endif
	return;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtysMgr::CleanUp()
{
#ifdef USE_SECURITYSYSTEM
	m_bSecurityValid = false;
#endif
	return;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
long scrtysMgr::ValidateLicense(BSTR& i_LicenseStr)
{
	long return_code = 0;
#ifdef USE_SECURITYSYSTEM
	long AutoActivateReturn;

	//Attempts to Activate the Serial Number
	return_code = spSSCProt->AutoActivateSerialNumber(i_LicenseStr, &AutoActivateReturn);
	if (AutoActivateReturn == TRUE_NON_EXPIRY)
	{
		// run the software
		m_bSecurityValid = true;
	}
	else
	{
		// Quit
		//DBG_ERROR("Invalid serial number - product not activated.");
	}
#endif
	return return_code;
}


//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtysMgr::CheckSecurity1()
{
#ifdef USE_SECURITYSYSTEM

	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtysMgr::CheckSecurity2()
{
#ifdef USE_SECURITYSYSTEM
	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtysMgr::CheckSecurity3()
{
#ifdef USE_SECURITYSYSTEM
	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtysMgr::CheckSecurity4()
{
#ifdef USE_SECURITYSYSTEM
	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtysMgr::CheckSecurity5()
{
#ifdef USE_SECURITYSYSTEM
	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	Call ONCE on launch of program
//------------------------------------------------------------------------
bool scrtysMgr::OpenSecurity()
{
#ifdef USE_SECURITYSYSTEM
	long return_code;
	long debugFlags = DBG_ALL_DETAIL;
	spSSCProt->StartUp(l_MainLicenseFileName, l_MainLicenseFilePassword, l_GlobalAuthorizationCodePassword, 
						l_FingerPrintOptionsCode, debugFlags, &return_code);

	if (return_code == TRUE_NON_EXPIRY)
	{
		// run the software
		return true;
	}
	else if (return_code == TRUE_EXPIRY)
	{
		// pop up nag screen if you want to 
		m_bSecurityValid = false;
	}
	else
	{
		BSTR SerialNumber;
		SerialNumber = RequestSerialNumber(); // Request serial number, possibly via dialog.

		return_code = ValidateLicense( SerialNumber );
		if (return_code != TRUE_NON_EXPIRY)
		{
			m_bSecurityValid = false;
		}
	}

#ifdef _DEBUG
	scrtysMgr::TestSecurity();		// for debug only
#endif

	return m_bSecurityValid;
#endif
	return true;
}

//------------------------------------------------------------------------
//	Call ONCE on exit of program
//------------------------------------------------------------------------
void scrtysMgr::CloseSecurity()
{
#ifdef USE_SECURITYSYSTEM
#endif
}

//------------------------------------------------------------------------
//	for testing only
//------------------------------------------------------------------------
void scrtysMgr::TestSecurity()
{
#ifdef USE_SECURITYSYSTEM
#endif
}

