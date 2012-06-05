/*****************************************************************************
**	tmlnChannel.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannel.hpp"

#include "Support/tmln/tmlnDriver.hpp"
#include "tmlnTimeline.hpp"

//#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannel::tmlnChannel(const char* i_Name, bool i_bMultiChannel)
:	m_Name(i_Name), 
	m_bMultiChannel(i_bMultiChannel),
	m_bLocked(false),
	m_LastOperateTime(-1),
	m_bNeedsOperate(true),
	m_Priority(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannel::~tmlnChannel()
{

}

//--------------------------------------------------------------------
//  Add driver to channel, the channel will point to it but
// not own it
//--------------------------------------------------------------------
void  tmlnChannel::AddDriver(tmlnDriver *i_Driver)
{
	m_Drivers.push_back(i_Driver);
	this->MarkDirty();
}


//--------------------------------------------------------------------
//  Remove driver from channel, this will not delete it.
//--------------------------------------------------------------------
void  tmlnChannel::RemoveDriver(tmlnDriver *i_Driver)
{
	envSTLHelpers::RemoveOneValue(m_Drivers, i_Driver);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  Get active drivers at a given time.  The set is appended to.
//	This is used in the process of updating drivers that are
//	active and will cause changes to happen to the channel 
//	and drivers, so it should not be used outside of that process.
//	Returns true if the channel should be Reset() before
//	evaluating drivers.
//--------------------------------------------------------------------
bool  tmlnChannel::GatherDriversToEvaluate(float i_Time,
					std::set<tmlnDriver*> &io_Preset,
					std::set<tmlnDriver*> &io_Active)
{
	bool bNeedsReset = false;

	bool found_active = false;
	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		tmlnDriver *pDriver = (*it);

		if ( tmlnTimeLine::GetLoopingToBeginning() )
		{
			pDriver->NotActive();
		}
		tmlnTimeLine::ResetLoopingToBeginning();

		//if (   (pDriver->GetBeginTime() <= i_Time)
		//	&& (pDriver->GetEndTime() >= i_Time))
		if (pDriver->IsWithin(i_Time))
		{
			io_Active.insert(pDriver);
			found_active = true;

			// if it's getting added to the list for the first time, let the driver
			//	decide if it needs to do anything special
			if ( pDriver->GetNotifyActive() )
			{
				pDriver->Active();
			}
		}
		else
		{
			// if it's not getting added for the first time after being active,
			//	let the driver do something about it.
			if ( !pDriver->GetNotifyActive() && pDriver->GetNotifyNotActive() )
			{
				pDriver->NotActive();
			}
		}
	}

	// If we didn't find a driver that is directly responsible for this time,
	// look for previous and next drivers
	if (!found_active)
	{
		tmlnDriver *pPrev = NULL, *pNext = NULL;
		float prev = -0.01f, next = -0.01f;
		for (it = m_Drivers.begin(); it != end; ++it)
		{
			tmlnDriver *pDriver = (*it);

			// find highest ending value less than i_Time
			float end_time = pDriver->GetEndTime();
			if (end_time <= i_Time && end_time > prev)
			{
				prev = end_time;
				pPrev = pDriver;
			}
			// find lowest begin value greater than i_Time
			float begin_time = pDriver->GetBeginTime();
			if (begin_time >= i_Time)
			{
				if (!pNext || begin_time < next)
				{
					next = begin_time;
					pNext = pDriver;
				}
			}
		}

		if (pNext)
		{
			io_Active.insert(pNext);

			if (pPrev && !pPrev->IsRestoreOriginalValue())
			{
				io_Preset.insert(pPrev);
			}
			else
			{
				//this->Reset(); // reset to "original" value
				bNeedsReset = true;
			}
		}
		else if (pPrev && !pPrev->IsRestoreOriginalValue())
		{
			// let previous driver set its end value
			 //io_Active.insert(pPrev);

			//bga - I am changing the meaning of this,
			// since this driver is not currently active
			// let's call it preset, even though it
			// is the driver that will set the value for this channel.
			// the reason for this change is that the driver may
			// alter more than this channel, so it needs to run earlier
			// than others drivers that might really be active
			// at this time on other channels.
			 io_Preset.insert(pPrev);
		}
		else
		{
			// no driver active
			//this->Reset(); // reset to "original" value
			bNeedsReset = true;
		}
	}

	return bNeedsReset;
}

//--------------------------------------------------------------------
//  Get active drivers within a given time range.  
//	The set is appended to.
//--------------------------------------------------------------------
void  tmlnChannel::GetActiveDrivers( float i_fStartTime, 
									 float i_fEndTime,
									 std::vector<tmlnDriver*> &io_Active )
{
	float begintime, endtime;

	// simple implementation, only active if within time bounds
	//
	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		begintime	= (*it)->GetBeginTime();
		endtime		= (*it)->GetEndTime();

		//	check if the driver falls in the range
		//
		if (   (begintime <= i_fStartTime && endtime >= i_fStartTime)
			|| (begintime <= i_fEndTime && endtime >= i_fEndTime)
			|| (i_fStartTime <= begintime && i_fEndTime >= endtime) )
		{
			io_Active.push_back( *it );
		}
	}
}

//--------------------------------------------------------------------
// Returns true if a driver on this channel would be executed at 
//	the given time. Note that some drivers may have end times
//	less than the given time, but if they do not restore the
//	channel's original value they are still considered active.
//--------------------------------------------------------------------
bool tmlnChannel::IsDriverActiveAtTime( float i_Time )
{
	// quick check
	if (m_Drivers.empty()) return false;

	tmlnDriver *pPrev = NULL, *pNext = NULL;
	float prev = 0.0f, next = 0.0f;

	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		tmlnDriver *pDriver = (*it);

		// find highest ending value less than i_Time
		float end_time = pDriver->GetEndTime();
		if (end_time <= i_Time && end_time > prev)
		{
			prev = end_time;
			pPrev = pDriver;
		}
		// find lowest begin value greater than i_Time
		float begin_time = pDriver->GetBeginTime();
		if (begin_time >= i_Time)
		{
			if (!pNext || begin_time < next)
			{
				next = begin_time;
				pNext = pDriver;
			}
		}

		// if time is within bounds, then this driver is definitely active,
		// return true right away.
		if (i_Time < end_time && i_Time > begin_time)
			return true;
	}

	// If we get here, then some drivers are in the channel,
	// but the time is not within any driver's bounds.
	// This means we need to check to see how the drivers 
	// before and after this time blend in and restore values.
	//
	// [rjk] i comment out this IF to allow for an object to be manipulated
	//	after a driver that has "IsRestoredOriginalValue" false.
	//
	if (pPrev && !pPrev->IsRestoreOriginalValue())
	{
		return true;
	}
	if (pNext)
	{
		// if the blend alpha for this driver is greater than 0.0
		// it will be influencing this channel.
		float prev_time = (pPrev) ? pPrev->GetEndTime() : 0.0f;
		float alpha = pNext->GetBlendAlpha(prev_time, i_Time);
		return (alpha > 0.0f);
	}
	return false;
}

//--------------------------------------------------------------------
// Return number of drivers on channel
//--------------------------------------------------------------------
int tmlnChannel::GetNumDrivers() const
{
	return (int)m_Drivers.size();
}

//--------------------------------------------------------------------
// Accessor to driver
//--------------------------------------------------------------------
const tmlnDriver& tmlnChannel::GetDriver(int i_Index) const
{
	return (*m_Drivers[i_Index]);
}
tmlnDriver& tmlnChannel::Driver(int i_Index)
{
	return (*m_Drivers[i_Index]);
}

//--------------------------------------------------------------------
//  Get end time of previous driver on this channel
//	Returns a very small negative number (could be treated as zero 
//	for blending purposes) if there are no previous drivers.
//--------------------------------------------------------------------
float  tmlnChannel::GetPreviousTime(float i_Time)
{
	// Could use sorting somehow, but i don't think there are
	// going to be many drivers per channel
	//
	//float prev = 0.0f;
	float prev = -0.0001f;
	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		// find highest ending value less than i_Time
		float end_time = (*it)->GetEndTime();
		if (end_time <= i_Time && end_time > prev)
		{
			prev = end_time;
		}
	}
	return prev;
}

//--------------------------------------------------------------------
// Get driver before the given time on this channel
// This tests end_time <= i_Time, so it will not find a driver 
// that overlaps this time.
// Returns NULL if there are no previous drivers.
//--------------------------------------------------------------------
tmlnDriver*  tmlnChannel::GetPreviousDriver(float i_Time)
{
	// Could use sorting somehow, but i don't think there are
	// going to be many drivers per channel
	//
	//float prev = 0.0f;
	float prev = -0.0001f;
	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	tmlnDriver* pMatch = NULL;
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		// find highest ending value less than i_Time
		float end_time = (*it)->GetEndTime();
		//if ( ((*it)->IsBefore(i_Time))
		if ( ((*it)->GetBeginTime() < i_Time)
			&& end_time <= i_Time && end_time > prev)
		{
			prev = end_time;
			pMatch = (*it);
		}
	}
	return pMatch;
}

//--------------------------------------------------------------------
// Get driver after the given time on this channel.
// This tests begin_time >= i_Time, so it will not find a driver 
// that overlaps this time.
// Returns NULL if there are no next drivers.
//--------------------------------------------------------------------
tmlnDriver*  tmlnChannel::GetNextDriver(float i_Time)
{
	// Could use sorting somehow, but i don't think there are
	// going to be many drivers per channel
	//
	float next = 0.0f;
	std::vector<tmlnDriver*>::iterator it, end = m_Drivers.end();
	tmlnDriver* pMatch = NULL;
	for (it = m_Drivers.begin(); it != end; ++it)
	{
		// find lowest ending value greater than i_Time
		float begin_time = (*it)->GetBeginTime();

		//	check if time falls within begin and end
		if ((*it)->GetEndTime() > i_Time && begin_time >= i_Time)
		{
			if (!pMatch || begin_time < next)
			{
				next = begin_time;
				pMatch = (*it);
			}
		}
	}
	return pMatch;
}


//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannel::Reset()
{
	// default has no implementation
}


//--------------------------------------------------------------------
//  GetChannelInfo - return a data structure with the channel data
//		for this channel suitable for writing to a file.
//--------------------------------------------------------------------
tmlnChannelInfo  tmlnChannel::GetChannelInfo() const
{
	tmlnChannelInfo info;
	info.m_bLocked = m_bLocked;
	return info;
}

//--------------------------------------------------------------------
//  SetChannelInfo - use the data structure to set info for this
//	channel.
//--------------------------------------------------------------------
void tmlnChannel::SetChannelInfo(const tmlnChannelInfo& i_Info)
{
	m_bLocked = i_Info.m_bLocked;
}

//--------------------------------------------------------------------
//  Return true if the channel needs the drivers to evaluate
//	for the given time. Checks i_Time against last time called.
//	Note: This call will reset the dirty bit and the time, so it
//	should only be called by the ChannelSet while doing the
//	Operate calls.
//--------------------------------------------------------------------
bool  tmlnChannel::GetNeedsOperate(float i_Time)
{
	bool bDirty =  (this->m_bNeedsOperate || 
		(i_Time != this->m_LastOperateTime));
	
	this->m_LastOperateTime = i_Time;
	this->m_bNeedsOperate = false;

	return bDirty;
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	channel is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//	Default implementation returns false.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannel::HasValueVariation()
{
	return false;
}
