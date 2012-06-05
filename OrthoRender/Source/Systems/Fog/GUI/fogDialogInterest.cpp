/****************************************************************************\
**	fogDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/GUI/fogDialogInterest.hpp"

#include "Systems/Fog/GUI/fogDialogDataUtil.hpp"
#include "Systems/Fog/Object/fogObjectMgr.hpp"
#include "Systems/Fog/Undo/fogOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"

namespace
{
	const char* c_SystemName = "Fog";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
fogDialogInterest::fogDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::DeleteObject(const nameString& i_Name)
{
	fogOperations::DeleteObject();
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::DuplicateObject(const nameString& i_Name)
{
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::ReloadObject(const nameString& i_Name)
{
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	fogOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  fogDialogInterest::DeselectObject(const nameString& i_Name) const
{
	fogOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	fogDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void fogDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	if (fogObjectMgr::GetNumObjects() == 0)
	{
		fsLocator path;
		path.Push(c_SystemName);
		itString fname;
		fname = itString("Fog");
		io_FileList.AddFilename(path,fname);
	}
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int fogDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	bool bGotOne = (fogObjectMgr::GetNumObjects() > 0);
	if (bGotOne)
	{
		return (e_DIAllow_Del | e_DIAllow_Edit);
	}
	else
	{
		//	since there is no object
		//
		return (e_DIAllow_Add);
	}
}

