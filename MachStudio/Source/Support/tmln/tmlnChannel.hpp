/*****************************************************************************
**	tmlnChannel.hpp
**
**	A Channel holds drivers that alter a specfic property of an
**	object
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNEL_HPP
#error tmlnChannel.hpp multiply included
#endif
#define TMLN_CHANNEL_HPP

#ifndef ENV_AUDITOR_HPP
#include "Core/Env/envAuditor.hpp"
#endif 
#ifndef TMLN_CHANNELINFO_HPP
#include "Support/tmln/tmlnChannelInfo.hpp"
#endif
#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 

#include <set>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class tmlnDriver;


//============================================================================
//============================================================================
class tmlnChannel : public envAuditable
{
public:
	//--------------------------------------------------------------------
	// Constructor sets name and flag for whether this channel can be
	//	split into categories.
	//--------------------------------------------------------------------
	tmlnChannel(const char* i_Name, bool i_bMultiChannel = false);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnChannel();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const std::string& GetName() const;
	inline void SetName(const char* i_Name);

	//--------------------------------------------------------------------
	// Integer representation of order that the channels should be
	//	handled when doing "Operate" on the drivers. Lower numbers are
	//	executed before larger numbers. The specific meaning of the 
	//	numbers is up to the user, but the default value is 0, so use
	//	negative numbers to have drivers execute earlier than usual.
	//--------------------------------------------------------------------
	inline int GetPriority() const;
	inline void SetPriority(int i_Priority);

	//--------------------------------------------------------------------
	//  Add driver to channel, the channel will point to it but
	// not own it
	//--------------------------------------------------------------------
	void  AddDriver(tmlnDriver *i_Driver);

	//--------------------------------------------------------------------
	//  Remove driver from channel, this will not delete it.
	//--------------------------------------------------------------------
	void  RemoveDriver(tmlnDriver *i_Driver);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ChannelLocked(bool i_lock);

	//--------------------------------------------------------------------
	//  Get active drivers at a given time.  The set is appended to.
	//	This is used in the process of updating drivers that are
	//	active and will cause changes to happen to the channel 
	//	and drivers, so it should not be used outside of that process.
	//	Returns true if the channel should be Reset() before
	//	evaluating drivers.
	//--------------------------------------------------------------------
	bool  GatherDriversToEvaluate(const maTime& i_Time,
							  std::set<tmlnDriver*> &io_Preset,
							  std::set<tmlnDriver*> &io_Active);

	//--------------------------------------------------------------------
	//  Get active drivers within a given time range.  
	//	The set is appended to.
	//--------------------------------------------------------------------
	void  GetActiveDrivers( const maTime& i_StartTime, const maTime& i_EndTime,
							std::vector<tmlnDriver*> &io_Active );

	//--------------------------------------------------------------------
	// Returns true if a driver on this channel would be executed at 
	//	the given time. Note that some drivers may have end times
	//	less than the given time, but if they do not restore the
	//	channel's original value they are still considered active.
	//--------------------------------------------------------------------
	bool IsDriverActiveAtTime( const maTime& i_Time );

	//--------------------------------------------------------------------
	// Return number of drivers on channel
	//--------------------------------------------------------------------
	int GetNumDrivers() const;

	//--------------------------------------------------------------------
	// Accessor to driver
	//--------------------------------------------------------------------
	const tmlnDriver& GetDriver(int i_Index) const;
	tmlnDriver& Driver(int i_Index);
	tmlnDriver* GetDriverDirect(int i_Index);

	//--------------------------------------------------------------------
	//  Get end time of previous driver on this channel.
	//	Returns a very small negative number (could be treated as zero 
	//	for blending purposes) if there are no previous drivers.
	//--------------------------------------------------------------------
	maTime  GetPreviousTime(const maTime& i_Time);

	//--------------------------------------------------------------------
	// Get driver before the given time on this channel.
	// This tests end_time <= i_Time, so it will not find a driver 
	// that overlaps this time.
	// Returns NULL if there are no previous drivers.
	//--------------------------------------------------------------------
	tmlnDriver*  GetPreviousDriver(const maTime& i_Time);

	//--------------------------------------------------------------------
	// Get driver after the given time on this channel.
	// This tests begin_time >= i_Time, so it will not find a driver 
	// that overlaps this time.
	// Returns NULL if there are no next drivers.
	//--------------------------------------------------------------------
	tmlnDriver*  GetNextDriver(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool IsMultiChannel() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool IsLocked() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline void SetLocked(bool i_bLocked);

	//--------------------------------------------------------------------
	//  GetChannelInfo - return a data structure with the channel data
	//		for this channel suitable for writing to a file.
	//--------------------------------------------------------------------
	tmlnChannelInfo  GetChannelInfo() const;

	//--------------------------------------------------------------------
	//  SetChannelInfo - use the data structure to set info for this
	//	channel.
	//--------------------------------------------------------------------
	void SetChannelInfo(const tmlnChannelInfo& i_Info);

	//--------------------------------------------------------------------
	//  Return true if the channel needs the drivers to evaluate
	//	for the given time. Checks i_Time against last time called.
	//	Note: This call will reset the dirty bit and the time, so it
	//	should only be called by the ChannelSet while doing the
	//	Operate calls.
	//--------------------------------------------------------------------
	virtual bool  GetNeedsOperate(const maTime& i_Time);

	//--------------------------------------------------------------------
	// MarkDirty - set dirty bit, meaning GetNeedsOperate() will return
	//	true next time, even if the time hasn't changed.
	//--------------------------------------------------------------------
	inline void MarkDirty();

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	channel is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//	Default implementation returns false.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

	//--------------------------------------------------------------------
	// Returns true if a channel can be keyed.
	//	Default implementation returns false.
	//--------------------------------------------------------------------
	virtual bool CanBeKeyed();

	//--------------------------------------------------------------------
	// Check if the channel contains the driver 
	//--------------------------------------------------------------------
	bool ContainsDriver(const tmlnDriver* i_Driver) const ;

private:
	std::string m_Name;
	std::vector<tmlnDriver*> m_Drivers;

	bool	m_bMultiChannel;
	bool	m_bLocked;
	maTime	m_LastOperateTime;
	bool	m_bNeedsOperate;
	int		m_Priority;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const std::string& tmlnChannel::GetName() const
{
	return m_Name;
}
inline void tmlnChannel::SetName(const char* i_Name)
{
	m_Name = i_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline int tmlnChannel::GetPriority() const
{
	return m_Priority;
}
inline void tmlnChannel::SetPriority(int i_Priority)
{
	m_Priority = i_Priority;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnChannel::IsMultiChannel() const
{
	return m_bMultiChannel;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnChannel::IsLocked() const
{
	return m_bLocked;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void tmlnChannel::SetLocked(bool i_bLocked)
{
	m_bLocked = i_bLocked;
}

//--------------------------------------------------------------------
// MarkDirty - set dirty bit, meaning GetNeedsOperate() will return
//	true next time, even if the time hasn't changed.
//--------------------------------------------------------------------
inline void tmlnChannel::MarkDirty()
{
	m_bNeedsOperate = true;
}

