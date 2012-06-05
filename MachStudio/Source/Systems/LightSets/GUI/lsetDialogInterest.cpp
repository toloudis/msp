/****************************************************************************\
**	lsetDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetDialogInterest.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "LightSets";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetDialogInterest::lsetDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("lsetDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	lsetOperations::AddObject();

	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = lsetObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		lsetOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = lsetObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		lsetOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	lsetOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  lsetDialogInterest::DeselectObject(const nameString& i_Name) const
{
	lsetOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	ReceiveDrag represents a drag and drop of the
//		selected items onto the given item in the tree view in 
//		in the scene manager.
//--------------------------------------------------------------------
//virtual
void  lsetDialogInterest::ReceiveDrag(const nameString& i_Name)
{
	if (i_Name.IsEmpty())
	{
		lsetOperations::RemoveSelectedLightsFromLightSets();
	}
	else
	{
		int index = lsetObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			lsetOperations::AddSelectionToLightSet(index);
		}
	}
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	lsetDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void lsetDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString("LightSet");
	io_FileList.AddFilename(path,fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int lsetDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
//			| e_DIAllow_Dupe
			);
}

