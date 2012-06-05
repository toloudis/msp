#include <stdio.h>
#include <windows.h>
#include <memory.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "appApplication.hpp"
#include "appApplicationPAC.hpp"	//ToDo figure out a way to not include a pack
#include "AppLayer.hpp"
#include "appTime.hpp"
#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
//#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envError.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "gfFileUtil.hpp"
#include "gfPaths.hpp"
#include "gfFileX.hpp"
#include "gfTextSetFile.hpp"
#include "gfPakFile.hpp"
#include "MathLayer.hpp"
#include "snSoundJob2D.hpp"
#include "snSoundManager.hpp"
#include "snSoundSystem.hpp"


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


//	main function
//
void main()
{
	//	initialize
	//
	BaseLayer::Init();
	AppLayer::Init();
	MathLayer::Init();

	appApplicationPAC::CreateMainWindow(640,480,0,0,itString("Audio SN test"));

	AudioLayer::Init();

	snSoundSystem::Initialize();
	snSoundManager::Initialize();

	//	do the tests
	//
	DoTests();

	float theTime;
	float theTimeElapsed;
	float theTimeStart;

	theTimeStart = appTime::GetTime();
	do
	{
		theTime = appTime::GetTime();
		theTimeElapsed = theTime - theTimeStart;

		snSoundManager::Think( theTime );

	} while (theTimeElapsed <= l_pSound->GetTimeLength());

	//	clean-up
	//
	snSoundManager::CleanUp();
	snSoundSystem::CleanUp();

	AudioLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();
}
