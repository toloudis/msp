/*****************************************************************************
**	chnlMarkerOperations.hpp
**
**	Operations related to markers
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_MARKEROPERATIONS_HPP
#error chnlMarkerOperations.hpp multiply included
#endif
#define CHNL_MARKEROPERATIONS_HPP

#include <string>


//============================================================================
//============================================================================
class chnlMarkerDataItem;
class maTime;


//============================================================================
//============================================================================
namespace chnlMarkerOperations
{
	//--------------------------------------------------------------------
	// Create a new marker for the the channels timeline
	//--------------------------------------------------------------------
	void AddMarker(); 
	void AddMarker(const maTime& i_Time, 
				   int &i_Type, 
				   const std::string &i_Note);

	//--------------------------------------------------------------------
	// Delete a marker by time or frame
	//--------------------------------------------------------------------
	void DeleteMarker();
	void DeleteMarker(const maTime& i_Time);
	void DeleteMarkerAtFrame(int i_Frame);

	//--------------------------------------------------------------------
	// Move current time to next or previous marker
	//--------------------------------------------------------------------
	void MoveToNextMarker();
	void MoveToPrevMarker();

	//--------------------------------------------------------------------
	// Set data for given marker by index
	//--------------------------------------------------------------------
	void SetMarkerData(int i_Index, const chnlMarkerDataItem &i_Data);

	//--------------------------------------------------------------------
	// Show Dialog to edit properties of marker at given time
	//--------------------------------------------------------------------
	void ShowProperties(const maTime& i_Time);

}	// end of namespace
