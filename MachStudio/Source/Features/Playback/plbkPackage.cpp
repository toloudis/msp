/*****************************************************************************
**  plbkPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Features/Playback/plbkPackage.hpp"

#include "Features/Playback/plbkModePlayback.hpp"
#include "Features/Playback/plbkPlaybackControlsDialogUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"


//============================================================================
//============================================================================
namespace plbkPackage
{
	namespace
	{	
		plbkModePlayback* l_pModePlayback = NULL;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Create the modes and add to the modeModeMgr
		l_pModePlayback = new plbkModePlayback();
		const bool bAddToMenu = true;
		modeModeID modeIDPlayback = modeModeMgr::AddMode( l_pModePlayback, bAddToMenu, "mode-playback.png", "Windows" );

		plbkPlaybackControlsDialogUtil::Init();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		plbkPlaybackControlsDialogUtil::CleanUp();

		// Let modeModeMgr destroy the modes? or needs a "RemoveMode" function
		modeModeMgr::RemoveMode(l_pModePlayback);
		delete l_pModePlayback;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	modeModeID GetModePlaybackID()
	{
		return l_pModePlayback->GetID();
	}

}	// end of namespace