/****************************************************************************\
**	mainInitSecurity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitSecurity.hpp"

#include "MainApp/mnmApp.hpp"

//#define USE_SOFTWARESHIELD
#ifdef USE_SOFTWARESHIELD
#include "SecuritySoftwareShield/scrty/scrtysPackage.hpp"
#define USE_SECURITY
#endif
//#define USE_ALADDIN_SECURITY
#ifdef USE_ALADDIN_SECURITY
#include "SecurityAladdin/scrty/scrtyaPackage.hpp"
#define USE_SECURITY
#endif
#ifdef USE_MATRIX_SECURITY
#include "SecurityMatrix/scrty/scrtymPackage.hpp"
#define USE_SECURITY
#endif

#ifdef USE_SECURITY
#include "Core/scrty/scrtySecurityX.hpp"
#include "Core/scrty/scrtyMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#endif


//--------------------------------------------------------------------
// Initializes application packages
//--------------------------------------------------------------------
mainInitSecurity::mainInitSecurity()
{
	#ifdef USE_SECURITY
	//	Check the security
	//
	try
	{
		#ifdef USE_ALADDIN_SECURITY
			scrtyaPackage::Initialize();
		#endif
		#ifdef USE_SOFTWARESHIELD
			scrtysPackage::Initialize();
		#endif
		#ifdef USE_MATRIX_SECURITY
			scrtymPackage::Initialize();
		#endif
	}
	catch (scrtyInvalidLicenseX)
	{
		//guiMessageBox::Show("License missing or invalid", "Critical Error", guiMessageBox::e_OKOnly);

		//	This is on initial license check, don't bother continuing.
		//m_bSuccessInit = false;
		mnmApp::SetActive( false );
	}
	catch (scrtyAPIFailedX)
	{
		//guiMessageBox::Show("Security API failed", "Critical Error", guiMessageBox::e_OKOnly);

		//	This is on initialization.  If this fails, don't bother continuing.
		//m_bSuccessInit = false;
		mnmApp::SetActive( false );
	}
	catch ( const envExceptionX& i_Ex )
	{
		guiMessageBox::Show(i_Ex.GetErrorMessage().c_str(), "Critical Error", guiMessageBox::e_OKOnly);
	}

	DBG_TRACE("Security Init complete.");
#endif
}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitSecurity::~mainInitSecurity()
{
	#ifdef USE_SECURITY
	#ifdef USE_ALADDIN_SECURITY
		scrtyaPackage::DeInitialize();
	#endif
	#ifdef USE_SOFTWARESHIELD
		scrtysPackage::DeInitialize();
	#endif
	#ifdef USE_MATRIX_SECURITY
		scrtymPackage::DeInitialize();
	#endif
	#endif
}


//--------------------------------------------------------------------
//	Check if there is a valid license (once during pre-app run)
//
//	TODO - do not report the problem immediately.
//--------------------------------------------------------------------
bool mainInitSecurity::ValidateLicense()
{
	bool bValid = true;

	#ifdef USE_SECURITY
	//	Check the security
	//
	try
	{
		if (!scrtyMgr::OpenSecurity())
		{
			guiMessageBox::Show("License invalid", "Critical Error", guiMessageBox::e_OKOnly);
			throw scrtyInvalidLicenseX();
		}
	}
	catch (scrtyInvalidLicenseX)
	{
		//guiMessageBox::Show("License missing or invalid", "Critical Error", guiMessageBox::e_OKOnly);

		//	This is on initial license check, don't bother continuing.
		//m_bSuccessInit = false;
		mnmApp::SetActive( false );
		bValid = false;
	}
	catch (scrtyAPIFailedX)
	{
		//guiMessageBox::Show("Security API failed", "Critical Error", guiMessageBox::e_OKOnly);

		//	This is on initialization.  If this fails, don't bother continuing.
		//m_bSuccessInit = false;
		mnmApp::SetActive( false );
		bValid = false;
	}
	catch ( const envExceptionX& i_Ex )
	{
		guiMessageBox::Show(i_Ex.GetErrorMessage().c_str(), "Critical Error", guiMessageBox::e_OKOnly);
	}

	DBG_TRACE("Security Init complete.");
	#endif
	return bValid;
}

