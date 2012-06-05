/****************************************************************************\
**  tmlnTimeInOutData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeInOutData.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <iomanip>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnTimeInOutData::tmlnTimeInOutData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnTimeInOutData::tmlnTimeInOutData(const maTime& i_fIn, const maTime& i_fOut) 
:	m_fInTime(i_fIn),
	m_fOutTime(i_fOut)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool tmlnTimeInOutData::operator == (const tmlnTimeInOutData& i_Item)
{
	return ((m_fInTime == i_Item.m_fInTime)&&(m_fOutTime == i_Item.m_fOutTime));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnTimeInOutData& tmlnTimeInOutData::operator=(const tmlnTimeInOutData& i_Data)
{
	if (this == &i_Data) return *this;
	m_fInTime = i_Data.m_fInTime;
	m_fOutTime = i_Data.m_fOutTime;
	return *this;
}

//------------------------------------------------------------------------
//	returns if the time is within (or equal to) the in / out time
//------------------------------------------------------------------------
bool tmlnTimeInOutData::IsTimeWithin( const maTime& i_fTime ) const
{
	//DBG_LOG3( "Checking %6.3f < %6.3f < %6.3f", m_fInTime, i_fTime, m_fOutTime );

	return ((i_fTime >= m_fInTime) && (i_fTime <= m_fOutTime));
}

//------------------------------------------------------------------------
//	returns if the time is after this block
//------------------------------------------------------------------------
bool tmlnTimeInOutData::IsTimeAfter( const maTime& i_fTime ) const
{
	return (i_fTime > m_fOutTime);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
maTime tmlnTimeInOutData::GetStartTime()
{
	return m_fInTime;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
maTime tmlnTimeInOutData::GetTotalTime()
{
	return (m_fOutTime - m_fInTime);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float tmlnTimeInOutData::GetPercentage(const maTime& i_fCurrentTime)
{
	maTime length = (m_fOutTime - m_fInTime);
	if (i_fCurrentTime < m_fInTime)
	{
		return 0.0f;
	}
	else if (i_fCurrentTime > m_fOutTime)
	{
		return 1.0f;
	}
	else /* within this block */
	{
		return ((i_fCurrentTime-m_fInTime) / length);
	}
}



//
//	List
//

//------------------------------------------------------------------------
//------------------------------------------------------------------------
tmlnTimeInOutDataList::tmlnTimeInOutDataList()
:	m_bCompleted(false)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void tmlnTimeInOutDataList::AddTimeInOut( const maTime& i_fIn, const maTime& i_fOut )
{
	std::vector<tmlnTimeInOutData>::iterator it;
    for (it = m_List.begin(); it != m_List.end(); it++)
	{
		if (i_fIn < (*it).m_fInTime)
		{
			m_List.insert(it, tmlnTimeInOutData(i_fIn, i_fOut));
			return;
		}
	}
	m_List.push_back(tmlnTimeInOutData(i_fIn, i_fOut));
	//DBG_LOG3("   added time %6.3f to %6.3f size(%d)", m_List[index].m_fInTime, m_List[index].m_fOutTime, m_List.size());
}

//------------------------------------------------------------------------
//	check if name matches
//------------------------------------------------------------------------
tmlnTimeInOutDataList& tmlnTimeInOutDataList::operator == (const tmlnTimeInOutDataList& i_Data)
{
	if (this == &i_Data) return *this;
	*this = i_Data;
	return *this;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
maTime tmlnTimeInOutDataList::GetTotalTime()
{
	maTime total_time = maTime::c_ZeroTime;
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		total_time += m_List[i].GetTotalTime();
	}
	return total_time;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float tmlnTimeInOutDataList::GetPercentage(const maTime& i_fCurrentTime)
{
	float finished_time = 0.0f;
	float total_time = 0.0f;
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		float tt = m_List[i].GetTotalTime().AsSeconds();
		float pct = m_List[i].GetPercentage( i_fCurrentTime );
		finished_time +=  tt * pct;
		total_time += tt;
	}
	if (total_time == 0.0f)
	{
		return (0.0f);
	}
	else
	{
		float percentage = (finished_time / total_time);
		return percentage;
	}
}

//------------------------------------------------------------------------
//	returns if the time is within (or equal to) any in / out time block.
//------------------------------------------------------------------------
bool tmlnTimeInOutDataList::IsTimeWithin( const maTime& i_fTime, bool i_bEmptyListReturnTrue ) const
{
	bool bEmptyList = (m_List.size() == 0);
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		if (m_List[i].IsTimeWithin( i_fTime ))
		{
			return true;
		}
	}

	//	if it isn't within but the flag is true return true anyway
	//
	if ( bEmptyList )
		return i_bEmptyListReturnTrue;
	else
		return false;
}

//------------------------------------------------------------------------
//	returns true if within a block and returns the start + end time
//	if the time isn't in a block the function returns false and the
//	start time will be 0 and the end time will be -1.
//------------------------------------------------------------------------
bool tmlnTimeInOutDataList::GetTimesIfWithin( const maTime& i_fTime, maTime& o_fStartTime, maTime& o_fEndTime, bool i_bEmptyListReturnTrue) const
{
	bool bEmptyList = (m_List.size() == 0);
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		if (m_List[i].IsTimeWithin( i_fTime ))
		{
			o_fStartTime = m_List[i].m_fInTime;
			o_fEndTime = m_List[i].m_fOutTime;
			return true;
		}
	}

	//TIME - If -1 is a special value, then have to be careful about units. Maybe use a class static const?
	o_fStartTime.SetSeconds( 0.0f );
	o_fEndTime.SetSeconds( -1.0f );
	return (bEmptyList && i_bEmptyListReturnTrue);
}

//------------------------------------------------------------------------
//	returns true ONLY if both times are valid within the same block
//------------------------------------------------------------------------
bool tmlnTimeInOutDataList::AreTimesValidWithinSameBlock( const maTime& i_fTime1, const maTime& i_fTime2 )
{
	bool bEmptyList = (m_List.size() == 0);
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		if (   (m_List[i].IsTimeWithin( i_fTime1 ))
			&& (m_List[i].IsTimeWithin( i_fTime2 )) )
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------
//	find the next valid capture time at or after the passed in time
//------------------------------------------------------------------------
maTime tmlnTimeInOutDataList::GetNextCaptureTime( const maTime& i_fStartCheckTime )
{
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		//	if the time is greater than the current block but less than
		//	the next block then return the start time of the next block
		//
		if (m_List[i].m_fInTime > i_fStartCheckTime)
		{
			return m_List[i].m_fInTime;
		}

		//	if within a time block return the passed in time
		//
		if (   (m_List[i].m_fInTime <= i_fStartCheckTime) 
			&& (m_List[i].m_fOutTime >= i_fStartCheckTime))
		{
			return i_fStartCheckTime;
		}
	}

	//TIME - magical time value without well defined units...
	return maTime::FromSeconds(-1.0f);
}

//------------------------------------------------------------------------
//	the completed flag in the list
//------------------------------------------------------------------------
void tmlnTimeInOutDataList::SetCompletedFlag()
{
	m_bCompleted = true;
}
void tmlnTimeInOutDataList::ClearCompletedFlag()
{
	m_bCompleted = false;
}

//------------------------------------------------------------------------
//	clear the data in the list
//------------------------------------------------------------------------
void tmlnTimeInOutDataList::Clear()
{
	m_List.clear();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void tmlnTimeInOutDataList::Debug_DisplayData() const
{
	for (int i = 0 ; i < m_List.size(); ++i)
	{
		std::string in_time_str, out_time_str;
		tmlnTimeUtil::GetTimeString(m_List[i].m_fInTime, in_time_str);
		tmlnTimeUtil::GetTimeString(m_List[i].m_fOutTime, out_time_str);
		DBG_LOG( std::setw(2) << i << ") " << std::setw(6) << std::setprecision(2) << in_time_str << " to " << out_time_str);
	}
}


//
//	Lists
//

//------------------------------------------------------------------------
//------------------------------------------------------------------------
tmlnTimeInOutDataLists::tmlnTimeInOutDataLists()
{
}

//------------------------------------------------------------------------
//	clear all the data in all the lists
//------------------------------------------------------------------------
void tmlnTimeInOutDataLists::ClearLists()
{
	TimeInOutLists::iterator it, end;
	end = m_InOutLists.end();
	for (it = m_InOutLists.begin(); it != end; ++it)
	{
		it->second.Clear();
	}
	m_InOutLists.clear();
}

//------------------------------------------------------------------------
//	find the data list and return true if name matches
//------------------------------------------------------------------------
bool tmlnTimeInOutDataLists::GetDataList(const std::string& i_Name,tmlnTimeInOutDataList& o_DataList)
{
	TimeInOutLists::const_iterator it = m_InOutLists.find(i_Name);
	if (it != m_InOutLists.end())
	{
		o_DataList = (it->second);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
//	add in and out time to a list
//------------------------------------------------------------------------
void tmlnTimeInOutDataLists::AddTimeInOut(const std::string& i_Name, const maTime& i_fIn, const maTime& i_fOut)
{
	//TimeInOutLists::iterator it = m_InOutLists.find(i_Name);
	//if (it != m_InOutLists.end())
	//{
	//	(it->second).AddTimeInOut(i_fIn, i_fOut);
	//}
	//else
	{
		//DBG_LOG3("adding time %s) %6.3f to %6.3f", i_Name.c_str(), i_fIn, i_fOut);
		m_InOutLists[i_Name].AddTimeInOut(i_fIn, i_fOut);
	}
}


//------------------------------------------------------------------------
//	clear the completed flag in the lists
//------------------------------------------------------------------------
void tmlnTimeInOutDataLists::ClearCompletedFlags()
{
	TimeInOutLists::iterator it, end;
	end = m_InOutLists.end();
	for (it = m_InOutLists.begin(); it != end; ++it)
	{
		it->second.ClearCompletedFlag();
	}
}
