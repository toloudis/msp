/*****************************************************************************
**	chnlMarkerMgr.hpp
**
**	Handles the marker data
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_MARKERMGR_HPP
#error chnlMarkerMgr.hpp multiply included
#endif
#define CHNL_MARKERMGR_HPP

#ifndef CHNL_TIMEDATA_HPP
#include "Features/Channels/Data/chnlTimeData.hpp"
#endif

#include <set>


//============================================================================
//============================================================================
class tmlnDriver;
class tmlnScriptObject;


//============================================================================
//============================================================================
namespace chnlMarkerMgr
{
	//------------------------------------------------------------------------
	//	Get the marker in and out time
	//------------------------------------------------------------------------
	float GetMarkerInTime();
	float GetMarkerOutTime();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddMarker(float i_Time, 
		int i_Type = e_MarkerType_Normal, 
		const std::string& i_Note = "");

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteMarker(float i_Time);

	//------------------------------------------------------------------------
	// Return number of markers
	//------------------------------------------------------------------------
	int GetNumMarkers();

	//------------------------------------------------------------------------
	// Accessors to data
	//------------------------------------------------------------------------
	const chnlMarkerDataItem& GetMarkerData(int i_Index);
	void SetMarkerData(int i_Index, const chnlMarkerDataItem &i_Data);

	//------------------------------------------------------------------------
	// Get and set marker data as a whole
	//------------------------------------------------------------------------
	void GetData(std::vector<chnlMarkerDataItem>&	o_Markers);
	void SetData(const std::vector<chnlMarkerDataItem>& i_Markers);

	//------------------------------------------------------------------------
	// SnapTimeToMarkers - see if given time is within snap threshold
	//	of the a marker. If so, return true and the index or 
	//	time for the marker.
	//------------------------------------------------------------------------
	bool SnapTimeToMarkers(float i_Time, int& o_MarkerIndex);
	bool SnapTimeToMarkers(float i_Time, float& o_MarkerTime);
	
	//------------------------------------------------------------------------
	// Get nearest marker to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	float GetNextMarkerTime( float i_Time );
	float GetPrevMarkerTime( float i_Time );

}	// end of namespace
