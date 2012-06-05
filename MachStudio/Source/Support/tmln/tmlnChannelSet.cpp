/*****************************************************************************
**	tmlnChannelSet.cpp
**
**	This class contains a list of channels and defines
**	operations on them
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelSet.hpp"

#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnDriver.hpp"

#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace
{
	// Special struct for having a list of drivers that can
	// be sorted based on priority and on time
	struct sSortableDriver
	{
		//sSortableDriver()
		//	: m_pDriver(0), m_Priority(0) {}
		sSortableDriver(tmlnDriver* i_pDriver, int i_Priority)
			: m_pDriver(i_pDriver), m_Priority(i_Priority) {}

		tmlnDriver* m_pDriver;
		int m_Priority;

		bool operator==(const sSortableDriver& i_Cmp) const
		{
			return (m_pDriver == i_Cmp.m_pDriver);
		}
	};

	bool get_dirty(const std::set<tmlnDriver*> &i_Set)
	{
		std::set<tmlnDriver*>::const_iterator it, end = i_Set.end();
		for (it = i_Set.begin(); it != end; ++it)
		{
			if ((*it)->IsDirty())
				return true;
		}
		return false;
	}

	// This was switched from std::set to std::vector in order to
	// be able to preserve the order of the drivers based on the
	// priorities of the channels
	void merge(std::vector<sSortableDriver> &o_Dest, 
			   const std::set<tmlnDriver*> &i_Src,
			   int i_Priority)
	{
		std::set<tmlnDriver*>::const_iterator it, end = i_Src.end();
		for (it = i_Src.begin(); it != end; ++it)
		{
			// Only insert if driver does not already exist
			sSortableDriver driver(*it, i_Priority);
			if (std::find(o_Dest.begin(), o_Dest.end(), driver) == o_Dest.end())
				o_Dest.push_back(driver);
		}
	}

	// predicate for sorting drivers based on end time
	struct endtime_driver_sort
	{
		inline bool operator()(const sSortableDriver& lhs, const sSortableDriver& rhs) 
		{ 
			if (lhs.m_Priority == rhs.m_Priority)
				return (lhs.m_pDriver->GetEndTime() < rhs.m_pDriver->GetEndTime());
			else
				return (lhs.m_Priority < rhs.m_Priority);
		} 
	};
	// predicate for sorting drivers based on begin time,
	// sorting later times before earlier times
	// (bringing the simulation time closer to the current time)
	struct begintime_driver_sort
	{
		inline bool operator()(const sSortableDriver& lhs, const sSortableDriver& rhs) 
		{ 
			if (lhs.m_Priority == rhs.m_Priority)
				return (lhs.m_pDriver->GetBeginTime() > rhs.m_pDriver->GetBeginTime());
			else
				return (lhs.m_Priority < rhs.m_Priority);
		} 
	};

}	// end of anonymous namespace

//--------------------------------------------------------------------
// destructor - deletes channels in set.
//--------------------------------------------------------------------
tmlnChannelSet::~tmlnChannelSet()
{
	//int num_channels = m_Channels.size();
	//DBG_LOG("deleting " << num_channels << " tmlnChannelSet channels");
	//for (int i=0 ; i < num_channels ; ++i)
	//{
	//	DBG_LOG( "  " << i << " Channel " << m_Channels[i]->GetName() );
	//}

	//	delete the channels
	envSTLHelpers::DeleteContainer(this->m_Channels);
}

//--------------------------------------------------------------------
//  Add Channel with given name. This class owns the channel,
// but returns a pointer to it for convenience
//--------------------------------------------------------------------
tmlnChannel * tmlnChannelSet::AddChannel(const char *i_Name)
{
	tmlnChannel *channel = new tmlnChannel(i_Name);
	this->AddChannel(channel);
	return channel;
}

//--------------------------------------------------------------------
//	Adds channel to channel set, taking ownership of the channel
//--------------------------------------------------------------------
void tmlnChannelSet::AddChannel(tmlnChannel* i_pChannel)
{
	m_Channels.push_back(i_pChannel);
}


//--------------------------------------------------------------------
//	Removes channel, ownership passes back to caller
//--------------------------------------------------------------------
void tmlnChannelSet::RemoveChannel(tmlnChannel* i_pChannel)
{
	envSTLHelpers::RemoveOneValue(m_Channels, i_pChannel);
}

//--------------------------------------------------------------------
//  return number of channels in set
//--------------------------------------------------------------------
int  tmlnChannelSet::GetNumChannels() const
{
	return (int) m_Channels.size();
}

//--------------------------------------------------------------------
//  Accessor to channel by index
//--------------------------------------------------------------------
tmlnChannel& tmlnChannelSet::Channel(int i_Index)
{
	return (*m_Channels[i_Index]);
}

//--------------------------------------------------------------------
// GetChannel
//--------------------------------------------------------------------
const tmlnChannel& tmlnChannelSet::GetChannel(int i_Index) const
{
	return (*m_Channels[i_Index]);

}


//--------------------------------------------------------------------
// GetChannel - returns the index.  if not found it returns -1
//--------------------------------------------------------------------
int tmlnChannelSet::GetChannel(const char *i_Name) const
{
	int num = m_Channels.size();
	for (int i=0; i<num; i++)
	{
		if (_stricmp(m_Channels[i]->GetName().c_str(), i_Name) == 0)
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
// Unhook this drivers from all channels
//--------------------------------------------------------------------
void tmlnChannelSet::UnhookDriver(tmlnDriver* i_pDriver)
{
	int num = m_Channels.size();
	for (int i=0; i<num; i++)
	{
		m_Channels[i]->RemoveDriver(i_pDriver);
	}
}

//--------------------------------------------------------------------
//  Update drivers in channels for given time. If a driver needs 
//  to be delayed to the end, it will be added to the delayed 
//  drivers list passed in.
//--------------------------------------------------------------------
void  tmlnChannelSet::Update(const maTime& i_Time,
							 std::list<tmlnDriver*> &o_DelayedDrivers)
{
	// First Gather active drivers
	std::vector<sSortableDriver> init_drivers;
	std::vector<sSortableDriver> active_drivers;
	{
		// Iterate through channels
		std::vector<tmlnChannel*>::iterator it, end = m_Channels.end();
		for (it = m_Channels.begin(); it != end; ++it)
		{
			std::set<tmlnDriver*> preset, active;
			tmlnChannel *pChannel = (*it);
			bool bResetChannel = pChannel->GatherDriversToEvaluate(i_Time, preset, active);

			// Find out if channel needs its drivers to evaluate...
			// If the time has changed, then we need to evaluate.
			bool bDirty = pChannel->GetNeedsOperate(i_Time);
			if (!bDirty)
			{
				// Check to see if any of the relevant drivers have had
				// their data changed recently, which would require
				// new evaluations.
				bDirty |= get_dirty(preset);
				bDirty |= get_dirty(active);
			}

			// If we do need to evaluate the drivers, add in the drivers
			// to the main lists.
			if (bDirty)
			{
				if (bResetChannel) 
					pChannel->Reset();

				merge(init_drivers, preset, pChannel->GetPriority());
				merge(active_drivers, active, pChannel->GetPriority());
			}
		}
	}

	// It is possible, when a driver affects more than one channel, for the same
	// driver to appear in both the init_drivers and the active_drivers lists. 
	// If this is the case, then it should be removed from the active_drivers list. 
	// The instance in init_drivers will handle all necessary cases that I can think of.
	{
		// This could be optimized later because we only really need to
		// check the drivers that affect more than one channel. But we
		// would then need to add a flag to tmlnDriver and make sure that
		// it was set correctly in all cases.
		std::vector<sSortableDriver>::iterator it, end = init_drivers.end();
		for (it = init_drivers.begin(); it != end; ++it)
		{
			envSTLHelpers::RemoveAllValues(active_drivers, *it);
		}
	}

	// Now sort the initial drivers so that they execute in order of their end times
	// this keeps the flow of time from moving back and forth and is needed
	// when drivers alter more than one channel when they operte.
	std::sort(init_drivers.begin(), init_drivers.end(), endtime_driver_sort());
	// Similar argument means that active drivers need to execute in 
	// order of their begin times
	std::sort(active_drivers.begin(), active_drivers.end(), begintime_driver_sort());

	// Set up initial state here with prev drivers
	bool bPreviousDriverDelayed = false;
	{
		std::vector<sSortableDriver>::iterator it, end = init_drivers.end();
		for (it = init_drivers.begin(); it != end; ++it)
		{
			tmlnDriver *driver = it->m_pDriver;
			if (driver->NeedsDelayedOperate())
			{
				o_DelayedDrivers.push_back(driver);
				bPreviousDriverDelayed = true;
			}
			else
			{
				driver->ClearDirty();
				driver->Operate(i_Time);
			}
		}
	}

	// Then call Operate on the active drivers
	{
		std::vector<sSortableDriver>::iterator it, end = active_drivers.end();
		for (it = active_drivers.begin(); it != end; ++it)
		{
			tmlnDriver *driver = it->m_pDriver;
			if (bPreviousDriverDelayed || driver->NeedsDelayedOperate())
			{
				o_DelayedDrivers.push_back(driver);
			}
			else
			{
				driver->ClearDirty();
				driver->Operate(i_Time);
			}
		}
	}
}

//--------------------------------------------------------------------
// Set channel info from a map of names of channels to info structures
//--------------------------------------------------------------------
void tmlnChannelSet::SetChannelInfo(const std::map<std::string, tmlnChannelInfo> &i_ChannelInfoMap)
{
	const tmlnChannelInfo c_DefaultInfo; 
	std::vector<tmlnChannel*>::iterator it, end = m_Channels.end();
	for (it = m_Channels.begin(); it != end; ++it)
	{
		std::map<std::string, tmlnChannelInfo>::const_iterator info_it = i_ChannelInfoMap.find((*it)->GetName());
		if (info_it != i_ChannelInfoMap.end())
			(*it)->SetChannelInfo(info_it->second);
		else
			(*it)->SetChannelInfo( c_DefaultInfo ); // set value to defaults
	}
}

//--------------------------------------------------------------------
// Get channel info as a map of names of channels to info structures
//--------------------------------------------------------------------
void tmlnChannelSet::GetChannelInfo(std::map<std::string, tmlnChannelInfo> &o_ChannelInfoMap) const
{
	// get default settings in order to tell when anything has changed
	const tmlnChannelInfo c_DefaultInfo; 
	std::vector<tmlnChannel*>::const_iterator it, end = m_Channels.end();
	for (it = m_Channels.begin(); it != end; ++it)
	{
		tmlnChannelInfo info = (*it)->GetChannelInfo();

		// only gather info on channels that have differences. Most channels
		// will not need to have info described, so it would be better
		// to not have to write out channel info if we don't need to.
		if (info != c_DefaultInfo)
			o_ChannelInfoMap[(*it)->GetName()] = info;
	}
}