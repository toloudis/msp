/****************************************************************************\
**	giDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/GUI/giDialogInterest.hpp"

#include "Systems/GlobalIllumination/GUI/giDialogDataUtil.hpp"
#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"
#include "Systems/GlobalIllumination/Undo/giOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"

namespace
{
	const char* c_SystemName = "GI";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
giDialogInterest::giDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void giDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void giDialogInterest::DeleteObject(const nameString& i_Name)
{
//	giOperations::DeleteObject();
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void giDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	giOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  giDialogInterest::DeselectObject(const nameString& i_Name) const
{
	giOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void giDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	giDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void giDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	if (giObjectMgr::GetNumObjects() == 0)
	{
		fsLocator path;
		path.Push(c_SystemName);
		itString fname;
		fname = itString("GI");
		io_FileList.AddFilename(path,fname);
	}
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int giDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return  e_DIAllow_Edit;

	//bool bGotOne = (giObjectMgr::GetNumObjects() > 0);
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

