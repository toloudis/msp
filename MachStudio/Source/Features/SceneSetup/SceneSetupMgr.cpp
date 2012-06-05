/*****************************************************************************
**	SceneSetupMgr.cpp
**
**		see .hpp
**
**	StudioGPU
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
		//int menu_id;
		//cmaCommand* pCmd;

		//	COMMAND: Project Settings
		//pCmd = new cmaCommandSimple("Project Settings", 
		//							"Edit", 
		//							"Project Settings",
		//							
		//							&SceneSetupDialogUtil::Show );
		//menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Edit", "Project Settings", pCmd );
	}

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	void  CleanUp()
	{
	}
}
