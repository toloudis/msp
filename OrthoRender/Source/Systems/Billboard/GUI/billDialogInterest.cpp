/****************************************************************************\
**	billDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/GUI/billDialogInterest.hpp"

#include "Systems/Billboard/GUI/billDialogDataUtil.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Billboards";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
billDialogInterest::billDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("billDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		billOperations::AddObject( billScriptData( i_FileName ) );
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
void billDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = billObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		billOperations::RemoveBillboard(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = billObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		billOperations::DuplicateBillboard(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::ReloadObject(const nameString& i_Name)
{
	
	//	int index = billObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		billOperations::ReloadObject(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	billOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  billDialogInterest::DeselectObject(const nameString& i_Name) const
{
	billOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	billDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void billDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	//	get the files
	fsysFileList system_filelist;
	billGeomList::BuildFileList(system_filelist);

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
int billDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			);
}

