/*****************************************************************************
**	mcpEVaRTConnectSDK2.cpp
**
**	Utilities for connecting to EVaRT real-time motion capture streaming.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#if (EVART_SDK == 2)

#include "mcpEVaRTConnectSDK2.hpp"

#include "dbgLog.hpp"
#include "maConstants.hpp"
#include "mcpHTRData.hpp"

//  EVaRT SDK headers
#include "EVaRT2.h"

// In order to use the function TryEnterCriticalSection, I have to define the
// version of windows high enough (at least 0x0400). 
// 0x0400 = Windows NT 4.0
// 0x0500 = Windows 2000
#define _WIN32_WINNT 0x0400 

#include <windows.h>

namespace mcpEVaRTConnectSDK2
{

	namespace
	{
		// Used to synchronize threads
		CRITICAL_SECTION		l_CriticalSection;			// Windows critical section object

		std::string l_LocalHostname("localhost");
		sHostInfo l_HostInfo;
		bool l_bConnected = false;
		bool l_bStreaming = false;

		// This array contains the hierarchy of the skeleton
		// communicated through the connection
		bool l_bHaveNewHierarchy = false;
		bool l_bHaveNewFrameData = false;

		struct sBody
		{
			std::string m_Name;
			std::vector<mcpHTRSegmentData> m_Segments;
		};
		std::vector<sBody> l_Bodies;

		// Scale Factor to handle different units
		float l_UnitScaling = 0.01f; //0.1f;		// Realtime usually sends mm units


		// Copy hierarchy information into our local copy
		void process_frame(sFrameOfData* data)
		{
			if ((data->nBodies > 0) &&
				(data->nBodies == l_Bodies.size()) )
			{
				for (int bi=0; bi<data->nBodies; ++bi)
				{
					sBodyData &body = data->BodyData[bi];
				
					const int count = l_Bodies[bi].m_Segments.size();
					if (body.nSegments == count)
					{
						for (int si=0; si<count; ++si)
						{
							mcpHTRSegmentData &segment = l_Bodies[bi].m_Segments[si];

							/*if (i==0)
							{
								DBG_LOG3("Position: %f, %f, %f", body.Segments[si][0], body.Segments[si][1], body.Segments[si][2]);
								DBG_LOG3("Rotation: %f, %f, %f", body.Segments[si][3], body.Segments[si][4], body.Segments[si][5]);
								DBG_LOG1("Bone Length: %f", body.Segments[si][6]);
							}*/

							// First 3 float values are translation. 
							//	Apply scaling for unit differences.
							segment.m_Position.Set(body.Segments[si][0] * l_UnitScaling, 
												body.Segments[si][1] * l_UnitScaling, 
												body.Segments[si][2] * l_UnitScaling);

							// Next 3 are rotation info...
							// Assuming rotation order ZYX with degrees
							maRotation rotx(maVector3d(1,0,0), body.Segments[si][3] * maConstants::c_fAngleToRad);
							maRotation roty(maVector3d(0,1,0), body.Segments[si][4] * maConstants::c_fAngleToRad);
							maRotation rotz(maVector3d(0,0,1), body.Segments[si][5] * maConstants::c_fAngleToRad);
							//segment.m_Rotation = (rotx * roty * rotz);
							segment.m_Rotation = (rotz * roty * rotx);

							// Scale (although this is length of the bone, which isn't
							//	really the scale factor we would need).
							// To do scale animation in real-time, we would need
							//	to compare the original bone length to the current value.
							segment.m_BoneLength = body.Segments[si][6] * l_UnitScaling;
						}
					}
				}
				l_bHaveNewFrameData = true;
			}
		}

		// Callback function which is called by the EVaRT SDK thread
		// The sFrameOfData* pointer is a pointer to data in the EVaRT SDK library (dll)
		// DON'T WRITE TO THIS MEMORY!
		static void __cdecl DataHandler(sFrameOfData* data)
		{
			if (!l_bStreaming)
			{
				// We receive data for alittle while after requesting
				//	a stop to the stream. So, check here for flag and 
				// ignore new data if necessary
				return;
			}

			// Example of how you could protect global data in your main thread
			if ( TryEnterCriticalSection(&l_CriticalSection) == 0 )
			{
				return;
			}

			process_frame(data);

			LeaveCriticalSection(&l_CriticalSection);
		}


		int get_index_for_body(const std::string &i_Name)
		{
			for (int i=0; i<l_Bodies.size(); i++)
			{
				if (l_Bodies[i].m_Name == i_Name)
					return i;
			}
			return -1;
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init - needs name of local computer
	//--------------------------------------------------------------------
	void  Init(const char* i_LocalHostname)
	{
		// Initialize the Windows critical section object
		InitializeCriticalSection(&l_CriticalSection);

		// Store local hostname to use during Connect call
		l_LocalHostname = i_LocalHostname;
		
		// Set up our data callback
		EVaRT2_SetDataHandlerFunc( DataHandler );
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		// Make sure we are disconnected
		Disconnect();

		DeleteCriticalSection(&l_CriticalSection);
	}

	//--------------------------------------------------------------------
	//  Connect to EVaRT SDK on given computer name
	//--------------------------------------------------------------------
	bool  Connect(const char* i_Hostname)
	{
		// For some reason the SDK doesn't take a const char*
		int rc = EVaRT2_Initialize(const_cast<char*>(l_LocalHostname.c_str()), 
								   const_cast<char*>(i_Hostname));
		if (rc == RC_Okay)
		{
			l_bConnected = true;

			// zero the host info structure and query EVaRT
			memset(&l_HostInfo, 0, sizeof(l_HostInfo));
			// query back the host info
			EVaRT2_GetHostInfo(&l_HostInfo);

			return true;
		}
		else
		{
			DBG_LOG2("Could not connect (SDK2) to host: %s from client: %s", i_Hostname, l_LocalHostname.c_str());
		}
		return false;
	}

	//--------------------------------------------------------------------
	//  Disconnect from EVaRT SDK
	//--------------------------------------------------------------------
	void  Disconnect()
	{
		// If connected, then disconnect
		if (IsConnected())
		{
			// Stop any active streaming
			if (IsStreaming())
			{
				StopStreaming();
			}

			DBG_LOG0("Disconnecting from EVaRT");
			l_bConnected = false;

			// Disconnect from EVaRT
			EVaRT2_Exit();
		}
	}

	//--------------------------------------------------------------------
	//	Returns true if it is connected.
	//--------------------------------------------------------------------
	bool IsConnected()
	{
		return l_bConnected;
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
		sBodyDefs* body_defs =  EVaRT2_GetBodyDefs();
		if (body_defs)
		{
			DBG_LOG1("Num body defs: %d", body_defs->nBodyDefs);

			if (body_defs->nBodyDefs > 0)
			{
				l_Bodies.resize(body_defs->nBodyDefs);

				for (int bi=0; bi<body_defs->nBodyDefs; ++bi)
				{
					l_Bodies[bi].m_Name = body_defs->BodyDefs[bi].szName;

					sHierarchy &hier = body_defs->BodyDefs[bi].Hierarchy;
					const int count = hier.nSegments;
					l_Bodies[bi].m_Segments.resize( count );
					for (int si=0; si<count; ++si)
					{
						l_Bodies[bi].m_Segments[si].m_Name = hier.szSegmentNames[si];
						l_Bodies[bi].m_Segments[si].m_ParentIndex = hier.iParents[si];
					}
				}
			}
			EVaRT2_FreeBodyDefs(body_defs);
			l_bHaveNewHierarchy = true;
		}

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
			l_bHaveNewHierarchy = false;
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

		if (IsConnected())
		{
			sFrameOfData* src = EVaRT2_GetCurrentFrame();
			if (src)
			{
				process_frame(src);
			}

			void* response;
			int nBytes;
			if (EVaRT2_Request("LiveMode", &response, &nBytes) == RC_Okay)
			{
				l_bStreaming = true;
			}
		}
			
		LeaveCriticalSection(&l_CriticalSection);
	}

	//--------------------------------------------------------------------
	// Stop the stream of data started from RequestFrameData
	//--------------------------------------------------------------------
	void StopStreaming()
	{
		// Make sure we're connected
		if (IsStreaming())
		{
			DBG_LOG0("Stopping streaming.");

			void* response;
			int nBytes;
			EVaRT2_Request("Pause", &response, &nBytes);
		}

		l_bStreaming = false;
	}

	//--------------------------------------------------------------------
	//	Returns true if it is streaming frame data at the moment.
	//--------------------------------------------------------------------
	bool IsStreaming()
	{
		return l_bStreaming;
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
		return l_Bodies.size();
	}

	//--------------------------------------------------------------------
	// Return name of a body by index.
	//--------------------------------------------------------------------
	std::string GetBodyName(int i_Index)
	{
		std::string name("");
		EnterCriticalSection(&l_CriticalSection);
		if (i_Index >= 0 && i_Index < l_Bodies.size())
			name = l_Bodies[i_Index].m_Name;
		LeaveCriticalSection(&l_CriticalSection);
		return name;
	}

	//--------------------------------------------------------------------
	// Return number of segments. Just for quick polling. Note that
	//	between calls to GetNumSegments and GetSegmentData, things
	//	might have changed.
	//--------------------------------------------------------------------
	int GetNumSegments()
	{
		int count = 0;

		EnterCriticalSection(&l_CriticalSection);

		if (!l_Bodies.empty())
		{
			count = l_Bodies[0].m_Segments.size();
		}

		LeaveCriticalSection(&l_CriticalSection);

		return count;
	}
	int GetNumSegments(const std::string &i_BodyName)
	{
		int count = 0;

		EnterCriticalSection(&l_CriticalSection);

		int index = get_index_for_body(i_BodyName);
		if (index >= 0 && index < l_Bodies.size())
			count = l_Bodies[index].m_Segments.size();

		LeaveCriticalSection(&l_CriticalSection);

		return count;
	}

	//--------------------------------------------------------------------
	// Get copy of the current segment data 
	//--------------------------------------------------------------------
	void GetSegmentData(std::vector<mcpHTRSegmentData> &o_Segments)
	{
		EnterCriticalSection(&l_CriticalSection);
		if (!l_Bodies.empty())
		{
			o_Segments = l_Bodies[0].m_Segments;
		}
		LeaveCriticalSection(&l_CriticalSection);
	}
	void GetSegmentData(const std::string &i_BodyName,
						std::vector<mcpHTRSegmentData> &o_Segments)
	{
		EnterCriticalSection(&l_CriticalSection);
		int index = get_index_for_body(i_BodyName);
		if (index >= 0 && index < l_Bodies.size())
		{
			o_Segments = l_Bodies[index].m_Segments;
		}
		LeaveCriticalSection(&l_CriticalSection);

	}

}	// end of namespace

#endif // EVART_SDK == 2