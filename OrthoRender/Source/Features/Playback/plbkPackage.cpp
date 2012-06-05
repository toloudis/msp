/*****************************************************************************
**  plbkPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Features/Playback/plbkPackage.hpp"
#include "Features/Playback/plbkModePlayback.hpp"

#include "Support/mode/modeModeMgr.hpp"

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
		modeModeID modeIDPlayback = modeModeMgr::AddMode( l_pModePlayback, true, "mode-playback.png" );
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		// Let modeModeMgr destroy the modes? or needs a "RemoveMode" function
		modeModeMgr::RemoveMode(l_pModePlayback);
		delete l_pModePlayback;
	}

}	// end of namespace