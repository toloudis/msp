/****************************************************************************\
**  snSoundManagerPACWin.cpp
**
**  snSoundManagerPACWin.cpp implements the DirectSound portion of the
**	snSoundManagerUtilPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/private/snSoundManagerPACWin.hpp"
#include "AudioDS/sn/snSoundManager.hpp"
#include "AudioDS/sn/snSoundSystem.hpp"
#include "Core/app/appFlowEventHandler.hpp"

#include "windows.h"


namespace snSoundManagerPAC
{
//----------------------------------------------------------------------------
//	snReloader
//
//	handles the unloading and reloading of the sound data when focus is lost.
//----------------------------------------------------------------------------
namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
class snReloader : public appFlowEventHandler
{
	public:

		//====================================================================
		//	Override this function to get appStartEvents.
		//====================================================================
		virtual void ReceiveStartEvent(appStartEvent& i_Event);

		//====================================================================
		//	Override this function to get appStopEvents.
		//====================================================================
		virtual void ReceiveStopEvent(appStopEvent& i_Event);

		//====================================================================
		//	Override this function to get appSuspendEvents.
		//====================================================================
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);

		//====================================================================
		//	Override this function to get appResumeEvents.
		//====================================================================
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);
};


//====================================================================
//====================================================================
void snReloader::ReceiveStartEvent(appStartEvent& i_Event)
{

}

//====================================================================
//====================================================================
void snReloader::ReceiveStopEvent(appStopEvent& i_Event)
{
}

//====================================================================
//====================================================================
void snReloader::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	snSoundManager::UnloadSounds();
	snSoundSystem::DeInitialize();
}

//====================================================================
//====================================================================
void snReloader::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	snSoundSystem::Initialize();
	snSoundManager::ReloadSounds();
}

}


//========================================================================
//========================================================================
snReloader* l_Reloader = NULL;

//========================================================================
//	Think()
//
//	Update for Windows.
//========================================================================
void	
Think( float i_SimulationTime )
{
}

//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
Init()
{
#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		// reloader
		l_Reloader = new snReloader;
	#elif (SN_SOUNDSYSTEM == SN_MILES)
	#else
	#endif
#else
#endif
}


//========================================================================
//	CleanUp()
//========================================================================
void 
CleanUp() throw ()
{
#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		// reloader
		delete l_Reloader;
	#elif (SN_SOUNDSYSTEM == SN_MILES)
	#else
	#endif
#else
#endif
}
}

