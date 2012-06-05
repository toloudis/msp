/*****************************************************************************
**	tmlnScriptObject.hpp
**
**	This class represents an object that executes a timeline.
**	It owns some drivers and contains channels that contains
**	the timeline of the drivers on properties of this object.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_SCRIPTOBJECT_HPP
#error tmlnScriptObject.hpp multiply included
#endif
#define TMLN_SCRIPTOBJECT_HPP

#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif

#include <list>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class tmlnChannel;
class tmlnChannelSet;
class tmlnDriver;


//============================================================================
//============================================================================
class tmlnScriptObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnScriptObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnScriptObject();

	//--------------------------------------------------------------------
	// Get the name of the object.
	//--------------------------------------------------------------------
	virtual std::string GetTmlnName() const;

	//--------------------------------------------------------------------
	//  Update active drivers. If a driver needs to be delayed to the
	//	end, it will be added to the delayed drivers list passed in.
	//--------------------------------------------------------------------
	void  Update(float i_Time,
				 std::list<tmlnDriver*> &o_DelayedDrivers);

	//--------------------------------------------------------------------
	//  Add driver to object, the object will own the driver
	//--------------------------------------------------------------------
	void  AddDriver(tmlnDriver *i_pDriver);

	//--------------------------------------------------------------------
	//  Remove driver from object and its channels.
	//	This will not delete the driver. 
	//	Returns true if the driver was found and removed in this object.
	//--------------------------------------------------------------------
	bool RemoveDriver(tmlnDriver *i_pDriver);

	//--------------------------------------------------------------------
	//  Return true if this objects has the given driver
	//--------------------------------------------------------------------
	bool  HasDriver(tmlnDriver *i_pDriver);

	//--------------------------------------------------------------------
	//  Add Channel with given name. This class owns the channel,
	// but returns a pointer to it for convenience
	//--------------------------------------------------------------------
	tmlnChannel* AddChannel(const char *i_Name);

	//--------------------------------------------------------------------
	//	Adds channel to channel set, taking ownership of the channel
	//--------------------------------------------------------------------
	void AddChannel(tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	//	Removes channel, ownership passes back to caller
	//--------------------------------------------------------------------
	void RemoveChannel(tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	//	Return true of the channel already exists
	//--------------------------------------------------------------------
	bool ChannelExists(const char *i_Name);

	//--------------------------------------------------------------------
	// Accessor to channel set for GUI display
	//--------------------------------------------------------------------
	const tmlnChannelSet& GetChannelSet() const;
	tmlnChannelSet& ChannelSet();

	//--------------------------------------------------------------------
	// Get number of drivers
	//--------------------------------------------------------------------
	int GetNumDrivers() const;

	//--------------------------------------------------------------------
	// Accessor to drivers
	//--------------------------------------------------------------------
	const tmlnDriver& GetDriver(int i_Index) const;
	tmlnDriver* GetDriverDirect(int i_Index);

	//--------------------------------------------------------------------
	// Delete all drivers in this script object
	//--------------------------------------------------------------------
	void ClearDrivers();

	//--------------------------------------------------------------------
	//	ShowDriverIcons - show or hide icons used by the drivers
	//		that are not part of real scene.
	//--------------------------------------------------------------------
	void ShowDriverIcons( bool i_bVisible );

	//--------------------------------------------------------------------
	// This is called when a driver in this script object is changed,
	//	or if a driver is added or deleted from this object.
	//--------------------------------------------------------------------
	virtual void NotifyDriverChanged() = 0;

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& io_List );

private:
	std::vector<tmlnDriver*> m_Drivers;
	tmlnChannelSet *m_pChannelSet;

};
