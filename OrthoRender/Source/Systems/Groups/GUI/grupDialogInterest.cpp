/****************************************************************************\
**	grupDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/GUI/grupDialogInterest.hpp"

#include "Systems/Groups/GUI/grupDialogDataUtil.hpp"
#include "Systems/Groups/Object/grupObjectMgr.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Groups";

	itString c_NewGroupString("New Group");
	itString c_NewGroupFromSelectionString("New Group From Selection");
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
grupDialogInterest::grupDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("grupDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	if (i_FileName == c_NewGroupFromSelectionString)
	{
		grupOperations::CreateGroupFromSelection();
	}
	else
		grupOperations::AddObject();

	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = grupObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		grupOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::DuplicateObject(const nameString& i_Name)
{
//	int index = grupObjectMgr::GetIndexForObject(i_Name);
//	if (index >= 0)
//	{
//		grupOperations::DuplicateObject(index);
//	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = grupObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		grupOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	grupOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  grupDialogInterest::DeselectObject(const nameString& i_Name) const
{
	grupOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	ActivateObject from Placed, represents a double-click on the
//		item in the placed menu.
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::ActivateObject(const nameString& i_Name) 
{
	grupOperations::SelectGroupObjects(i_Name);

}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	grupDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void grupDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	io_FileList.AddFilename(path,c_NewGroupString);
	io_FileList.AddFilename(path,c_NewGroupFromSelectionString);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int grupDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
//			| e_DIAllow_Dupe
			);
}

