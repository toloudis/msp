/*****************************************************************************
**	mcpRealTimeConnect.hpp
**
**	Utilities for connecting to EVaRT real-time motion capture streaming.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MCP_REALTIMECONNECT_HPP
#error mcpRealTimeConnect.hpp multiply included
#endif
#define MCP_REALTIMECONNECT_HPP

#include <vector>

class mcpHTRSegmentData;

namespace mcpRealTimeConnect
{
	//--------------------------------------------------------------------
	//	Returns version of EVaRT SDK being used
	//--------------------------------------------------------------------
	int GetSDKVersion();

	//--------------------------------------------------------------------
	// Init - SDK2 needs name of localhost
	//--------------------------------------------------------------------
	void  Init(const char* i_LocalHostname = "localhost");

	//--------------------------------------------------------------------
	//  Clean up 
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	//  Connect to EVaRT SDK on given computer name
	//--------------------------------------------------------------------
	bool  Connect(const char* i_Hostname);

	//--------------------------------------------------------------------
	//  Disconnect from EVaRT SDK
	//--------------------------------------------------------------------
	void  Disconnect();

	//--------------------------------------------------------------------
	//	Returns true if it is connected.
	//--------------------------------------------------------------------
	bool IsConnected();

	//--------------------------------------------------------------------
	// Set scale factor to apply to skeletal data. Compensates
	//	for different units.
	//--------------------------------------------------------------------
	void SetScaleFactor(float i_Scale);
	float GetScaleFactor();
	
	//--------------------------------------------------------------------
	//	Request hierarchy for EVaRT. The hierarchy will be return
	//	asynchronously, so you have to poll HaveNewHierarchy to see
	//	when it arrives.
	//--------------------------------------------------------------------
	void RequestHierarchy();

	//--------------------------------------------------------------------
	//	Return whether we have a new hierarchy for EVaRT.
	//	This will only return true once after each hierarchy change;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewHierarchy();

	//--------------------------------------------------------------------
	//	Request real-time frame updates for EVaRT. The data will be 
	//	returned asynchronously, so you have to poll HaveNewFrameData 
	//	to see when it arrives.
	//--------------------------------------------------------------------
	void RequestFrameData();

	//--------------------------------------------------------------------
	// Stop the stream of data started from RequestFrameData
	//--------------------------------------------------------------------
	void StopStreaming();

	//--------------------------------------------------------------------
	//	Returns true if it is streaming frame data at the moment.
	//--------------------------------------------------------------------
	bool IsStreaming();

	//--------------------------------------------------------------------
	//	Return whether we have new real-time frame data for EVaRT.
	//	This will only return true once after each frame of animation;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewFrameData();

	//--------------------------------------------------------------------
	// Return number of bodies. In SDK2, there can be more than one
	//	skeleton being tracked at a time.
	//--------------------------------------------------------------------
	int GetNumBodies();

	//--------------------------------------------------------------------
	// Return name of a body by index.
	//--------------------------------------------------------------------
	std::string GetBodyName(int i_Index);

	//--------------------------------------------------------------------
	// Return number of segments. Just for quick polling. Note that
	//	between calls to GetNumSegments and GetSegmentData, things
	//	might have changed.
	//--------------------------------------------------------------------
	int GetNumSegments();
	int GetNumSegments(const std::string &i_BodyName);

	//--------------------------------------------------------------------
	// Get copy of the current segment data 
	//--------------------------------------------------------------------
	void GetSegmentData(std::vector<mcpHTRSegmentData> &o_Segments);
	void GetSegmentData(const std::string &i_BodyName,
						std::vector<mcpHTRSegmentData> &o_Segments);

}	// end of namespace
