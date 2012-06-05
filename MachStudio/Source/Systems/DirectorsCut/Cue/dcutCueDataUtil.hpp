/*****************************************************************************
**	dcutCueDataUtil.hpp
**
**	Utility for managing data related to the CameraCueForm
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


#ifdef DCUT_CUEDATAUTIL_HPP
#error dcutCueDataUtil.hpp multiply included
#endif
#define DCUT_CUEDATAUTIL_HPP

class dcutCueFormData;

namespace dcutCueDataUtil
{
	//--------------------------------------------------------------------
	// Clear - reset camera cue data
	//--------------------------------------------------------------------
	void Clear();

	//--------------------------------------------------------------------
	// SetData - set camera cue data from document
	//--------------------------------------------------------------------
	void SetData(const dcutCueFormData& i_Data);

	//--------------------------------------------------------------------
	// GetCurrentData
	//--------------------------------------------------------------------
	dcutCueFormData GetCurrentData();

	//--------------------------------------------------------------------
	// SetViewingIndex - set camera index for a given view
	//--------------------------------------------------------------------
	void SetViewingIndex(int i_View, int i_CameraIndex);
	int GetViewingIndex(int i_View);

	//--------------------------------------------------------------------
	// DeleteCamera shifts down indices
	//--------------------------------------------------------------------
	void DeleteCamera(int i_Index);

	//--------------------------------------------------------------------
	// AddCue adds a keyframe for the camera index at given time
	//--------------------------------------------------------------------
	void AddCue(float i_Time, int i_Index);

	//--------------------------------------------------------------------
	// GetCameraForTime returns camera index active at given time
	//--------------------------------------------------------------------
	int GetCameraForTime(float i_Time);

}	// end of namespace
