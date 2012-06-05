#include "stdafx.h"
#include "Form1.h"
#include "mnmApp.hpp"

#include <stdio.h>
#include <windows.h>
#include <memory.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

//#include "appApplication.hpp"
#include "appApplicationPAC.hpp"	//ToDo figure out a way to not include a pack
#include "AppLayer.hpp"
#include "appTime.hpp"
#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
//#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envError.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "gfFileUtil.hpp"
#include "gfPaths.hpp"
#include "gfFileX.hpp"
#include "gfTextSetFile.hpp"
#include "gfPakFile.hpp"
#include "MathLayer.hpp"
//#include "snSoundJob2D.hpp"
//#include "snSoundManager.hpp"
#include "snSoundSystem.hpp"


using namespace System::Windows::Forms;


HANDLE g_hMutexAppRunning = NULL;


namespace
{
snSoundJob2D* l_pSound;

void DoLoadAudioTests()
{
	// Start sound
	//
	fsLocator FileLoc;
	fsFileUtil::ANSIFilenameToLocator( "iamsinis.wav", FileLoc );
	l_pSound = snSoundManager::CreateSoundJob2DStatic( FileLoc, itString("SoundName") );
	l_pSound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );
	l_pSound->Load();
	l_pSound->SetDeleteWhenFinished(false);
	snSoundManager::SetupSoundJob(l_pSound);

	l_pSound->SetVolume( 1.0f );
	l_pSound->Start( appTime::GetTime() );
}

void DoTests()
{
	std::string strLogging;

	try
	{
		DoLoadAudioTests();
	}								//WARNING! you'll need to set the locFile*.txt files to be writable and move them to your exe dir
	catch (const fsReadOnlyX& roe)
	{
		fsFileUtil::LocatorToANSIFilename(roe.GetLocator(), strLogging);
		DBG_LOG1("Locator: %s, is invalid", strLogging.c_str());
	}
	catch (const fsInvalidLocatorX& ile)
	{
		fsFileUtil::LocatorToANSIFilename(ile.GetLocator(), strLogging);
		DBG_LOG1("Locator: %s, is invalid", strLogging.c_str());
	}
	catch (const fsFileDoesntExistX& fdee)
	{
		fsFileUtil::LocatorToANSIFilename(fdee.GetLocator(), strLogging);
		DBG_LOG1("file: %s, does not exist.", strLogging.c_str());
	}
	catch (const gfFileNotInPakX& fnip)
	{
		fsFileUtil::LocatorToANSIFilename(fnip.GetLocator(), strLogging);
		DBG_LOG1("file: %s, does not exist in pakfile.", strLogging.c_str());
	}
}

}


//
// Main entry point
//
int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	//
	appApplicationPAC::SetHINSTANCE(hInstance);

	Sn::Form1 * main_form = new Sn::Form1();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );
	//	initialize
	//
	BaseLayer::Init();
	AppLayer::Init();
	MathLayer::Init();

	//appApplicationPAC::CreateMainWindow(640,480,0,0,itString("Audio SN test"));

	AudioLayer::Init();

	snSoundSystem::Initialize();
	snSoundManager::Initialize();

	// needed before initialize_render()
	mnmApp *pApp = new mnmApp();
	mnmApp::SetActive( false );

	//	run the app
	//
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	//
	//	the "run" loop
	//
	mnmApp::SetActive( true );		//	set the app active right before the app runs
	Application::Run( main_form );

	//	clean-up
	//
	snSoundManager::CleanUp();
	snSoundSystem::CleanUp();

	AudioLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();

	delete pApp;

	if (g_hMutexAppRunning != NULL )
	{
		CloseHandle(g_hMutexAppRunning);
		g_hMutexAppRunning = NULL;
	}

	return 0;
}
