/*****************************************************************************
**	chnlOperations.hpp
**
**	Functionality from user interface of channel trax editor
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_OPERATIONS_HPP
#error chnlOperations.hpp multiply included
#endif
#define CHNL_OPERATIONS_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif 


#include <vector>
#include <set>


//============================================================================
//============================================================================
class chnlMarkerDataItem;
class chnlNoteDataItem;
class tmlnScriptObject;
class tmlnDriver;


//============================================================================
//============================================================================
namespace chnlOperations
{
	//--------------------------------------------------------------------
	// Show available drivers for selected object
	//--------------------------------------------------------------------
	void ShowAvailableDrivers();
	
	//--------------------------------------------------------------------
	// Delete selected drivers from selected script objects
	//--------------------------------------------------------------------
	void DeleteSelectedDrivers();

	//--------------------------------------------------------------------
	// Delete given drivers from selected script objects
	//--------------------------------------------------------------------
	void DeleteDrivers(std::set<tmlnDriver*>& i_Drivers);

	//--------------------------------------------------------------------
	// Set blend type for all selected drivers at once
	//--------------------------------------------------------------------
	void SetSelectedDriversBlend(tmlnDriver::BlendType i_BlendType);

	//--------------------------------------------------------------------
	// Remove the link between two drivers to form two individual drivers
	//--------------------------------------------------------------------
	void CutDrivers();
	
	//--------------------------------------------------------------------
	// Copy information about selected drivers to clipboard
	//--------------------------------------------------------------------
	void CopyDrivers();

	//--------------------------------------------------------------------
	// Create new drivers bsaed on information in clipboard
	//--------------------------------------------------------------------
	void PasteDrivers();

	//--------------------------------------------------------------------
	// Split selected drivers at current time
	//--------------------------------------------------------------------
	void SplitDrivers();

	//--------------------------------------------------------------------
	// Move time forward or backwards to next time tick
	//--------------------------------------------------------------------
	void MoveTimeNextTick();
	void MoveTimePrevTick();

	//--------------------------------------------------------------------
	//  When a driver's properties have changed, need to notify the
	//		script object that there were changes so the corresponding
	//		data can be updated and dirty bits set.
	//--------------------------------------------------------------------
	void  NotifyScriptObject();

	//--------------------------------------------------------------------
	// Set up callbacks for this driver to update trax editor display
	//--------------------------------------------------------------------
	void MonitorChanges(tmlnDriver &i_Driver);

	//--------------------------------------------------------------------
	// Clear out all the monitoring of drivers
	//--------------------------------------------------------------------
	void ClearMonitors();


}	// end of namespace
