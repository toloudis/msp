/*****************************************************************************
**	mcpRealTimeConnect.cpp
**
**	Utilities for connecting to EVaRT real-time motion capture streaming.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "mcpRealTimeConnect.hpp"

#if (EVART_SDK == 1)
#include "mcpEVaRTConnectSDK1.hpp"
#elif (EVART_SDK == 2)
#include "mcpEVaRTConnectSDK2.hpp"
#endif


namespace mcpRealTimeConnect
{
	//--------------------------------------------------------------------
	//	Returns version of EVaRT SDK being used
	//--------------------------------------------------------------------
	int GetSDKVersion()
	{
#if (EVART_SDK == 1)
		return 1;
#elif (EVART_SDK == 2)
		return 2;
#else
		return 0;
#endif
	}

	//--------------------------------------------------------------------
	// Init - SDK2 needs name of localhost
	//--------------------------------------------------------------------
	void  Init(const char* i_LocalHostname)
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::Init();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::Init(i_LocalHostname);
#endif
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::CleanUp();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::CleanUp();
#endif
	}

	//--------------------------------------------------------------------
	//  Connect to EVaRT SDK on given computer name
	//--------------------------------------------------------------------
	bool  Connect(const char* i_Hostname)
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::Connect(i_Hostname);
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::Connect(i_Hostname);
#else
		return false;
#endif
	}

	//--------------------------------------------------------------------
	//  Disconnect from EVaRT SDK
	//--------------------------------------------------------------------
	void  Disconnect()
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::Disconnect();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::Disconnect();
#endif
	}

	//--------------------------------------------------------------------
	//	Returns true if it is connected.
	//--------------------------------------------------------------------
	bool IsConnected()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::IsConnected();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::IsConnected();
#else
		return false;
#endif
	}

	//--------------------------------------------------------------------
	// Set scale factor to apply to skeletal data. Compensates
	//	for different units.
	//--------------------------------------------------------------------
	void SetScaleFactor(float i_Scale)
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::SetScaleFactor(i_Scale);
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::SetScaleFactor(i_Scale);
#endif
	}
	float GetScaleFactor()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::GetScaleFactor();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::GetScaleFactor();
#else
		return 1.0f;
#endif
	}
	
	//--------------------------------------------------------------------
	//	Request hierarchy for EVaRT. The hierarchy will be return
	//	asynchronously, so you have to poll HaveNewHierarchy to see
	//	when it arrives.
	//--------------------------------------------------------------------
	void RequestHierarchy()
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::RequestHierarchy();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::RequestHierarchy();
#endif
	}

	//--------------------------------------------------------------------
	//	Return whether we have a new hierarchy for EVaRT.
	//	This will only return true once after each hierarchy change;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewHierarchy()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::HaveNewHierarchy();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::HaveNewHierarchy();
#else
		return false;
#endif
	}

	
	//--------------------------------------------------------------------
	//	Request real-time frame updates for EVaRT. The data will be 
	//	returned asynchronously, so you have to poll HaveNewFrameData 
	//	to see when it arrives.
	//--------------------------------------------------------------------
	void RequestFrameData()
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::RequestFrameData();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::RequestFrameData();
#endif
	}

	//--------------------------------------------------------------------
	// Stop the stream of data started from RequestFrameData
	//--------------------------------------------------------------------
	void StopStreaming()
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::StopStreaming();
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::StopStreaming();
#endif
	}

	//--------------------------------------------------------------------
	//	Returns true if it is streaming frame data at the moment.
	//--------------------------------------------------------------------
	bool IsStreaming()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::IsStreaming();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::IsStreaming();
#else
		return false;
#endif
	}

	//--------------------------------------------------------------------
	//	Return whether we have new real-time frame data for EVaRT.
	//	This will only return true once after each frame of animation;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewFrameData()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::HaveNewFrameData();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::HaveNewFrameData();
#else
		return false;
#endif
	}

	//--------------------------------------------------------------------
	// Return number of bodies. In SDK2, there can be more than one
	//	skeleton being tracked at a time.
	//--------------------------------------------------------------------
	int GetNumBodies()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::GetNumBodies();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::GetNumBodies();
#else
		return 0;
#endif
	}

	//--------------------------------------------------------------------
	// Return name of a body by index.
	//--------------------------------------------------------------------
	std::string GetBodyName(int i_Index)
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::GetBodyName(i_Index);
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::GetBodyName(i_Index);
#else
		return "";
#endif
	}

	//--------------------------------------------------------------------
	// Return number of segments. Just for quick polling. Note that
	//	between calls to GetNumSegments and GetSegmentData, things
	//	might have changed.
	//--------------------------------------------------------------------
	int GetNumSegments()
	{
#if (EVART_SDK == 1)
		return mcpEVaRTConnectSDK1::GetNumSegments();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::GetNumSegments();
#else
		return 0;
#endif
	}
	int GetNumSegments(const std::string &i_BodyName)
	{
#if (EVART_SDK == 1)
		// No real body names in SDK1
		return mcpEVaRTConnectSDK1::GetNumSegments();
#elif (EVART_SDK == 2)
		return mcpEVaRTConnectSDK2::GetNumSegments(i_BodyName);
#else
		return 0;
#endif
	}

	//--------------------------------------------------------------------
	// Get copy of the current segment data 
	//--------------------------------------------------------------------
	void GetSegmentData(std::vector<mcpHTRSegmentData> &o_Segments)
	{
#if (EVART_SDK == 1)
		mcpEVaRTConnectSDK1::GetSegmentData(o_Segments);
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::GetSegmentData(o_Segments);
#endif
	}
	void GetSegmentData(const std::string &i_BodyName,
						std::vector<mcpHTRSegmentData> &o_Segments)
	{
#if (EVART_SDK == 1)
		// No real body names in SDK1
		mcpEVaRTConnectSDK1::GetSegmentData(o_Segments);
#elif (EVART_SDK == 2)
		mcpEVaRTConnectSDK2::GetSegmentData(i_BodyName, o_Segments);
#endif
	}

}	// end of namespace
