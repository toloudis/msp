/*****************************************************************************
**  mnpPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpPackage.hpp"

#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/ObjectManip/mnpCommands.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"

#include "Support/mode/modeModeMgr.hpp"

#include "Tool/gui/guiSingleDocHandler.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace mnpPackage
{
	//========================================================================
	//========================================================================
	namespace
	{	
		mnpModeObjectManip* l_pModeObjectManip = NULL;
		modeModeID l_ModeIDManip;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Create the modes and add to the modeModeMgr
		l_pModeObjectManip = new mnpModeObjectManip();
		const bool bAddToMenu = false;
		modeModeID modeIDObjectManip = modeModeMgr::AddMode( l_pModeObjectManip, bAddToMenu, "mode-edit.png" );
		l_ModeIDManip = modeIDObjectManip;

		// This is the default mode
		modeModeMgr::Push( modeIDObjectManip );

		//
		mnpCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		// Let modeModeMgr destroy the modes? or needs a "RemoveMode" function
		modeModeMgr::RemoveMode(l_pModeObjectManip);
		delete l_pModeObjectManip;
	}

	//--------------------------------------------------------------------
	//	Launch the current file
	//--------------------------------------------------------------------
	void LaunchFile(fsLocator& i_SceneFile)
	{
		if(cptrRenderUtil::GetCaptureProgress())
			return;
		guiSingleDocHandler::Open(i_SceneFile);

		// now launch the correct mode
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDManip );
		DBG_ASSERT( pMode != 0, "No object manip mode" );
		mnpModeObjectManip *pActualMode = dynamic_cast<mnpModeObjectManip*>(pMode);
		DBG_ASSERT( pActualMode != 0, "mode ID is not the mode" );

		modeModeMgr::Push(l_ModeIDManip);
	}

	//--------------------------------------------------------------------
	//	Launch a new, empty scene
	//--------------------------------------------------------------------
	void LaunchNew()
	{
		if(cptrRenderUtil::GetCaptureProgress())
			return;
		guiSingleDocHandler::New();

		// now launch the correct mode
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDManip );
		DBG_ASSERT( pMode != 0, "No object manip mode" );
		mnpModeObjectManip *pActualMode = dynamic_cast<mnpModeObjectManip*>(pMode);
		DBG_ASSERT( pActualMode != 0, "mode ID is not the mode" );

		modeModeMgr::Push(l_ModeIDManip);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	modeModeID GetModeObjectManipID()
	{
		return l_ModeIDManip;
		//return l_pModeObjectManip->GetID();
	}

}	// end of namespace
