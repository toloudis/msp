/*****************************************************************************
**  rptPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/rptPackage.hpp"

#include "Features/Reports/rptReportMgr.hpp"
#include "Features/Reports/ReportInv/rptReportInv.hpp"
#include "Features/Reports/ReportMem/rptReportMem.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

//	tools
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"

//	lib
#include "Core/dbg/dbgLog.hpp"


//============================================================================
//============================================================================
namespace rptPackage
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_menuitem_reports()
	{
		guiMenuMgr::AddMenu( "Tools", "Reports" );

		//std::string cmdName = std::string( CommandPrefs::GetConstTagName() );

		//cmaCommand* pCmd = new CommandPrefs( index );
		//pCmd->SetTag( cmdName );
		//guiCommandMgr::Add( pCmd );
		//
		//cmaCommandMgr::RegisterObject( cmdName, index );
	}

	//--------------------------------------------------------------------
	// Init -- initialize the package
	//--------------------------------------------------------------------
	void Init()
	{
//WXGUI
/*
		create_menuitem_reports();
*/

		rptReportMgr::Init();

		//	ReportInv
		//
		// TODO: - come up with a better way of adding/maintaining reports
		//
		rptReportMgr::AddReport( new rptReportInv() );


		//
		//	commands
		//
//WXGUI
/*
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Redo
		pCmd = new cmaCommandSimple("Inventory", 
									"Reports", 
									"Report on the inventory for a scene",
									
									&undoUndoMgr::Redo );
		menu_id = guiMenuMgr::AddMenuItem( "Reports", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Reports", "Inventory", pCmd );

		// COMMAND: MemUsage
		pCmd = new cmaCommandSimple("MemUsage", 
									"Reports", 
									"Report on the memory usage for a scene",
									
									&rptReportMem::Generate );
		menu_id = guiMenuMgr::AddMenuItem( "Reports", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Reports", "MemUsage", pCmd );
*/
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		rptReportMgr::CleanUp();
	}
}
