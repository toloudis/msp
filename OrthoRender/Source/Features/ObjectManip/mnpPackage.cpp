/*****************************************************************************
**  mnpPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpPackage.hpp"

#include "Features/ObjectManip/mnpCommands.hpp"
#include "Features/ObjectManip/orthoModeObjectManip.hpp"
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace mnpPackage
{
	//========================================================================
	//========================================================================
//	namespace
//	{	
		orthoModeObjectManip* l_pModeObjectManip = NULL;
		modeModeID l_ModeIDManip;
//	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Create the modes and add to the modeModeMgr
		l_pModeObjectManip = new orthoModeObjectManip();
		modeModeID modeIDObjectManip = modeModeMgr::AddMode( l_pModeObjectManip, true, "mode-edit.png" );
		l_ModeIDManip = modeIDObjectManip;

		// This is the default mode
		modeModeMgr::Push( modeIDObjectManip );

		//	set-up request interests
//		orthoRequestTaskUtil::Initialize();
		//
//WXGUI
/*
		mnpCommands::SetupMenu();
*/
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	clean-up request interests
//		orthoRequestTaskUtil::DeInitialize();

		// Let modeModeMgr destroy the modes? or needs a "RemoveMode" function
		modeModeMgr::RemoveMode(l_pModeObjectManip);
		delete l_pModeObjectManip;
	}

	//--------------------------------------------------------------------
	//	Launch the current file
	//--------------------------------------------------------------------
	void LaunchFile(fsLocator& i_SceneFile)
	{
		guiSingleDocHandler::Open(i_SceneFile);

		// now launch the correct mode
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDManip );
		DBG_ASSERT0( pMode != 0, "No object manip mode" );
		orthoModeObjectManip *pActualMode = dynamic_cast<orthoModeObjectManip*>(pMode);
		DBG_ASSERT0( pActualMode != 0, "mode ID is not the mode" );

		modeModeMgr::Push(l_ModeIDManip);
	}

	//--------------------------------------------------------------------
	//	Load then Convert the current file
	//--------------------------------------------------------------------
	void ConvertFile(fsLocator& i_SceneFile)
	{
		guiSingleDocHandler::Open(i_SceneFile);

		docSingleDocumentMgr::SetWriteFormat( docDocument::eDocXML );
		docSingleDocumentMgr::SaveDocument();

		modeModeMgr::Clear();

		// now launch the correct mode
		//modeMode* pMode = modeModeMgr::GetMode( l_ModeIDManip );
		//DBG_ASSERT0( pMode != 0, "No object manip mode" );
		//orthoModeObjectManip *pActualMode = dynamic_cast<orthoModeObjectManip*>(pMode);
		//DBG_ASSERT0( pActualMode != 0, "mode ID is not the mode" );
		//modeModeMgr::Push(l_ModeIDManip);
	}

}	// end of namespace