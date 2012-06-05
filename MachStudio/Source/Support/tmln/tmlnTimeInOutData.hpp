/********************************************************************************************\
**  tmlnTimeInOutData.hpp
**
**		Data structures and types for time in (start) and out (stop).
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_TIMEINOUTDATA_HPP
#error tmlnTimeInOutData.hpp multiply included
#endif
#define TMLN_TIMEINOUTDATA_HPP

#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 

#include <map>
#include <string>
#include <vector>


//============================================================================
//
//============================================================================
class tmlnTimeInOutData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnTimeInOutData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnTimeInOutData(const maTime& i_fIn, const maTime& i_fOut);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const tmlnTimeInOutData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnTimeInOutData& operator = (const tmlnTimeInOutData& i_Data);

	//------------------------------------------------------------------------
	//	returns if the time is after this block
	//------------------------------------------------------------------------
	bool IsTimeAfter( const maTime& i_fTime ) const;

	//------------------------------------------------------------------------
	//	returns if the time is within (or equal to) any in / out time block.
	//------------------------------------------------------------------------
	bool IsTimeWithin( const maTime& i_fTime ) const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maTime GetStartTime();
	maTime GetTotalTime();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float GetPercentage(const maTime& i_fCurrentTime);

public:
	maTime m_fInTime;
	maTime m_fOutTime;
};


//============================================================================
//============================================================================
class tmlnTimeInOutDataList
{	
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnTimeInOutDataList();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddTimeInOut( const maTime& i_fIn, const maTime& i_fOut );

	//------------------------------------------------------------------------
	//	check if name matches
	//------------------------------------------------------------------------
	tmlnTimeInOutDataList& operator == (const tmlnTimeInOutDataList& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maTime GetTotalTime();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float GetPercentage(const maTime& i_fCurrentTime);

	//------------------------------------------------------------------------
	//	returns if the time is within (or equal to) any in / out time block.
	//------------------------------------------------------------------------
	bool IsTimeWithin( const maTime& i_fTime, bool i_bEmptyListReturnTrue = false ) const;

	//------------------------------------------------------------------------
	//	returns true if within a block and returns the start + end time
	//	if the time isn't in a block the function returns false and the
	//	start time will be 0 and the end time will be -1.
	//------------------------------------------------------------------------
	bool GetTimesIfWithin( const maTime& i_fTime, maTime& o_fStartTime, maTime& o_fEndTime, bool i_bEmptyListReturnTrue = false) const;

	//------------------------------------------------------------------------
	//	returns true ONLY if both times are valid within the same block
	//------------------------------------------------------------------------
	bool AreTimesValidWithinSameBlock( const maTime& i_fTime1, const maTime& i_fTime2 );

	//------------------------------------------------------------------------
	//	find the next valid capture time at or after the passed in time
	//------------------------------------------------------------------------
	maTime GetNextCaptureTime( const maTime& i_fStartCheckTime );

	//------------------------------------------------------------------------
	//	clear the data in the list
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//	the completed flag in the list
	//------------------------------------------------------------------------
	void SetCompletedFlag();
	void ClearCompletedFlag();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Debug_DisplayData() const;

public:
	//
	//	data
	//
	std::vector<tmlnTimeInOutData> m_List;
	bool m_bCompleted;
};


//============================================================================
//	typedefs
//============================================================================
typedef const char *TimeInOutGroup;				 // Use strings to group types
typedef std::map<std::string, tmlnTimeInOutDataList> TimeInOutLists;


//============================================================================
//============================================================================
class tmlnTimeInOutDataLists
{	
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnTimeInOutDataLists();

	//------------------------------------------------------------------------
	//	clear all the data in all the lists
	//------------------------------------------------------------------------
	void ClearLists();

	//------------------------------------------------------------------------
	//	find the DataList and return true if name matches
	//------------------------------------------------------------------------
	bool GetDataList(const std::string& i_Name, tmlnTimeInOutDataList& o_DataList);

	//------------------------------------------------------------------------
	//	add in and out time to a list
	//------------------------------------------------------------------------
	void AddTimeInOut(const std::string& i_Name, const maTime& i_fIn, const maTime& i_fOut);

	//------------------------------------------------------------------------
	//	clear the completed flag in the lists
	//------------------------------------------------------------------------
	void ClearCompletedFlags();

public:
	//
	//	data
	//
	TimeInOutLists m_InOutLists;
};


