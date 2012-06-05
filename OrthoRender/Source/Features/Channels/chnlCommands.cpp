/*****************************************************************************
**  chnlCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007- All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlCommands.hpp"

#include "Features/Channels/mGUI/chnlTraxEditor.h"

#include "Core/Ma/maFunctions.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//	COMMAND: Driver Properties Dialog launch
	//--------------------------------------------------------------------
	void Execute_DriverProperties()
	{
		std::set<tmlnDriver*> drivers;
		const bool i_bSkipLocked = false;
		chnlDialogUtil::GetSelectedDrivers(drivers, i_bSkipLocked);
		if (drivers.empty())
			guiDialogTabbedMgr::Show("Driver");
		else
			(*drivers.begin())->DoEditProperties();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Add Driver
	//--------------------------------------------------------------------
	void Execute_AddDriver()
	{
		chnlOperations::ShowAvailableDrivers();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Delete Driver
	//--------------------------------------------------------------------
	void Execute_DeleteDriver()
	{
		chnlOperations::DeleteSelectedDrivers();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Split Driver
	//--------------------------------------------------------------------
	void Execute_SplitDriver()
	{
		chnlOperations::SplitDrivers();
	}
	
	void Execute_CutDriver()
	{
		chnlOperations::CutDrivers();
	}
	//--------------------------------------------------------------------
	//	COMMAND: Copy Driver
	//--------------------------------------------------------------------
	void Execute_CopyDriver()
	{
		chnlOperations::CopyDrivers();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Paste Driver
	//--------------------------------------------------------------------
	void Execute_PasteDriver()
	{
		chnlOperations::PasteDrivers();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Add Marker
	//--------------------------------------------------------------------
	void Execute_AddMarker()
	{
		chnlMarkerOperations::AddMarker();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Delete Marker
	//--------------------------------------------------------------------
	void Execute_DeleteMarker()
	{
		chnlMarkerOperations::DeleteMarker();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time To Next Tick
	//--------------------------------------------------------------------
	void Execute_MoveTimeToNextTick()
	{
		chnlOperations::MoveTimeNextTick();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time To Prev Tick
	//--------------------------------------------------------------------
	void Execute_MoveTimeToPrevTick()
	{
		chnlOperations::MoveTimePrevTick();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time To Beginning of timeline
	//--------------------------------------------------------------------
	void Execute_MoveToBeginningOfTimeline()
	{
		tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time To End of timeline
	//--------------------------------------------------------------------
	void Execute_MoveToEndOfTimeline()
	{
		tmlnTimeLine::SetValue( tmlnTimeLine::GetMaximum() );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time Ahead One Second
	//--------------------------------------------------------------------
	void Execute_MoveAheadOneSecond()
	{
		float new_time = maFunctions::Lowest(tmlnTimeLine::GetValue() + 1.0f, tmlnTimeLine::GetMaximum());
		tmlnTimeLine::SetValue( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time Back One Second
	//--------------------------------------------------------------------
	void Execute_MoveBackOneSecond()
	{
		float new_time = maFunctions::Highest(tmlnTimeLine::GetValue() - 1.0f, tmlnTimeLine::GetMinimum());
		tmlnTimeLine::SetValue( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move To Next Marker
	//--------------------------------------------------------------------
	void Execute_MoveToNextMarker()
	{
		chnlMarkerOperations::MoveToNextMarker();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move To Prev Marker
	//--------------------------------------------------------------------
	void Execute_MoveToPrevMarker()
	{
		chnlMarkerOperations::MoveToPrevMarker();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Ahead One Frame
	//--------------------------------------------------------------------
	void Execute_MoveAheadOneFrame()
	{
		float new_time = maFunctions::Lowest(tmlnTimeLine::GetValue() + tmlnTimeLine::GetFrameIncrement(), 
											  tmlnTimeLine::GetMaximum());
		tmlnTimeLine::SetValue( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Back One Frame
	//--------------------------------------------------------------------
	void Execute_MoveBackOneFrame()
	{
		float new_time = maFunctions::Highest(tmlnTimeLine::GetValue() - tmlnTimeLine::GetFrameIncrement(), 
											  tmlnTimeLine::GetMinimum());
		tmlnTimeLine::SetValue( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Timeline Zoom In
	//--------------------------------------------------------------------
	void Execute_TimelineZoomIn()
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance != nullptr)
		{
			chnlTraxEditor::FormInstance->TimelineZoomIn();
		}
#endif
	}

	//--------------------------------------------------------------------
	//	COMMAND: Timeline Zoom Out
	//--------------------------------------------------------------------
	void Execute_TimelineZoomOut()
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance != nullptr)
		{
			chnlTraxEditor::FormInstance->TimelineZoomOut();
		}
#endif
	}

	//--------------------------------------------------------------------
	//	COMMAND: Delete Selected Clips
	//--------------------------------------------------------------------
	void Execute_DeleteSelectedClips()
	{
		chnlOperations::DeleteSelectedDrivers();
	}

	//--------------------------------------------------------------------
	//	COMMAND: Expand Timeline to include all drivers
	//--------------------------------------------------------------------
	void Execute_ExpandTimeline()
	{
		float min_time, max_time;
		if (tmlnTimelineMgr::GetDriverBounds(min_time, max_time))
		{
			// Expand minimum and maximum time, if needed, to include
			// all drivers.
			min_time = maFunctions::Lowest(min_time, tmlnTimeLine::GetMinimum());
			max_time = maFunctions::Highest(max_time, tmlnTimeLine::GetMaximum());
			tmlnTimeLine::SetTimeRange(min_time, max_time);
		}
	}

	//--------------------------------------------------------------------
	//	COMMAND: Compress Timeline to include all drivers
	//--------------------------------------------------------------------
	void Execute_CompressTimeline()
	{
		float min_time, max_time;
		if (tmlnTimelineMgr::GetDriverBounds(min_time, max_time))
		{
			// Set timeline to this range exactly
			tmlnTimeLine::SetTimeRange(min_time, max_time);
		}
	}
}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void chnlCommands::SetupMenu()
{
	int menu_id;
	cmaCommand* pCmd;

	//
	//	commands
	//

	//	COMMAND: Driver Properties Dialog launch
	pCmd = new cmaCommandSimple("Driver Properties", 
								"Timeline Functions", 
								"Bring up the properties dialog for a driver",
									
								&Execute_DriverProperties );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Windows", "Driver Properties Dialog", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Drivers" );

	//	COMMAND: Driver Add
	pCmd = new cmaCommandSimple("Add Driver", 
								"Timeline Functions", 
								"Add the currently selected driver at the current time.",
									
								&Execute_AddDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Add Driver", pCmd );

	//	COMMAND: Driver Delete
	pCmd = new cmaCommandSimple("Delete Driver", 
								"Timeline Functions", 
								"Delete the currently selected driver at the current time.",
									
								&Execute_DeleteDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Delete Driver", pCmd );

	//	COMMAND: Driver Split
	pCmd = new cmaCommandSimple("Split Driver", 
								"Timeline Functions", 
								"Split the currently selected driver at the current time.",
									
								&Execute_SplitDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Split Driver", pCmd );

	//	COMMAND: Driver Cut
	pCmd = new cmaCommandSimple("Cut Drivers", 
								"Timeline Functions", 
								"Cut the currently selected driver at the current time.",
									
								&Execute_CutDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Cut Driver", pCmd );

	//	COMMAND: Driver Copy
	pCmd = new cmaCommandSimple("Copy Driver", 
								"Timeline Functions", 
								"Copy the currently selected driver at the current time.",
									
								&Execute_CopyDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Copy Driver", pCmd );

	//	COMMAND: Driver Paste
	pCmd = new cmaCommandSimple("Paste Driver", 
								"Timeline Functions", 
								"Paste the currently selected driver at the current time.",
									
								&Execute_PasteDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Paste Driver", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Markers" );

	//	COMMAND: Add Marker
	pCmd = new cmaCommandSimple("Marker Add", 
								"Timeline Functions", 
								"Add a marker at the current time.",
									
								&Execute_AddMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Markers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Add Marker", pCmd );

	//	COMMAND: Delete Marker
	pCmd = new cmaCommandSimple("Marker Delete", 
								"Timeline Functions", 
								"Delete a marker at the current time.",
									
								&Execute_DeleteMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Markers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Delete Marker", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Timeline" );

	//	COMMAND: Move Time To Next Tick
	pCmd = new cmaCommandSimple("Move Time To Next Tick", 
								"Timeline Functions", 
								"Move the current time to the next tick in the timeline (next driver or marker)",
									
								&Execute_MoveTimeToNextTick );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Time To Next Tick", pCmd );

	//	COMMAND: Move Time To Prev Tick
	pCmd = new cmaCommandSimple("Move Time To Prev Tick", 
								"Timeline Functions", 
								"Move the current time to the Prev tick in the timeline (Prev driver or marker)",
									
								&Execute_MoveTimeToPrevTick );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Time To Prev Tick", pCmd );

	//	COMMAND: Move Time To Beginning of Timeline
	pCmd = new cmaCommandSimple("Move Time To Beginning of Timeline", 
								"Timeline Functions", 
								"Move the current time to the Beginning of Timeline",
									
								&Execute_MoveToBeginningOfTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Time To Beginning of Timeline", pCmd );

	//	COMMAND: Move Time To End of Timeline
	pCmd = new cmaCommandSimple("Move Time To End of Timeline", 
								"Timeline Functions", 
								"Move the current time to the End of Timeline",
									
								&Execute_MoveToEndOfTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Time To End of Timeline", pCmd );

	//	COMMAND: Move Ahead One Second
	pCmd = new cmaCommandSimple("Move Ahead One Second", 
								"Timeline Functions", 
								"Move ahead one second in the Timeline",
									
								&Execute_MoveAheadOneSecond );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Ahead One Second", pCmd );

	//	COMMAND: Move Back One Second
	pCmd = new cmaCommandSimple("Move Back One Second", 
								"Timeline Functions", 
								"Move back one second",
									
								&Execute_MoveBackOneSecond );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Back One Second", pCmd );

	//	COMMAND: Move To Next Marker
	pCmd = new cmaCommandSimple("Move To Next Marker", 
								"Timeline Functions", 
								"Move to Next Marker in the Timeline",
									
								&Execute_MoveToNextMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move To Next Marker", pCmd );

	//	COMMAND: Move To Prev Marker
	pCmd = new cmaCommandSimple("Move To Prev Marker", 
								"Timeline Functions", 
								"Move to Prev Marker in the Timeline",
									
								&Execute_MoveToPrevMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move To Prev Marker", pCmd );

	//	COMMAND: Move Ahead One Frame
	pCmd = new cmaCommandSimple("Move Ahead One Frame", 
								"Timeline Functions", 
								"Move Ahead One Frame in the Timeline",
									
								&Execute_MoveAheadOneFrame );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Ahead One Frame", pCmd );

	//	COMMAND: Move Back One Frame
	pCmd = new cmaCommandSimple("Move Back One Frame", 
								"Timeline Functions", 
								"Move Back One Frame in the Timeline",
									
								&Execute_MoveBackOneFrame );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Move Back One Frame", pCmd );

	//	COMMAND: Timeline Zoom In
	pCmd = new cmaCommandSimple("Zoom In", 
								"Timeline Functions", 
								"Zoom In on the Timeline",
									
								&Execute_TimelineZoomIn );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Zoom In", pCmd );

	//	COMMAND: Timeline Zoom Out
	pCmd = new cmaCommandSimple("Zoom Out", 
								"Timeline Functions", 
								"Zoom Out on the Timeline",
									
								&Execute_TimelineZoomOut );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline Functions", "Zoom Out", pCmd );

	//	COMMAND: Expand Timeline All Drivers
	pCmd = new cmaCommandSimple("Expand Timeline All Drivers", 
								"Timeline Functions", 
								"Expand timeline range to make sure it includes all drivers",
								&Execute_ExpandTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Compress Timeline All Drivers
	pCmd = new cmaCommandSimple("Compress Timeline All Drivers", 
								"Timeline Functions", 
								"Compress timeline range to frame just the time to include all drivers",
								&Execute_CompressTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

}
