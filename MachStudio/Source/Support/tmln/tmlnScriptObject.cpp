/*****************************************************************************
**	tmlnScriptObject.cpp
**
**	This class represents an object that executes a timeline.
**	It owns some drivers and contains channels that contains
**	the timeline of the drivers on properties of this object.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnScriptObject.hpp"


#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"

#include <sstream>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnScriptObject::tmlnScriptObject()
: m_pChannelSet(new tmlnChannelSet)
{				
	m_DriverRelationship.reset(new relRelationshipMultiple<tmlnDriver>("Drivers", *this, m_Drivers));
	l_bCheckForConflicts = true;
	this->AddRelationship(m_DriverRelationship);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnScriptObject::~tmlnScriptObject()
{
	delete m_pChannelSet;
	envSTLHelpers::DeleteContainer(m_Drivers);
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string tmlnScriptObject::GetDisplayName() const
{
	return std::string("");
}

//--------------------------------------------------------------------
//  Update active drivers
//--------------------------------------------------------------------
void  tmlnScriptObject::Update(const maTime& i_Time,
				 std::list<tmlnDriver*> &o_DelayedDrivers)
{
	m_pChannelSet->Update(i_Time, o_DelayedDrivers);
}

//--------------------------------------------------------------------
// SetDriverRelationship - default is to connect the drivers to 
//	this object, but derived classes can change this relationship
//	by passing in a new one here. All existing drivers will switch
//	to this relationship and all newly created drivers will use this
//	relationship instead.
//--------------------------------------------------------------------
void tmlnScriptObject::SetDriverRelationship(relObject& i_ParentObject)
{	
	// remove old relationship
	this->RemoveRelationship(m_DriverRelationship);

	// Create new multiple relationship connecting the drivers to the new 
	// parent object
	m_DriverRelationship.reset(new relRelationshipMultiple<tmlnDriver>("Drivers", i_ParentObject, m_Drivers));
	i_ParentObject.AddRelationship(m_DriverRelationship);

	const int num_drivers = m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
		m_Drivers[i]->SetParentRelationship(m_DriverRelationship);
}



//--------------------------------------------------------------------
//  Add driver to object, the object will own the driver
//--------------------------------------------------------------------
void  tmlnScriptObject::AddDriver(tmlnDriver *i_pDriver)
{
	DBG_ASSERT(i_pDriver != NULL, "Driver is NULL and shouldn't be");
	if(l_bCheckForConflicts)
		int num_conflicts = check_channel_for_driver_conflicts(i_pDriver);
	m_Drivers.push_back(i_pDriver);
	i_pDriver->SetParentRelationship(m_DriverRelationship);
}


//--------------------------------------------------------------------
//  Add driver to object, the object will own the driver.
//  The index is used to put the driver back where it was in the
//	driver list in order.
//--------------------------------------------------------------------
void  tmlnScriptObject::RestoreDriver(tmlnDriver *i_pDriver,
									  int i_DriverIndex)
{
	DBG_ASSERT(i_pDriver != NULL, "Driver is NULL and shouldn't be");
	/*
		insert doesn't seem to resize the vector properly, so putting
		the drivers back in place would crash if their index was out of
		bounds.
	*/
	//m_Drivers.insert(m_Drivers.begin()+i_DriverIndex, i_pDriver);
	m_Drivers.push_back(i_pDriver);
	i_pDriver->SetParentRelationship(m_DriverRelationship);
}


//--------------------------------------------------------------------
//  Remove driver from object and its channels.
//	This will not delete the driver.
//--------------------------------------------------------------------
bool tmlnScriptObject::RemoveDriver(tmlnDriver *i_pDriver)
{
	int driver_index = 0;
	return RemoveDriver(i_pDriver, driver_index);
}
bool tmlnScriptObject::RemoveDriver(tmlnDriver *i_pDriver,
									int &o_DriverIndex)
{
	std::vector<tmlnDriver*>::iterator it = std::find(m_Drivers.begin(), m_Drivers.end(), i_pDriver);
	if (it != m_Drivers.end())
	{
		o_DriverIndex = (int)(it - m_Drivers.begin());
		m_Drivers.erase(it);
		this->m_pChannelSet->UnhookDriver(i_pDriver);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------
//  Return true if this objects has the given driver
//--------------------------------------------------------------------
bool  tmlnScriptObject::HasDriver(tmlnDriver *i_pDriver)
{
	return envSTLHelpers::Contains(m_Drivers, i_pDriver);
}

//--------------------------------------------------------------------
//  Add Channel with given name. This class owns the channel,
// but returns a pointer to it for convenience
//--------------------------------------------------------------------
tmlnChannel* tmlnScriptObject::AddChannel(const char *i_Name)
{
	return m_pChannelSet->AddChannel(i_Name);
}

//--------------------------------------------------------------------
//	Adds channel to channel set, taking ownership of the channel
//--------------------------------------------------------------------
void tmlnScriptObject::AddChannel(tmlnChannel* i_pChannel)
{
	DBG_ASSERT(i_pChannel != NULL, "Channel is NULL and shouldn't be");
	m_pChannelSet->AddChannel(i_pChannel);
}

//--------------------------------------------------------------------
//	Removes channel, ownership passes back to caller
//--------------------------------------------------------------------
void tmlnScriptObject::RemoveChannel(tmlnChannel* i_pChannel)
{
	DBG_ASSERT(i_pChannel != NULL, "Channel is NULL and shouldn't be");
	m_pChannelSet->RemoveChannel(i_pChannel);
}

//--------------------------------------------------------------------
//	Return true of the channel already exists
//--------------------------------------------------------------------
bool tmlnScriptObject::ChannelExists(const char *i_Name)
{
	int index = m_pChannelSet->GetChannel(i_Name);

	return (index != -1);
}

//--------------------------------------------------------------------
// Accessor to channel set for GUI display
//--------------------------------------------------------------------
const tmlnChannelSet& tmlnScriptObject::GetChannelSet() const
{
	return (*m_pChannelSet);
}
tmlnChannelSet& tmlnScriptObject::ChannelSet()
{
	return (*m_pChannelSet);
}

//--------------------------------------------------------------------
// Get number of drivers
//--------------------------------------------------------------------
int tmlnScriptObject::GetNumDrivers() const
{
	return m_Drivers.size();
}

//--------------------------------------------------------------------
// Accessor to drivers
//--------------------------------------------------------------------
const tmlnDriver& tmlnScriptObject::GetDriver(int i_Index) const
{
	return (*m_Drivers[i_Index]);
}

tmlnDriver* tmlnScriptObject::GetDriverDirect(int i_Index)
{
	return( m_Drivers[i_Index] );
}

//--------------------------------------------------------------------
// Delete all drivers in this script object
//--------------------------------------------------------------------
void tmlnScriptObject::ClearDrivers()
{
	// Disconnect these drivers from the channels
	int num_drivers = m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
		this->m_pChannelSet->UnhookDriver(m_Drivers[i]);

	envSTLHelpers::DeleteContainer(m_Drivers);
}


//--------------------------------------------------------------------
//	ShowDriverIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void tmlnScriptObject::ShowDriverIcons( bool i_bVisible )
{
	int ndrivers = m_Drivers.size();
	for (int i=0; i<ndrivers; i++)
	{
		m_Drivers[i]->ShowIcons(i_bVisible);
	}
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void tmlnScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	int ndrivers = m_Drivers.size();
	for (int i=0; i<ndrivers; i++)
	{
		m_Drivers[i]->GetResourceList(io_List);
	}
}

//--------------------------------------------------------------------
// Set the flag for checking conflicts
//--------------------------------------------------------------------
void tmlnScriptObject::SetCheckForConflicts(bool i_bCheckForConflicts)
{
	l_bCheckForConflicts = i_bCheckForConflicts;
}

//--------------------------------------------------------------------
// Check if there is a need to prompt for driver conflicts
//--------------------------------------------------------------------
bool tmlnScriptObject::GetCheckForConflicts()
{
	return l_bCheckForConflicts;
}


//--------------------------------------------------------------------
//  Error reporting for driver conflicts
//--------------------------------------------------------------------
void tmlnScriptObject::error_report(const maTime& i_Time, const std::string& i_ChannelName) const 
{
	std::ostringstream oss;
	std::string time_string;
	tmlnTimeUtil::GetTimeString(i_Time, time_string);
	oss<< " Conflict in Object : " << this->GetDisplayName()<<std::endl;
	oss << "Drivers in Channel \""<< i_ChannelName << "\" conflicts at " << time_string;

	guiMessageBox::Show(oss.str().c_str(), "Error",guiMessageBox::e_OKOnly);
	DBG_LOG(oss.str().c_str());
}


bool tmlnScriptObject::check_driver_conflicts(const tmlnDriver* i_pClip1, const tmlnDriver* i_pClip2, tmlnChannel& i_Channel) const
{
	const char* channelName = i_Channel.GetName().c_str();

	// Avoid checking against itself
	if(i_pClip1 == i_pClip2)
	{
		return false;
	}
		
	maTime c1s = i_pClip1->GetBeginTime();
	maTime c1e = i_pClip1->GetEndTime();
	
	maTime c2s = i_pClip2->GetBeginTime();
	maTime c2e = i_pClip2->GetEndTime();
	
	// check times to see if overlap
	//
	if  ((c2s < c1s) && (c2e > c1s))		// clip 2 start inside
	{
		error_report(c1s, channelName );
		return true;
	}
	else if ((c2s < c1e) && (c2e > c1e))	// clip 2 end inside
	{
		error_report(c1e, channelName);
		return true;
	}
	else if ((c1s < c2s) && (c1e > c2s))	// clip 1 start inside
	{
		error_report(c2s, channelName);
		return true;
	}
	else if ((c1s < c2e) && (c1e > c2e))   	// clip 1 end inside
	{
		error_report(c1s, channelName);
		return true;
	}
	else if ((c1s == c2s) && ( c1e == c2e)) // keys overlap
	{
		error_report(c1s, channelName);
		return true;
	}
	
	return false;
}
//--------------------------------------------------------------------
//  Check driver for conflicts
//--------------------------------------------------------------------
int tmlnScriptObject::check_channel_for_driver_conflicts(tmlnChannel& i_Channel, const tmlnDriver* i_pClip) const
{
	int clips_count = i_Channel.GetNumDrivers(); 
	int num_conflicts = 0;

	for (int i=0; i < clips_count; ++i)
	{
		//tmlnDriver* clip1 = dynamic_cast<tmlnDriver*>(i_pChannel.GetDriverDirect(i));
		//for (int j=i+1; j < clips_count; ++j)
		{
			tmlnDriver* pClip = i_Channel.GetDriverDirect(i);
			
			if(check_driver_conflicts(pClip, i_pClip, i_Channel))
			{
				++num_conflicts;
			}
		}
	}
	return num_conflicts;
}

//--------------------------------------------------------------------
//  Loop through all the qualified channels and check for conflicts
//--------------------------------------------------------------------
int tmlnScriptObject::check_channel_for_driver_conflicts(const tmlnDriver* i_pClip) const
{
	int num_conflicts = 0;
	
	const tmlnChannelSet& channel_set = this->GetChannelSet();
	const int num_names = channel_set.GetNumChannels();
	for(int i=0; i < this->m_pChannelSet->GetNumChannels();i++)
	{
		tmlnChannel channel = channel_set.GetChannel(i);
		// Check if the channel contains the driver. Check for conflict only
		// if it does.
		if(channel.ContainsDriver(i_pClip))
			num_conflicts +=check_channel_for_driver_conflicts(channel,i_pClip);
	}
	return num_conflicts;
}
