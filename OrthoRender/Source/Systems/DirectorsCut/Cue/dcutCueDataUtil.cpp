/*****************************************************************************
**	dcutCueDataUtil.cpp
**
**	Utility for managing data related to the CameraCueForm
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Cue/dcutCueDataUtil.hpp"

#include "Systems/DirectorsCut/Data/dcutScriptData.hpp"
#include "Systems/DirectorsCut/Data/dcutDocumentChunk.hpp"

#include "Support/cams/camsCameraMgr.hpp"


//============================================================================
//============================================================================
namespace dcutCueDataUtil
{
	namespace
	{
		int l_Index[4];

	}	// end of namespace


	//--------------------------------------------------------------------
	// Clear - reset camera cue data
	//--------------------------------------------------------------------
	void Clear()
	{
		l_Index[0] = -1;
		l_Index[1] = -1;
		l_Index[2] = -1;
		l_Index[3] = -1;

	}

	//--------------------------------------------------------------------
	// SetData - set camera cue data from document
	//--------------------------------------------------------------------
	void SetData(const dcutCueFormData& i_Data)
	{
		l_Index[0] = i_Data.m_Index[0];
		l_Index[1] = i_Data.m_Index[1];
		l_Index[2] = i_Data.m_Index[2];
		l_Index[3] = i_Data.m_Index[3];
	}

	//--------------------------------------------------------------------
	// GetCurrentData
	//--------------------------------------------------------------------
	dcutCueFormData GetCurrentData()
	{
		dcutCueFormData data;
		camsCameraMgr::GetCameraNames(data.m_CameraNames);
		data.m_Index[0] = l_Index[0];
		data.m_Index[1] = l_Index[1];
		data.m_Index[2] = l_Index[2];
		data.m_Index[3] = l_Index[3];
		return data;
	}

	//--------------------------------------------------------------------
	// SetViewingIndex - set camera index for a given view
	//--------------------------------------------------------------------
	void SetViewingIndex(int i_View, int i_CameraIndex)
	{
		DBG_ASSERT0(i_View >= 0 && i_View < 4, "View index out of range");
		l_Index[i_View] = i_CameraIndex;

		// set dirty bit in document
		dcutDocumentChunk::ActiveDataChanged();
	}
	int GetViewingIndex(int i_View)
	{
		DBG_ASSERT0(i_View >= 0 && i_View < 4, "View index out of range");
		return l_Index[i_View];
	}

	//--------------------------------------------------------------------
	// DeleteCamera shifts down indices
	//--------------------------------------------------------------------
	void DeleteCamera(int i_Index)
	{
		for (int i=0; i<4; i++)
		{
			if (l_Index[i] == i_Index)
				l_Index[i] = -1;
			else if (l_Index[i] > i_Index)
				l_Index[i]--;
		}
	}


	//--------------------------------------------------------------------
	// AddCue adds a keyframe for the camera index at given time
	//--------------------------------------------------------------------
	void AddCue(float i_Time, int i_Index)
	{
	//	// check to see if there is already an existing key at this time
	//	int ind = l_Cues.GetLowerKeyIndex(i_Time);
	//	if (ind >= l_Cues.GetNumKeys())
	//		l_Cues.AddKey(i_Time, i_Index);	// add new key
	//	else
	//	{
	//		float old_time = 0;
	//		int value = 0;
	//		l_Cues.GetKeyData(ind, old_time, value);
	//		if (old_time == i_Time)
	//			l_Cues.AlterKey(ind, i_Index); // alter existing key
	//		else if (value != i_Index)
	//			l_Cues.AddKey(i_Time, i_Index);	// if camera changed, add new key
	//	}

	}

	//--------------------------------------------------------------------
	// GetCameraForTime returns camera index active at given time
	//--------------------------------------------------------------------
	int GetCameraForTime(float i_Time)
	{
		//int ind = l_Cues.GetLowerKeyIndex(i_Time);
		//if (ind < l_Cues.GetNumKeys())
		//{
		//	float old_time = 0;
		//	int value = 0;
		//	l_Cues.GetKeyData(ind, old_time, value);
		//	return value;
		//}
		//return l_Cues.GetValue(l_Cues.GetLength());  // outside of range, get last value
		return 0;
	}

}	// end of namespace
