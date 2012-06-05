/*****************************************************************************
**  fgtPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FilmGates/fgtPackage.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Systems/Common/GUI/cmmPython.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace fgtPackage
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_menuitem_frames()
	{
		int menu_id;
		cmaCommand* pCmd;

		guiMenuMgr::AddMenu( "View", "Resolutions" );

		//	COMMAND: Toggle Safe Frame
		pCmd = new cmaCommandToggle("Action Safe Frame", 
									"Resolutions", 
									"Toggle display of action safe frame",
									&fgtFrameMgr::SetViewActionFrame,
									&fgtFrameMgr::GetViewActionFrame);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Resolutions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Resolutions", "Toggle Action Safe Frame Visibility", pCmd );

		pCmd = new cmaCommandToggle("Title Safe Frame", 
									"Resolutions", 
									"Toggle display of title safe frame",
									&fgtFrameMgr::SetViewTitleFrame,
									&fgtFrameMgr::GetViewTitleFrame);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Resolutions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Resolutions", "Toggle Title Safe Frame Visibility", pCmd );

		cmmPython::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	// Init -- initialize the package
	//--------------------------------------------------------------------
	void Init()
	{
		create_menuitem_frames();

		fgtFrameMgr::Init();

		//	Read in the valid safe frames
		//
		fgtFrameMgr::Read();
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		fgtFrameMgr::CleanUp();
	}
}
