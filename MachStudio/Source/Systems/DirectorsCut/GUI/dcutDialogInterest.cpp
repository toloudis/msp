/****************************************************************************\
**	dcutDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/GUI/dcutDialogInterest.hpp"

#include "Systems/DirectorsCut/GUI/dcutDialogDataUtil.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#include "Systems/DirectorsCut/Undo/dcutOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

namespace
{
	const char* c_SystemName = "Director's Cuts";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDialogInterest::dcutDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("dcutDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	dcutOperations::AddObject();
	
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = dcutObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		dcutOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::DuplicateObject(const nameString& i_Name)
{
	
	//	int index = dcutObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		dcutOperations::DuplicateCamera(index);
	//	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = dcutObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		dcutOperations::ReloadCamera(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	dcutOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  dcutDialogInterest::DeselectObject(const nameString& i_Name) const
{
	dcutOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	dcutDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void dcutDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString("New Cut");
	io_FileList.AddFilename(path,fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int dcutDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				| e_DIAllow_Del
				//| e_DIAllow_Dupe
				);
}

