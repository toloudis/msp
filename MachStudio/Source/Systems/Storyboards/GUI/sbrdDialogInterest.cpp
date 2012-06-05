/****************************************************************************\
**	sbrdDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/GUI/sbrdDialogInterest.hpp"

#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_SystemName = "Storyboards";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdDialogInterest::sbrdDialogInterest()
:	cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("sbrdDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		sbrdOperations::AddObject( sbrdScriptData() );
	}
	catch (const fsFileDoesntExistX& i_Ex)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		std::string msg = "Cannot find file\n" + filename;
		guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
	}

	// change focus to the main app
	guiMainWindow::Focus();
	
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = sbrdObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		sbrdOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = sbrdObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
//		sbrdOperations::DuplicateBillboard(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::ReloadObject(const nameString& i_Name)
{
	
	//	int index = sbrdObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		sbrdOperations::ReloadObject(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	sbrdOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  sbrdDialogInterest::DeselectObject(const nameString& i_Name) const
{
	sbrdOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	sbrdDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void sbrdDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	////	get the files
	//fsysFileList system_filelist;
	//sbrdGeomList::BuildFileList(system_filelist);
	//
	////	massage the data
	//system_filelist.StripPathsBefore(itString("Data"),true);
	//system_filelist.PrependOnPaths(itString(c_SystemName));
	//
	////	append the list
	//io_FileList.AppendFromFileList(system_filelist);

	fsLocator path;
	path.Push(c_SystemName);
	itString fname;
	fname = itString("Storyboards");
	io_FileList.AddFilename(path,fname);
}


//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int sbrdDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	bool bGotOne = (sbrdObjectMgr::GetNumObjects() > 0);
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

