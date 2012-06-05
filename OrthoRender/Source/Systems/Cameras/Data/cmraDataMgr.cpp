/*****************************************************************************
**	cmraDataMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Data/cmraDataMgr.hpp"

//#include "Systems/Cameras/Cue/cmraCueDataUtil.hpp"
//#include "Systems/Cameras/Cue/cmraCueDialogUtil.hpp"
#include "Systems/Cameras/Data/cmraDataParser.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Graphics/cam/camCameraManipOrbit.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/fs/fsResourceTrackerData.hpp"


//
namespace
{
	void get_init_camera(maPoint3d &o_Target, float &o_Pitch, float &o_Yaw, float &o_Radius)
	{
		camCameraManipOrbit *manip = 
			dynamic_cast<camCameraManipOrbit*>(cam3dMgr::GetCameraManip());
		if (manip)
		{
			o_Target = manip->GetTarget();
			o_Pitch = manip->GetPitch();
			o_Yaw = manip->GetYaw();
			o_Radius = manip->GetRadius();
		}
		else
		{
			camCamera& camera = cam3dMgr::GetCamera();
			
			maPoint3d cam_pos = camera.GetPosition();
			maVector3d cam_dir = camera.GetDirection();

			o_Target = camera.GetTarget();
			o_Radius = ( cam_pos - o_Target ).Length();

			maPoint3d target_dir = -cam_dir;

			o_Yaw = float(::atan2f(target_dir.m_X, target_dir.m_Z));
			float zx_len = sqrtf( target_dir.m_Z * target_dir.m_Z + target_dir.m_X * target_dir.m_X );
			o_Pitch = float(::atan2f(target_dir.m_Y, zx_len));
		}
	}

	void set_init_camera(const maPoint3d &i_Target, float i_Pitch, float i_Yaw, float i_Radius)
	{
		camCameraManipOrbit *manip = 
			dynamic_cast<camCameraManipOrbit*>(cam3dMgr::GetCameraManip());
		if (manip)
		{
			manip->SetTarget(i_Target);
			manip->SetPitch(i_Pitch);
			manip->SetYaw(i_Yaw);
			manip->SetRadius(i_Radius);
		}
	}

}	// end of namespace

//--------------------------------------------------------------------
//  Clear
//--------------------------------------------------------------------
void  cmraDataMgr::Clear()
{
	cmraObjectMgr::Clear();
	cmraDialogUtil::UpdateListDialog();

	//cmraCueDataUtil::Clear();
	//cmraCueDialogUtil::UpdateCameraNames();
}


//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
cmraCamerasData cmraDataMgr::GetData()
{
	cmraCamerasData data = cmraObjectMgr::GetData();

	// Initial data now stored as editor camera within object manager
	//maPoint3d target;
	//float pitch, yaw, radius;
	//get_init_camera(target, pitch, yaw, radius);
	//data.m_Target.SetValue(target);
	//data.m_Pitch.SetValue(pitch);
	//data.m_Yaw.SetValue(yaw);
	//data.m_Radius.SetValue(radius);

	//data.m_CueForm = cmraCueDataUtil::GetCurrentData();

	return data;
}

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void cmraDataMgr::SetData(const cmraCamerasData &i_Data)
{
	// Initial data now stored as editor camera within object manager
	//set_init_camera(i_Data.m_Target.GetValue(), i_Data.m_Pitch.GetValue(), i_Data.m_Yaw.GetValue(), i_Data.m_Radius.GetValue());

	cmraObjectMgr::SetData(i_Data);
	cmraDialogUtil::UpdateListDialog();

	//cmraCueDataUtil::SetData(i_Data.m_CueForm);
	//cmraCueDialogUtil::UpdateCameraNames();
}

//--------------------------------------------------------------------
// Append objects from this data into list, not allowing duplicates
//--------------------------------------------------------------------
void cmraDataMgr::MergeData(const cmraCamerasData &i_Data)
{
	cmraObjectMgr::MergeData(i_Data);
}

//--------------------------------------------------------------------
//  Get number of cameras
//--------------------------------------------------------------------
int cmraDataMgr::GetNumObjects()
{
	return cmraObjectMgr::GetNumObjects();
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  cmraDataMgr::DeleteObject(int i_Index)
{
	cmraObjectMgr::DeleteObject(i_Index);
}

//--------------------------------------------------------------------
//  Get the camera
//--------------------------------------------------------------------
cmraScriptObject* cmraDataMgr::GetObject(int i_Index)
{
	return cmraObjectMgr::GetObject(i_Index);
}

//--------------------------------------------------------------------
// return index of given Name, returns -1 if not found
//--------------------------------------------------------------------
int cmraDataMgr::GetIndexForObject( const nameString& i_Name )
{
	return cmraObjectMgr::GetIndexForObject(i_Name);
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void cmraDataMgr::GetResourceList( fsResourceTrackerData& io_List )
{
}
