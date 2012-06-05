/*****************************************************************************
**	chnlMarkerMgr.cpp
**
**	Handles the marker data
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"

#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/Data/chnlTimeData.hpp"

#include "Core/Dbg/dbgAssert.hpp"

#include <algorithm>
#include <math.h>



//============================================================================
//============================================================================
namespace chnlMarkerMgr
{
	namespace
	{
		std::vector<chnlMarkerDataItem>	l_Markers;

		//	
		struct time_match
		{
			time_match(float i_Time) : m_Time(i_Time) {}
			bool operator()(const chnlMarkerDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			float m_Time;
		};

	}	// end of namespace

	//------------------------------------------------------------------------
	//	Get the marker in and out time
	//------------------------------------------------------------------------
	float GetMarkerInTime()
	{
		for (int i = 0; i < l_Markers.size(); ++i)
		{
			if (l_Markers[i].m_TimeMarkerType == 1)			// 1 = in
				return l_Markers[i].m_Time.GetValue();
		}
		return 0.0f;
	}
	float GetMarkerOutTime()
	{
		for (int i = 0; i < l_Markers.size(); ++i)
		{
			if (l_Markers[i].m_TimeMarkerType == 2)			// 2 = out
				return l_Markers[i].m_Time.GetValue();
		}
		return -1.0f;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddMarker(float i_Time, int i_Type, const std::string& i_Note)
	{
		chnlMarkerDataItem item;
		item.m_Time = i_Time;
		item.m_TimeMarkerType = i_Type;
		item.m_Note = i_Note;
		l_Markers.push_back( item );
	
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteMarker(float i_Time)
	{
		std::vector<chnlMarkerDataItem> &markers = l_Markers;
		std::vector<chnlMarkerDataItem>::iterator it =
			std::find_if(markers.begin(), markers.end(), time_match(i_Time));
		if (it != markers.end())
		{
			markers.erase( it );
		}	
	
	}

	//------------------------------------------------------------------------
	// Return number of markers
	//------------------------------------------------------------------------
	int GetNumMarkers()
	{
		return l_Markers.size();
	}

	//------------------------------------------------------------------------
	// Get and Set individual marker data items
	//------------------------------------------------------------------------
	const chnlMarkerDataItem& GetMarkerData(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0) && (i_Index < l_Markers.size()), "Index out of range to GetMarkerData call" );

		return l_Markers[i_Index];
	}
	void SetMarkerData(int i_Index, const chnlMarkerDataItem &i_Data)
	{
		DBG_ASSERT0( (i_Index >= 0) && (i_Index < l_Markers.size()), "Index out of range to SetMarkerData call" );

		l_Markers[i_Index] = i_Data;
	}

	//------------------------------------------------------------------------
	// Get and set marker data as a whole
	//------------------------------------------------------------------------
	void GetData(std::vector<chnlMarkerDataItem>&	o_Markers)
	{
		o_Markers = l_Markers;
	}
	void SetData(const std::vector<chnlMarkerDataItem>& i_Markers)
	{
		l_Markers = i_Markers;
	}

	//------------------------------------------------------------------------
	// SnapTimeToMarkers - see if given time is within snap threshold
	//	of the a marker. If so, return true and the time for the marker.
	//------------------------------------------------------------------------
	bool SnapTimeToMarkers(float i_Time, int& o_MarkerIndex)
	{
		bool bFound = false;
		float mtime, closest;
		for (int i = 0; i < l_Markers.size(); ++i)
		{
			mtime = l_Markers[i].m_Time.GetValue();
			if (chnlSnapUtil::TimesMatch(i_Time, mtime))
			{
				// Lost for closest match
				float diff = ::fabsf(mtime - i_Time);
				if (!bFound || (diff < closest))
				{
					closest = diff;
					o_MarkerIndex = i;
					bFound = true;
				}
			}
		}
		return bFound;
	}
	bool SnapTimeToMarkers(float i_Time, float& o_MarkerTime)
	{
		int marker_index = 0;
		if (SnapTimeToMarkers(i_Time, marker_index))
		{
			o_MarkerTime = l_Markers[marker_index].m_Time.GetValue();
			return true;
		}
		return false;
	}

	
	//------------------------------------------------------------------------
	// Get nearest marker to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	float GetNextMarkerTime( float i_Time )
	{
		if ( l_Markers.empty() ) return -1;

		float best_time = i_Time;
		bool bFound = false;
		int i;
		for (i = 0; i < l_Markers.size(); ++i)
		{
			if (i_Time < l_Markers[i].m_Time.GetValue())
			{
				//	if the time is the lesser time then store it.
				if ( !bFound || best_time > l_Markers[i].m_Time.GetValue() )
				{
					bFound = true;
					best_time = l_Markers[i].m_Time.GetValue();
				}
			}
		}
		if (bFound)
			return best_time;
		else
			return -1;
	}
	float GetPrevMarkerTime( float i_Time )
	{
		if ( l_Markers.empty() ) return -1;

		float best_time = i_Time;
		bool bFound = false;
		int i;
		for (i = 0; i < l_Markers.size(); ++i)
		{
			if (i_Time > l_Markers[i].m_Time.GetValue())
			{
				//	if the time is the greater time then store it.
				if ( !bFound || best_time < l_Markers[i].m_Time.GetValue() )
				{
					bFound = true;
					best_time = l_Markers[i].m_Time.GetValue();
				}
			}
		}
		if (bFound)
			return best_time;
		else
			return -1;
	}


}	// end of namespace
