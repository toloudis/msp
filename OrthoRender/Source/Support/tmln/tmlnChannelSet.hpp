/*****************************************************************************
**	tmlnChannelSet.hpp
**
**	This class contains a list of channels and defines
**	operations on them
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELSET_HPP
#error tmlnChannelSet.hpp multiply included
#endif
#define TMLN_CHANNELSET_HPP

#include <map>
#include <list>
#include <vector>


//============================================================================
//============================================================================
class tmlnChannel;
class tmlnChannelInfo;
class tmlnDriver;


//============================================================================
//============================================================================
class tmlnChannelSet
{
public:
	//--------------------------------------------------------------------
	// destructor - deletes channels in set.
	//--------------------------------------------------------------------
	virtual ~tmlnChannelSet();

	//--------------------------------------------------------------------
	//  Add Channel with given name. This class owns the channel,
	// but returns a pointer to it for convenience
	//--------------------------------------------------------------------
	tmlnChannel * AddChannel(const char *i_Name);

	//--------------------------------------------------------------------
	//	Adds channel to channel set, taking ownership of the channel
	//--------------------------------------------------------------------
	void AddChannel(tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	//	Removes channel, ownership passes back to caller
	//--------------------------------------------------------------------
	void RemoveChannel(tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	//  return number of channels in set
	//--------------------------------------------------------------------
	int  GetNumChannels() const;

	//--------------------------------------------------------------------
	//  Accessor to channel by index
	//--------------------------------------------------------------------
	tmlnChannel& Channel(int i_Index);

	//--------------------------------------------------------------------
	// GetChannel
	//--------------------------------------------------------------------
	const tmlnChannel& GetChannel(int i_Index) const;

	//--------------------------------------------------------------------
	// GetChannel - returns the index.  if not found it returns -1
	//--------------------------------------------------------------------
	int GetChannel(const char *i_Name) const;

	//--------------------------------------------------------------------
	// Unhook this drivers from all channels
	//--------------------------------------------------------------------
	void UnhookDriver(tmlnDriver* i_pDriver);

	//--------------------------------------------------------------------
	//  Update drivers in channels for given time. If a driver needs 
	//  to be delayed to the end, it will be added to the delayed 
	//  drivers list passed in.
	//--------------------------------------------------------------------
	void  Update(float i_Time,
				 std::list<tmlnDriver*> &o_DelayedDrivers);

	//--------------------------------------------------------------------
	//  Update drivers in channels for given time
	//--------------------------------------------------------------------
	void  Update(float i_Time);

	//--------------------------------------------------------------------
	// Set channel info from a map of names of channels to info structures
	//--------------------------------------------------------------------
	void SetChannelInfo(const std::map<std::string, tmlnChannelInfo> &i_ChannelInfoMap);

	//--------------------------------------------------------------------
	// Get channel info as a map of names of channels to info structures
	//--------------------------------------------------------------------
	void GetChannelInfo(std::map<std::string, tmlnChannelInfo> &o_ChannelInfoMap) const;

private:
	std::vector<tmlnChannel*> m_Channels;
};
