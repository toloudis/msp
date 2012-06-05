/****************************************************************************\
**	aoDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/GUI/aoDialogInterest.hpp"

#include "Systems/AmbientOcclusion/GUI/aoDialogDataUtil.hpp"
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"
#include "Systems/AmbientOcclusion/Undo/aoOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"

namespace
{
	const char* c_SystemName = "AO";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
aoDialogInterest::aoDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void aoDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void aoDialogInterest::DeleteObject(const nameString& i_Name)
{
//	aoOperations::DeleteObject();
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void aoDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	aoOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  aoDialogInterest::DeselectObject(const nameString& i_Name) const
{
	aoOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void aoDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	aoDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void aoDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	if (aoObjectMgr::GetNumObjects() == 0)
	{
		fsLocator path;
		path.Push(c_SystemName);
		itString fname;
		fname = itString("AO");
		io_FileList.AddFilename(path,fname);
	}
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int aoDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return  e_DIAllow_Edit;

	//bool bGotOne = (aoObjectMgr::GetNumObjects() > 0);
	//if (bGotOne)
	//{
	//	return (e_DIAllow_Del | e_DIAllow_Edit);
	//}
	//else
	//{
	//	//	since there is no object
	//	//
	//	return (e_DIAllow_Add);
	//}
}

