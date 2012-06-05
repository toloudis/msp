/****************************************************************************\
**	mainInitLibrary.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitLibrary.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "ToolUIWx/twx/twxAppUtil.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"

#include "Core/app/private/appApplicationPAC.hpp"	// FIX - if this is sorted with includes, it compiles with errors.


//--------------------------------------------------------------------
// Initializes library packages
//--------------------------------------------------------------------
mainInitLibrary::mainInitLibrary(HINSTANCE hInstance,
								 int nCmdShow)
:	m_bSuccess(false)
{	
	//
	appApplicationPAC::SetHINSTANCE(hInstance);

#ifdef USE_WXWIDGETS
	twxAppUtil::SetInstance(hInstance, nCmdShow);

	// Get command line arguments in format that wxWidgets likes
	//int argc = 0;
    //wxChar **argv = NULL;
	//twxAppUtil::ParseCommandLine(argc, argv);
	// wxWidgets can't handle our /lastfile command line arguments, so don't even 
	// bother given them the correct ones.
	m_Argc = 1;
	m_Argv[0] = new TCHAR[::strlen(mnmConstants::c_EXECUTABLE)+1];
	wcscpy(m_Argv[0], itString(mnmConstants::c_EXECUTABLE).GetString());
	m_Argv[1] = NULL;

	// Initialize wxWidgets
	if (twxAppUtil::Init(m_Argc, m_Argv))
#endif // USE_WXWIDGETS
	{
		// Initialize the debug library and path configuration
		// parts of the library
		fsLocator udsp;
		udsp.Push(mnmConstants::c_COMPANY);
		udsp.Push(mnmConstants::c_PRODUCT);
		gfPaths::SetUserDataSubPath( udsp );
		dbgPackage::Init();

		fsLocator appPath = gfPaths::GetPath(gfPaths::e_ExePath);
		mnmPaths::SetupPaths( appPath );

		// Checking whether the computer has multiple processors and
		// setting the state of the Multithreading flag based on this
		int num_cpus = envThread::hardware_concurrency();
		DBG_LOG("Number of processing units: " << num_cpus);
		bool bDoMultithreading = (num_cpus > 1);
		envThreadGroup::SetThreadingEnabled(bDoMultithreading);

		m_bSuccess = true;
	}
}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitLibrary::~mainInitLibrary()
{
	//
	dbgPackage::CleanUp(); // because we explicitly did an Init() of gf above (makes ref count match)

#ifdef USE_WXWIDGETS
	// clean up wxWidgets
	wuiPackage::DeInitialize();
	twxAppUtil::CleanUp();

	// Only have to free args if we used the twxAppUtil to parse them above
	//twxAppUtil::FreeArgs(argc, argv);
	delete [] m_Argv[0];
#endif // USE_WXWIDGETS

}

//--------------------------------------------------------------------
// Returns true if Init succeeded
//--------------------------------------------------------------------
bool mainInitLibrary::InitSuccessful() 
{ 
	return m_bSuccess; 
}
