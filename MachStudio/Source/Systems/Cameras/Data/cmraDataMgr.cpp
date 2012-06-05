/*****************************************************************************
**	cmraDataMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Data/cmraDataMgr.hpp"

//#include "Systems/Cameras/Cue/cmraCueDataUtil.hpp"
//#include "Systems/Cameras/Cue/cmraCueDialogUtil.hpp"
#include "Systems/Cameras/Data/cmraDataParser.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/fs/fsResourceTrackerData.hpp"


//
namespace
{
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
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void cmraDataMgr::ClearItemNameUIDs(cmraCamerasData &o_Data)
{}

//--------------------------------------------------------------------
// Append objects from this data into list, not allowing duplicates
//
//	If i_bRenameDupes is true, the duplicate entry will be renamed
//	and merged.  If it is false, the duplicate will NOT be merged.
//--------------------------------------------------------------------
//static 
void cmraDataMgr::MergeData(const cmraCamerasData &i_Data, bool i_bRenameDupes)
{
	cmraObjectMgr::MergeData(i_Data, i_bRenameDupes);
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
	cmraObjectMgr::GetResourceList(io_List);
}
