/*****************************************************************************
**	chnlDialogUtil.hpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_DIALOGUTIL_HPP
#error chnlDialogUtil.hpp multiply included
#endif
#define CHNL_DIALOGUTIL_HPP

#ifndef CHNL_TIMEDATA_HPP
#include "Features/Channels/Data/chnlTimeData.hpp"
#endif

#include <set>


//============================================================================
//============================================================================
class tmlnDriver;
class tmlnScriptObject;


//============================================================================
//============================================================================
namespace chnlDialogUtil
{
	//------------------------------------------------------------------------
	// Init
	//------------------------------------------------------------------------
	void  Init();

	//------------------------------------------------------------------------
	//  Clean up dialogs
	//------------------------------------------------------------------------
	void  CleanUp();

	//------------------------------------------------------------------------
	//  Create trax editor and show it if dialog memory has it open
	//------------------------------------------------------------------------
	void  CreateChannelEditor();

	//------------------------------------------------------------------------
	//  Show dialog to edit channels
	//------------------------------------------------------------------------
	void  ShowChannelEditor();

	//------------------------------------------------------------------------
	//  If you alter driver's properties and want the channel
	//	editor updated, call this function
	//------------------------------------------------------------------------
	void  UpdateChannels();

	//--------------------------------------------------------------------
	//  This driver has changed its properties related to the
	//	trax editor display, so update the channels related to it.
	//--------------------------------------------------------------------
	void  UpdateDriver(tmlnDriver *i_pDriver);

	//------------------------------------------------------------------------
	//  Called when a driver's time is altered, it updates the ticks
	//		for the snapping points.
	//------------------------------------------------------------------------
	void  UpdateTimeTicks();

	//------------------------------------------------------------------------
	//	Update Markers and Notes in trax editor
	//------------------------------------------------------------------------
	void UpdateMarkersAndNotes();

	//------------------------------------------------------------------------
	// Update channel editor based on selection, which can be NULL
	//------------------------------------------------------------------------
	void ObjectSelected(tmlnScriptObject* i_pObject);

	//------------------------------------------------------------------------
	// Call this depending on if an object is selected that can
	//	have a script object created for it.
	//------------------------------------------------------------------------
	void SetCanAddChannels(bool i_bCanAdd);

	//------------------------------------------------------------------------
	// Set amount of time covered by timeline
	//------------------------------------------------------------------------
	void SetTotalTime(float i_TotalTime);

	//------------------------------------------------------------------------
	// Get set of drivers that are selected in the trax editor. If skip locked
	// is true, then only drivers in channels that are unlocked will be returned.
	//------------------------------------------------------------------------
	void GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, bool i_bSkipLocked);

	//------------------------------------------------------------------------
	// Deselect all drivers in the channel interface
	//------------------------------------------------------------------------
	void ClearSelection();

	//------------------------------------------------------------------------
	// Select driver in the channel interface - this will do an append
	//	to the selection, it will not deselect other drivers.
	//------------------------------------------------------------------------
	void AddToSelection( tmlnDriver* i_pDriver );

}	// end of namespace
