/****************************************************************************\
**	setsDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/GUI/setsDialogInterest.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/GUI/setsDialogDataUtil.hpp"
#include "Systems/Sets/GUI/setsGeomList.hpp"
#include "Systems/Sets/Undo/setsOperations.hpp"
#include "Systems/Sets/Data/setsScriptData.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	const char* c_SystemName = "Sets";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
setsDialogInterest::setsDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("setsDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		setsOperations::AddSetItem( setsScriptData( i_FileName ) );
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
void setsDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = setsDataMgr::GetIndexForItem(i_Name);
	if (index >= 0)
	{
		setsOperations::RemoveSetItem(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::DuplicateObject(const nameString& i_Name)
{
	//	int index = setsDataMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		setsOperations::DuplicateSet(index);
	//	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = setsDataMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		setsOperations::ReloadSet(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	int index = setsDataMgr::GetIndexForItem(i_Name);
	if (index >= 0)
		setsDataMgr::SelectObject(index, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  setsDialogInterest::DeselectObject(const nameString& i_Name) const
{
	int index = setsDataMgr::GetIndexForItem(i_Name);
	if (index >= 0)
		setsDataMgr::DeselectObject(index);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	setsDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void setsDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	//	get the files
	fsysFileList system_filelist;
	setsGeomList::BuildFileList(system_filelist);

	//	massage the data
	system_filelist.StripPathsBefore(itString("Data"),true);
	system_filelist.PrependOnPaths(itString(c_SystemName));

	//	append the list
	io_FileList.AppendFromFileList(system_filelist);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int setsDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Add
			| e_DIAllow_Del
			//| e_DIAllow_Edit
			//| e_DIAllow_Dupe
			);
}

