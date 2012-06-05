/*****************************************************************************
**	dcutDataMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Data/dcutDataMgr.hpp"

#include "Systems/DirectorsCut/Cue/dcutCueDataUtil.hpp"
#include "Systems/DirectorsCut/Cue/dcutCueDialogUtil.hpp"
#include "Systems/DirectorsCut/Data/dcutDataParser.hpp"
#include "Systems/DirectorsCut/GUI/dcutDialogUtil.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"

#include "Graphics/cam/camCameraManipOrbit.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/fs/fsResourceTrackerData.hpp"


//
namespace
{
}	// end of namespace


//--------------------------------------------------------------------
//  Clear
//--------------------------------------------------------------------
void  dcutDataMgr::Clear()
{
	dcutObjectMgr::Clear();
	dcutDialogUtil::UpdateListDialog();

	dcutCueDataUtil::Clear();
	dcutCueDialogUtil::UpdateCameraNames();
}


//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
dcutCuesData dcutDataMgr::GetData()
{
	dcutCuesData data = dcutObjectMgr::GetData();
	data.m_CueForm = dcutCueDataUtil::GetCurrentData();
	return data;
}

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void dcutDataMgr::SetData(const dcutCuesData &i_Data)
{
	dcutObjectMgr::SetData(i_Data);
	dcutDialogUtil::UpdateListDialog();

	dcutCueDataUtil::SetData(i_Data.m_CueForm);
	dcutCueDialogUtil::UpdateCameraNames();
}

//--------------------------------------------------------------------
// Append objects from this data into list, not allowing duplicates
//--------------------------------------------------------------------
void dcutDataMgr::MergeData(const dcutCuesData &i_Data)
{
	dcutObjectMgr::MergeData(i_Data);
}

//--------------------------------------------------------------------
//  Get number of cameras
//--------------------------------------------------------------------
int dcutDataMgr::GetNumObjects()
{
	return dcutObjectMgr::GetNumObjects();
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  dcutDataMgr::DeleteObject(int i_Index)
{
	dcutObjectMgr::DeleteObject(i_Index);
}

//--------------------------------------------------------------------
//  Get the camera
//--------------------------------------------------------------------
dcutScriptObject* dcutDataMgr::GetObject(int i_Index)
{
	return dcutObjectMgr::GetObject(i_Index);
}

//--------------------------------------------------------------------
// return index of given Name, returns -1 if not found
//--------------------------------------------------------------------
int dcutDataMgr::GetIndexForObject( const nameString& i_Name )
{
	return dcutObjectMgr::GetIndexForObject(i_Name);
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void dcutDataMgr::GetResourceList( fsResourceTrackerData& io_List )
{
}
