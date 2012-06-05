/*****************************************************************************
**	mcpEVaRTConnectSDK1.cpp
**
**	Utilities for connecting to EVaRT real-time motion capture streaming.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#if (EVART_SDK == 1)

#include "mcpEVaRTConnectSDK1.hpp"

#include "dbgLog.hpp"
#include "maConstants.hpp"
#include "mcpHTRData.hpp"

//  EVaRT SDK headers
#include "EVaRT.h"

// In order to use the function TryEnterCriticalSection, I have to define the
// version of windows high enough (at least 0x0400). 
// 0x0400 = Windows NT 4.0
// 0x0500 = Windows 2000
#define _WIN32_WINNT 0x0400 

#include <windows.h>

namespace mcpEVaRTConnectSDK1
{

	namespace
	{
		// Used to synchronize threads
		CRITICAL_SECTION		l_CriticalSection;			// Windows critical section object

		// This array contains the hierarchy of the skeleton
		// communicated through the connection
		bool l_bHaveNewHierarchy = false;
		bool l_bHaveNewFrameData = false;
		std::vector<mcpHTRSegmentData> l_Segments;

		// Scale Factor to handle different units
		float l_UnitScaling = 0.01f; //0.1f;		// Realtime usually sends mm units

		// Callback function which is called by the EVaRT SDK thread
		// The void * pointer is a pointer to data in the EVaRT SDK library (dll)
		// DON'T WRITE TO THIS MEMORY!
		static int EVaRT_Data_Handler( int DataType, void *Data )
		{
			// Example of how you could protect global data in your main thread
			if ( TryEnterCriticalSection(&l_CriticalSection) == 0 )
			{
				return 0;
			}
			
			switch (DataType)
			{
			case HIERARCHY:
				{
					sHierarchy *p = (sHierarchy *) Data;

					const int count = p->nSegments;
					l_Segments.resize( count );
					for (int i=0; i<count; ++i)
					{
						l_Segments[i].m_Name = p->szSegmentNames[i];
						l_Segments[i].m_ParentIndex = p->iParents[i];
					}

					l_bHaveNewHierarchy = true;
				}
				break;
			case HTR2_DATA:
				{
					sHtr2Frame *p = (sHtr2Frame *) Data;
					
					// Transfer the data into our hierarchy of segments
					// so that we are always tracking the latest position
					//
					const int count = l_Segments.size();
					for (int i=0; i<count; ++i)
					{
						mcpHTRSegmentData &segment = l_Segments[i];

						/*if (i==0)
						{
							DBG_LOG3("Position: %f, %f, %f", p->Segments[i][0], p->Segments[i][1], p->Segments[i][2]);
							DBG_LOG3("Rotation: %f, %f, %f", p->Segments[i][3], p->Segments[i][4], p->Segments[i][5]);
							DBG_LOG1("Bone Length: %f", p->Segments[i][6]);
						}*/

						// First 3 float values are translation. 
						//	Apply scaling for unit differences.
						segment.m_Position.Set(p->Segments[i][0] * l_UnitScaling, 
											   p->Segments[i][1] * l_UnitScaling, 
											   p->Segments[i][2] * l_UnitScaling);

						// Next 3 are rotation info...
						// Assuming rotation order ZYX with degrees
						maRotation rotx(maVector3d(1,0,0), p->Segments[i][3] * maConstants::c_fAngleToRad);
						maRotation roty(maVector3d(0,1,0), p->Segments[i][4] * maConstants::c_fAngleToRad);
						maRotation rotz(maVector3d(0,0,1), p->Segments[i][5] * maConstants::c_fAngleToRad);
						//segment.m_Rotation = (rotx * roty * rotz);
						segment.m_Rotation = (rotz * roty * rotx);

						// Scale (although this is length of the bone, which isn't
						//	really the scale factor we would need).
						// To do scale animation in real-time, we would need
						//	to compare the original bone length to the current value.
						segment.m_BoneLength = p->Segments[i][6] * l_UnitScaling;
					}

					l_bHaveNewFrameData = true;
				}
				break;
			}

			LeaveCriticalSection(&l_CriticalSection);
			return 0;
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		// Initialize the Windows critical section object
		InitializeCriticalSection(&l_CriticalSection);

		// Initialize EVaRT SDK, only call this function once
		EVaRT_Initialize();

		// Tell EVaRT what function to call when it has something to send us
		// The callback function is called by the EVaRT SDK thread, not our main thread
		EVaRT_SetDataHandlerFunc( EVaRT_Data_Handler );
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		// Make sure we are disconnected
		Disconnect();

		// Shutdown EVaRT SDK, only call this once
		EVaRT_Exit();

		DeleteCriticalSection(&l_CriticalSection);
	}

	//--------------------------------------------------------------------
	//  Connect to EVaRT SDK on given computer name
	//--------------------------------------------------------------------
	bool  Connect(const char* i_Hostname)
	{
		// For some reason the SDK doesn't take a const char*
		if (EVaRT_Connect(const_cast<char*>(i_Hostname)) == OK)
		{
			DBG_LOG1("Connected to host: %s", i_Hostname);

			// Make sure we're connected
			return (EVaRT_IsConnected() != 0);
		}
		else
		{
			DBG_LOG1("Could not connect to host: %s", i_Hostname);
		}
		return false;
	}

	//--------------------------------------------------------------------
	//  Disconnect from EVaRT SDK
	//--------------------------------------------------------------------
	void  Disconnect()
	{

		// If connected, then disconnect
		if (EVaRT_IsConnected())
		{
			// Stop any active streaming
			if (IsStreaming())
			{
				StopStreaming();
			}

			DBG_LOG0("Disconnecting from EVaRT");

			// Disconnect from EVaRT
			EVaRT_Disconnect();
		}
	}

	//--------------------------------------------------------------------
	//	Returns true if it is connected.
	//--------------------------------------------------------------------
	bool IsConnected()
	{
		return (EVaRT_IsConnected() != 0);
	}

	//--------------------------------------------------------------------
	// Set scale factor to apply to skeletal data. Compensates
	//	for different units.
	//--------------------------------------------------------------------
	void SetScaleFactor(float i_Scale)
	{
		l_UnitScaling = i_Scale;
	}
	float GetScaleFactor()
	{
		return l_UnitScaling;
	}
	
	//--------------------------------------------------------------------
	//	Request hierarchy for EVaRT. The hierarchy will be return
	//	asynchronously, so you have to poll HaveNewHierarchy to see
	//	when it arrives.
	//--------------------------------------------------------------------
	void RequestHierarchy()
	{
		EnterCriticalSection(&l_CriticalSection);

		l_bHaveNewHierarchy = false;

		// The hierarchy is provided through our callback function
		// at some later point
		EVaRT_RequestHierarchy2();
			
		LeaveCriticalSection(&l_CriticalSection);
	}

	//--------------------------------------------------------------------
	//	Return whether we have a new hierarchy for EVaRT.
	//	This will only return true once after each hierarchy change;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewHierarchy()
	{
		if (l_bHaveNewHierarchy)
		{
			EnterCriticalSection(&l_CriticalSection);
			l_bHaveNewHierarchy = false;
			LeaveCriticalSection(&l_CriticalSection);

			return true;
		}
		return false;
	}

	
	//--------------------------------------------------------------------
	//	Request real-time frame updates for EVaRT. The data will be 
	//	returned asynchronously, so you have to poll HaveNewFrameData 
	//	to see when it arrives.
	//--------------------------------------------------------------------
	void RequestFrameData()
	{
		EnterCriticalSection(&l_CriticalSection);

		l_bHaveNewFrameData = false;

		// The frame data is provided through our callback function
		// at some later point
		EVaRT_SetDataTypesWanted(HTR2_DATA);

		EVaRT_StartStreaming();
			
		LeaveCriticalSection(&l_CriticalSection);
	}

	//--------------------------------------------------------------------
	// Stop the stream of data started from RequestFrameData
	//--------------------------------------------------------------------
	void StopStreaming()
	{
		// Make sure we're connected
		if (EVaRT_IsStreaming())
		{
			DBG_LOG0("Stopping streaming.");
			EVaRT_StopStreaming();
		}
	}

	//--------------------------------------------------------------------
	//	Returns true if it is streaming frame data at the moment.
	//--------------------------------------------------------------------
	bool IsStreaming()
	{
		return (EVaRT_IsStreaming() != 0);
	}

	//--------------------------------------------------------------------
	//	Return whether we have new real-time frame data for EVaRT.
	//	This will only return true once after each frame of animation;
	//	during this call it will clear the flag and subsequent calls 
	//	will return false.
	//--------------------------------------------------------------------
	bool HaveNewFrameData()
	{
		if (l_bHaveNewFrameData)
		{
			EnterCriticalSection(&l_CriticalSection);
			l_bHaveNewFrameData = false;
			LeaveCriticalSection(&l_CriticalSection);

			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Return number of bodies. In SDK2, there can be more than one
	//	skeleton being tracked at a time.
	//--------------------------------------------------------------------
	int GetNumBodies()
	{
		// Only can have one skeleton when using SDK1
		if (l_Segments.empty())
			return 0;
		else
			return 1;
	}

	//--------------------------------------------------------------------
	// Return name of a body by index.
	//--------------------------------------------------------------------
	std::string GetBodyName(int i_Index)
	{
		return "Default";
	}

	//--------------------------------------------------------------------
	// Return number of segments. Just for quick polling. Note that
	//	between calls to GetNumSegments and GetSegmentData, things
	//	might have changed.
	//--------------------------------------------------------------------
	int GetNumSegments()
	{
		// I don't think a critical section is needed here because
		// it is only reading the value
		int count = l_Segments.size();
		return count;
	}

	//--------------------------------------------------------------------
	// Get copy of the current segment data 
	//--------------------------------------------------------------------
	void GetSegmentData(std::vector<mcpHTRSegmentData> &o_Segments)
	{
		EnterCriticalSection(&l_CriticalSection);
		o_Segments = l_Segments;
		LeaveCriticalSection(&l_CriticalSection);
	}

}	// end of namespace

#endif // EVART_SDK == 1