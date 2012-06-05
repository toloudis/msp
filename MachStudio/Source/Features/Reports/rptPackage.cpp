/*****************************************************************************
**  rptPackage.cpp
**
**		see .hpp
**
**	StudioGPU
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


//============================================================================
//============================================================================
namespace rptPackage
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_menuitem_reports()
	{
#ifdef _DEBUG
		guiMenuMgr::AddMenu( "Actions", "Reports" );
#endif
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
#ifdef _DEBUG
		create_menuitem_reports();

		rptReportMgr::Init();

		//	ReportInv
		//
		// TODO: - come up with a better way of adding/maintaining reports
		//
		rptReportMgr::AddReport( new rptReportInv() );


		//
		//	commands
		//

		//bga - removed Reports menu in product
		////	COMMAND: Redo
		//pCmd = new cmaCommandSimple("Inventory", 
		//							"Reports", 
		//							"Report on the inventory for a scene",
		//							
		//							&undoUndoMgr::Redo );
		//menu_id = guiMenuMgr::AddMenuItem( "Reports", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Reports", "Inventory", pCmd );


		int menu_id;
		cmaCommand* pCmd;
		// COMMAND: MemUsage
		pCmd = new cmaCommandSimple("Memory Usage", 
									"Reports", 
									"Report on the memory usage for a scene",
									
									&rptReportMem::Generate );
		menu_id = guiMenuMgr::AddMenuItem( "Reports", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Reports", "Memory Usage", pCmd );
#endif
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		rptReportMgr::CleanUp();
	}
}
