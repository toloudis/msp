/*****************************************************************************
**	tmlnScriptObject.cpp
**
**	This class represents an object that executes a timeline.
**	It owns some drivers and contains channels that contains
**	the timeline of the drivers on properties of this object.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnDriver.hpp"

#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnScriptObject::tmlnScriptObject()
: m_pChannelSet(new tmlnChannelSet)
{
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
std::string tmlnScriptObject::GetTmlnName() const
{
	return std::string("");
}

//--------------------------------------------------------------------
//  Update active drivers
//--------------------------------------------------------------------
void  tmlnScriptObject::Update(float i_Time,
				 std::list<tmlnDriver*> &o_DelayedDrivers)
{
	m_pChannelSet->Update(i_Time, o_DelayedDrivers);
}

//--------------------------------------------------------------------
//  Add driver to object, the object will own the driver
//--------------------------------------------------------------------
void  tmlnScriptObject::AddDriver(tmlnDriver *i_pDriver)
{
	DBG_ASSERT0(i_pDriver != NULL, "Driver is NULL and shouldn't be");
	m_Drivers.push_back(i_pDriver);
}

//--------------------------------------------------------------------
//  Remove driver from object and its channels.
//	This will not delete the driver.
//--------------------------------------------------------------------
bool tmlnScriptObject::RemoveDriver(tmlnDriver *i_pDriver)
{
	if (envSTLHelpers::RemoveOneValue(m_Drivers, i_pDriver))
	{
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
	DBG_ASSERT0(i_pChannel != NULL, "Channel is NULL and shouldn't be");
	m_pChannelSet->AddChannel(i_pChannel);
}

//--------------------------------------------------------------------
//	Removes channel, ownership passes back to caller
//--------------------------------------------------------------------
void tmlnScriptObject::RemoveChannel(tmlnChannel* i_pChannel)
{
	DBG_ASSERT0(i_pChannel != NULL, "Channel is NULL and shouldn't be");
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
