/*****************************************************************************
**	chnlMarkerMgr.hpp
**
**	Handles the marker data
**
**	StudioGPU
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
	maTime GetMarkerInTime();
	maTime GetMarkerOutTime();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool AddMarker(const maTime& i_Time, 
		int i_Type = e_MarkerType_Normal, 
		const std::string& i_Note = "");

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteMarker(const maTime& i_Time);

	//------------------------------------------------------------------------
	// Check if there is a marker existed at a given time
	//------------------------------------------------------------------------
	bool IsMarkerExisted(const maTime& i_Time);

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
	// Get Maker index at the given time
	//------------------------------------------------------------------------
	int GetMarkerIndexAtTime(const maTime& i_Time);

	//------------------------------------------------------------------------
	// Get maker array at the given frame
	// There might be multiple markers found
	//------------------------------------------------------------------------
	void GetMarkersAtFrame(int i_Frame, std::vector<chnlMarkerDataItem>& o_Markers);

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
	bool SnapTimeToMarkers(const maTime& i_Time, int& o_MarkerIndex);
	bool SnapTimeToMarkers(const maTime& i_Time, maTime& o_MarkerTime);
	
	//------------------------------------------------------------------------
	// Get nearest marker to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	maTime GetNextMarkerTime( const maTime& i_Time );
	maTime GetPrevMarkerTime( const maTime& i_Time );

}	// end of namespace
