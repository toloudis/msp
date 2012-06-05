/****************************************************************************\
**	trfnDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/GUI/trfnDialogInterest.hpp"

#include "Systems/Transforms/GUI/trfnDialogDataUtil.hpp"
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Parents";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
trfnDialogInterest::trfnDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("trfnDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	trfnOperations::AddObject();

	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = trfnObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		trfnOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
nameString trfnDialogInterest::DuplicateObject(const nameString& i_Name,
											   std::map<nameString, nameString> &o_DuplicateNameMap)
{
	int index = trfnObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		return trfnOperations::DuplicateObject(index, o_DuplicateNameMap);
	}
	return nameString();
}

//--------------------------------------------------------------------
//	Second part of duplication step, remap all internal name
//	attachments so that the new objects are attached to each other
//  and not to the original objects anymore.
//--------------------------------------------------------------------
void trfnDialogInterest::RemapNames(const nameString& i_Name, 
								    const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	int index = trfnObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		trfnObjectMgr::RemapNames(index, i_DuplicateNameMap);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = trfnObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		trfnOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	trfnOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  trfnDialogInterest::DeselectObject(const nameString& i_Name) const
{
	trfnOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	ReceiveDrag represents a drag and drop of the
//		selected items onto the given item in the tree view in 
//		in the scene manager.
//--------------------------------------------------------------------
//virtual
void  trfnDialogInterest::ReceiveDrag(const nameString& i_Name)
{
	if (i_Name.IsEmpty())
		trfnOperations::RemoveSelectedFromParents();
	else
		trfnOperations::AddSelectedToTransform(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	trfnDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void trfnDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString("Parent");
	io_FileList.AddFilename(path,fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int trfnDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			);
}

