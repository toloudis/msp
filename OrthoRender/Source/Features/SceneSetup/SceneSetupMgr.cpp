/*****************************************************************************
**	SceneSetupMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/SceneSetupMgr.hpp"

#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#include "Features/SceneSetup/Data/SceneSetupDocumentInterest.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <string>


//============================================================================
//============================================================================
namespace SceneSetupMgr
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		docSingleTypeMgr::AddDocumentInterest(new SceneSetupDocumentInterest());

		//
		//	commands
		//
//WXGUI
/*
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Import
		pCmd = new cmaCommandSimple("Project Settings", 
									"Tools", 
									"Project Settings",
									
									&SceneSetupDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Tools", "Project Settings", pCmd );
*/
	}

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	void  CleanUp()
	{
	}
}
