/****************************************************************************\
**	lyrsDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/GUI/lyrsDialogInterest.hpp"

#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"
#include "Systems/Layers/Object/lyrsObjectMgr.hpp"
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Layers";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
lyrsDialogInterest::lyrsDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("lyrsDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	lyrsOperations::AddObject();

	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = lyrsObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		lyrsOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::DuplicateObject(const nameString& i_Name)
{
//	int index = lyrsObjectMgr::GetIndexForObject(i_Name);
//	if (index >= 0)
//	{
//		lyrsOperations::DuplicateObject(index);
//	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = lyrsObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		lyrsOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	lyrsOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  lyrsDialogInterest::DeselectObject(const nameString& i_Name) const
{
	lyrsOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	lyrsDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void lyrsDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString("Layer");
	io_FileList.AddFilename(path,fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int lyrsDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
//			| e_DIAllow_Dupe
			);
}

