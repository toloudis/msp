/****************************************************************************\
**	propDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/GUI/propDialogInterest.hpp"

#include "Systems/Props/GUI/propDialogDataUtil.hpp"
#include "Systems/Props/GUI/propGeomList.hpp"
#include "Systems/Props/Object/propObjectMgr.hpp"
#include "Systems/Props/Undo/propOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	const char* c_SystemName = "Props";
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDialogInterest::propDialogInterest()
: cmmDialogInterest(c_SystemName)
{

}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("propDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		propOperations::AddObject( propScriptData( i_FileName ) );
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
void propDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = propObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		propOperations::RemoveProp(index);
	}

}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = propObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		propOperations::DuplicateObject(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = propObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		propOperations::ReloadProp(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	propOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  propDialogInterest::DeselectObject(const nameString& i_Name) const
{
	propOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	Select Object Part from Placed
//		If i_bAppend==true, add object to selection list.
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::SelectObjectPart(const nameString& i_Name, 
							  const std::string i_PartName, 
							  const std::string i_CategoryName, 
							  bool i_bAppend) const
{
	propOperations::SelectObjectPart(i_Name, i_PartName, i_CategoryName, i_bAppend);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	propDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void propDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	//	get the files
	fsysFileList system_filelist;
	propGeomList::BuildFileList(system_filelist);

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
int propDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			);
}

