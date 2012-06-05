/*****************************************************************************
**	chnlCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007- All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlCommands.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlOperations.hpp"
#include "Features/Channels/chnlTimeTickMgr.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"
#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"

#include "Features/Prefs/prefsMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriverClipboard.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeLineMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_Toolbar_Timeline_Name = "Timeline";

	//----------------------------------------------------------------------------
	// Common function for setting a new timeline time. Adjusts the time to
	// a frame and sets the new timeline in a deferred way.
	//----------------------------------------------------------------------------
	void set_new_time(const maTime& i_Time)
	{
		maTime time = i_Time;
		tmlnTimeUtil::AdjustTimeToFrame( time );
		// the "Deferred" call will set the timeline after the
		// current render thread finishes
		tmlnTimeLine::SetTimeDeferred(time);
	}

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_TimelineToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(c_Toolbar_Timeline_Name, i_bChecked);
	}
	bool Get_TimelineToolbar()
	{
		return guiToolbarMgr::IsVisible(c_Toolbar_Timeline_Name);
	}

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
		maTime new_time = maFunctions::Lowest(tmlnTimeLine::GetValue() + maTime::FromSeconds(1.0f), tmlnTimeLine::GetMaximum());
		tmlnTimeLine::SetValue( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Time Back One Second
	//--------------------------------------------------------------------
	void Execute_MoveBackOneSecond()
	{
		maTime new_time = maFunctions::Highest(tmlnTimeLine::GetValue() - maTime::FromSeconds(1.0f), tmlnTimeLine::GetMinimum());
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
		maTime old_time = tmlnTimeLine::GetDeferredTime();
		maTime new_time = maFunctions::Lowest(old_time + tmlnTimeLine::GetFrameIncrement(), 
											  tmlnTimeLine::GetMaximum());
		set_new_time( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Move Back One Frame
	//--------------------------------------------------------------------
	void Execute_MoveBackOneFrame()
	{
		maTime old_time = tmlnTimeLine::GetDeferredTime();
		maTime new_time = maFunctions::Highest(old_time - tmlnTimeLine::GetFrameIncrement(), 
											  tmlnTimeLine::GetMinimum());
		set_new_time( new_time );
	}

	//--------------------------------------------------------------------
	//	COMMAND: Timeline Zoom In
	//--------------------------------------------------------------------
	void Execute_TimelineZoomIn()
	{
	
#ifdef USE_WXWIDGETS
		// We get the zoom value and convert it to slider value. Increment this value and 
		// get the next zoom value. 
		if (chnlTraxDialog::FormInstance)
		{
			float val = chnlTraxDialog::FormInstance->GetSliderZoom()/100;
			val = ::powf(val * 100/5 ,.5);
			val = val *10; // This is the slider value
			val++;
			val = ::powf(val / 100, 2);
			val = val * 500; // 500 is the range.
			
			chnlTraxDialog::FormInstance->SetSliderZoom(val );
			chnlTraxDialog::FormInstance->SetTimeScale(val );
		}
#endif
	}

	//--------------------------------------------------------------------
	//	COMMAND: Timeline Zoom Out
	//--------------------------------------------------------------------
	void Execute_TimelineZoomOut()
	{
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
		{
			float val = chnlTraxDialog::FormInstance->GetSliderZoom()/100;
			val = ::powf(val * 100/5, .5);
			val = val *10;
			val--;
			val = ::powf(val / 100, 2);
			val = val * 500;

			chnlTraxDialog::FormInstance->SetSliderZoom(val);
			chnlTraxDialog::FormInstance->SetTimeScale(val);
			
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
		maTime min_time, max_time;
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
		maTime min_time, max_time;
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

	//	COMMAND: Timeline toolbar
	pCmd = new cmaCommandToggle("Timeline Toolbar", 
								"Toolbars", 
								"View the Timeline toolbar",
								&Set_TimelineToolbar, 
								&Get_TimelineToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "View Timeline Toolbar", pCmd );

	//	COMMAND: Driver Properties Dialog launch
	pCmd = new cmaCommandSimple("Driver Properties", 
								"Windows", 
								"Bring up the properties dialog for a driver",
								&Execute_DriverProperties );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Windows", "Driver Properties Dialog", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Drivers" );

	//	COMMAND: Driver Add
	// pCmd = new cmaCommandSimple("Add Driver", 
	//							"Drivers", 
	//							"Add the currently selected driver at the current time.",
	//							&Execute_AddDriver );
	////menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Add Driver", pCmd ); 

	//	COMMAND: Driver Delete
	pCmd = new cmaCommandSimple("Delete Driver", 
								"Drivers", 
								"Delete the currently selected driver at the current time.",
								&Execute_DeleteDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Delete Driver", pCmd );

	//	COMMAND: Driver Split
	pCmd = new cmaCommandSimple("Split Driver", 
								"Drivers", 
								"Split the currently selected driver at the current time.",
								&Execute_SplitDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Split Driver", pCmd );

	//	COMMAND: Driver Cut
	pCmd = new cmaCommandSimple("Cut Drivers", 
								"Drivers", 
								"Cut the currently selected driver at the current time.",
								&Execute_CutDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Cut Driver", pCmd );

	//	COMMAND: Driver Copy
	pCmd = new cmaCommandSimple("Copy Driver", 
								"Drivers", 
								"Copy the currently selected driver at the current time.",
								&Execute_CopyDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Copy Driver", pCmd );

	//	COMMAND: Driver Paste
	pCmd = new cmaCommandSimple("Paste Driver", 
								"Drivers", 
								"Paste the currently selected driver at the current time.",
								&Execute_PasteDriver );
	menu_id = guiMenuMgr::AddMenuItem( "Drivers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Driver Functions", "Paste Driver", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Markers" );

	//	COMMAND: Add Marker
	pCmd = new cmaCommandSimple("Marker Add", 
								"Markers", 
								"Add a marker at the current time.",
								&Execute_AddMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Markers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Add Marker", pCmd );

	//	COMMAND: Delete Marker
	pCmd = new cmaCommandSimple("Marker Delete", 
								"Markers", 
								"Delete a marker at the current time.",
								&Execute_DeleteMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Markers", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Delete Marker", pCmd );

	//
	guiMenuMgr::AddMenu( "Actions", "Timeline" );

	//	COMMAND: Move Time To Next Tick
	pCmd = new cmaCommandSimple("Move Time to Next Keyframe", 
								"Timeline", 
								"Move the current time to the next tick in the timeline (next driver or marker)",
								&Execute_MoveTimeToNextTick );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-tickFwd.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Time to Next KeyFrame", pCmd );

	//	COMMAND: Move Time To Prev Tick
	pCmd = new cmaCommandSimple("Move Time to Previous KeyFrame", 
								"Timeline", 
								"Move the current time to the Prev tick in the timeline (Prev driver or marker)",
								&Execute_MoveTimeToPrevTick );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-tickRev.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Time to Previous KeyFrame", pCmd );

	//	COMMAND: Move Time To Beginning of Timeline
	pCmd = new cmaCommandSimple("Move Time to Beginning of Timeline", 
								"Timeline", 
								"Move the current time to the Beginning of Timeline",
								&Execute_MoveToBeginningOfTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-begin.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Time to Beginning of Timeline", pCmd );

	//	COMMAND: Move Time To End of Timeline
	pCmd = new cmaCommandSimple("Move Time to End of Timeline", 
								"Timeline", 
								"Move the current time to the End of Timeline",
								&Execute_MoveToEndOfTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-end.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Time to End of Timeline", pCmd );

	//	COMMAND: Move To Next Marker
	pCmd = new cmaCommandSimple("Move To Next Marker", 
								"Timeline", 
								"Move to Next Marker in the Timeline",
								&Execute_MoveToNextMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-markerFwd.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move To Next Marker", pCmd );

	//	COMMAND: Move To Prev Marker
	pCmd = new cmaCommandSimple("Move To Previous Marker", 
								"Timeline", 
								"Move to Previous Marker in the Timeline",
								&Execute_MoveToPrevMarker );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-markerRev.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move To Previous Marker", pCmd );

	//	COMMAND: Move Ahead One Frame
	pCmd = new cmaCommandSimple("Move Ahead One Frame", 
								"Timeline", 
								"Move Ahead One Frame in the Timeline",
								&Execute_MoveAheadOneFrame );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-1frameFwd.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Ahead One Frame", pCmd );

	//	COMMAND: Move Back One Frame
	pCmd = new cmaCommandSimple("Move Back One Frame", 
								"Timeline", 
								"Move Back One Frame in the Timeline",
								&Execute_MoveBackOneFrame );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-1frameRev.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Back One Frame", pCmd );

	//	COMMAND: Move Ahead One Second
	pCmd = new cmaCommandSimple("Move Ahead One Second", 
								"Timeline", 
								"Move ahead one second in the Timeline",
								&Execute_MoveAheadOneSecond );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-1secFwd.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Ahead One Second", pCmd );

	//	COMMAND: Move Back One Second
	pCmd = new cmaCommandSimple("Move Back One Second", 
								"Timeline", 
								"Move back one second",
								&Execute_MoveBackOneSecond );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str(), c_Toolbar_Timeline_Name, "timeline-1secRev.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Move Back One Second", pCmd );

	//	COMMAND: Timeline Zoom In
	pCmd = new cmaCommandSimple("Zoom In", 
								"Timeline", 
								"Zoom In on the Timeline",
								&Execute_TimelineZoomIn );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Zoom In", pCmd );

	//	COMMAND: Timeline Zoom Out
	pCmd = new cmaCommandSimple("Zoom Out", 
								"Timeline", 
								"Zoom Out on the Timeline",
								&Execute_TimelineZoomOut );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Timeline", "Zoom Out", pCmd );

	//	COMMAND: Expand Timeline All Drivers
	pCmd = new cmaCommandSimple("Expand Timeline all Drivers", 
								"Timeline", 
								"Expand timeline range to make sure it includes all drivers",
								&Execute_ExpandTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Compress Timeline All Drivers
	pCmd = new cmaCommandSimple("Compress Timeline all Drivers", 
								"Timeline", 
								"Compress timeline range to frame just the time to include all drivers",
								&Execute_CompressTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Timeline", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

}
