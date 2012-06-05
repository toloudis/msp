/*****************************************************************************
**	tmlnScriptObject.hpp
**
**	This class represents an object that executes a timeline.
**	It owns some drivers and contains channels that contains
**	the timeline of the drivers on properties of this object.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_SCRIPTOBJECT_HPP
#error tmlnScriptObject.hpp multiply included
#endif
#define TMLN_SCRIPTOBJECT_HPP

#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
#endif 

#include <list>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class maTime;
class tmlnChannel;
class tmlnChannelSet;
class tmlnDriver;


//============================================================================
//============================================================================
class tmlnScriptObject : public relObject
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
	virtual std::string GetDisplayName() const;

	//--------------------------------------------------------------------
	//  Update active drivers. If a driver needs to be delayed to the
	//	end, it will be added to the delayed drivers list passed in.
	//--------------------------------------------------------------------
	void  Update(const maTime& i_Time,
				 std::list<tmlnDriver*> &o_DelayedDrivers);

	//--------------------------------------------------------------------
	// SetDriverRelationship - default is to connect the drivers to 
	//	this object, but derived classes can change this relationship
	//	by passing in a new one here. All existing drivers will switch
	//	to this relationship and all newly created drivers will use this
	//	relationship instead.
	//--------------------------------------------------------------------
	void SetDriverRelationship(relObject& i_ParentObject);

	//--------------------------------------------------------------------
	//  Add driver to object, the object will own the driver
	//--------------------------------------------------------------------
	void  AddDriver(tmlnDriver *i_pDriver);

	//--------------------------------------------------------------------
	//  Add driver to object, the object will own the driver.
	//  The index is used to put the driver back where it was in the
	//	driver list in order.
	//--------------------------------------------------------------------
	void  RestoreDriver(tmlnDriver *i_pDriver,
					    int i_DriverIndex);

	//--------------------------------------------------------------------
	//  Remove driver from object and its channels.
	//	This will not delete the driver. 
	//	Returns true if the driver was found and removed in this object.
	//  Returns old index for driver for use with RestoreDriver.
	//--------------------------------------------------------------------
	bool RemoveDriver(tmlnDriver *i_pDriver);
	bool RemoveDriver(tmlnDriver *i_pDriver,
					  int &o_DriverIndex);

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

	//--------------------------------------------------------------------
	// Set the flag for checking conflicts
	//--------------------------------------------------------------------
	void SetCheckForConflicts(bool i_bCheckForConflicts);

	//--------------------------------------------------------------------
	// Check if there is a need to prompt for driver conflicts
	//--------------------------------------------------------------------
	bool GetCheckForConflicts();
private:
	
	
	//--------------------------------------------------------------------
	//  Error reporting for driver conflicts
	//--------------------------------------------------------------------
	void error_report(const maTime& i_Time, const std::string& i_ChannelName) const;
	bool check_driver_conflicts(const tmlnDriver* i_pClip1, const tmlnDriver* i_pClip2,  tmlnChannel& i_pChannel) const;
		
	//--------------------------------------------------------------------
	//  Check driver for conflicts
	//--------------------------------------------------------------------
	int check_channel_for_driver_conflicts(tmlnChannel& i_Channel, const tmlnDriver* i_pDriver) const;
	
	//--------------------------------------------------------------------
	//   Loop through all the qualified channels and check for conflicts
	//--------------------------------------------------------------------
	int check_channel_for_driver_conflicts(const tmlnDriver *i_pDriver) const;

private:
	shared_ptr<relRelationship> m_DriverRelationship;
	std::vector<tmlnDriver*> m_Drivers;
	tmlnChannelSet *m_pChannelSet;
	bool l_bCheckForConflicts;

};
