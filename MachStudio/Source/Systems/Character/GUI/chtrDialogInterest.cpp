/****************************************************************************\
**	chtrDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrDialogInterest.hpp"

#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"
#include "Systems/Character/GUI/chtrPartConstants.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMainWindow.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_SystemName = "Objects";
	const itString c_BrowseObject("Browse");
	//const char* c_BrowseInitDir = "C:\\projects";

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDialogInterest::chtrDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("chtrDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		if (i_FileName == c_BrowseObject)
		{
			chtrOperations::BrowseLoadObjects();
		}
		else
		{
			DBG_LOG("Object model: " <<  i_FileName);
			//chtrOperations::AddObject( chtrScriptData( i_FileName ) );
			
			// Get fullpath to file before adding, using chtrGeomList
			//fsysFileList file_list;
			//chtrGeomList::BuildFileList(file_list);
			//fsLocator full_path;
			//file_list.GetFilePath(i_FileName, full_path);
			//full_path.Push(i_FileName);
			//chtrOperations::AddObject( chtrScriptData( full_path ) );

			// Get fullpath to file before adding, using passed in path
			//fsLocator full_path;
			//full_path.Push( gfPaths::GetAppPath() );
			//full_path.Push("Data");
			//fsLocator rel_path(i_Path);
			//rel_path.Remove(itString(c_SystemName));
			//full_path.Push(rel_path);
			//full_path.Push(i_FileName);
			//chtrOperations::AddObject( chtrScriptData( full_path ) );

			// This code is only used now with python. In python, the full path
			// should be passed in, except that the first directory in the locator
			// is "Objects".
			fsLocator full_path = i_Path;
			if (full_path.GetNumNames() > 1)
			{
				full_path.RemoveBefore(1);
				chtrOperations::AddObject( chtrScriptData( full_path ) );
			}
		}
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
void chtrDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = chtrObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		chtrOperations::RemoveCharacter(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
nameString chtrDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = chtrObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		return chtrOperations::DuplicateObject(index);
	}
	return nameString();
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::ReloadObject(const nameString& i_Name)
{
	int index = chtrObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		chtrOperations::ReloadCharacter(index);
	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	chtrOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  chtrDialogInterest::DeselectObject(const nameString& i_Name) const
{
	chtrOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	Select Object Part from Placed
//		If i_bAppend==true, add object to selection list.
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::SelectObjectPart(const nameString& i_Name, 
							  const std::string i_PartName, 
							  const std::string i_CategoryName, 
							  cmmDialogInterest::SelectionMode i_Mode) const
{
	if (i_Mode == cmmDialogInterest::e_Toggle)
		chtrOperations::ToggleObjectPartSelection(i_Name, i_PartName, i_CategoryName);
	else if (i_Mode == cmmDialogInterest::e_Append)
		chtrOperations::SelectObjectPart(i_Name, i_PartName, i_CategoryName, true);
	else
		chtrOperations::SelectObjectPart(i_Name, i_PartName, i_CategoryName, false);
}

//--------------------------------------------------------------------
//	ActivateObjectPart from Placed, represents a double-click
//		on a part item in the placed menu.
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::ActivateObjectPart(const nameString& i_Name, 
							  const std::string i_PartName, 
							  const std::string i_CategoryName) const
{
	chtrOperations::ActivateObjectPart(i_Name, i_PartName, i_CategoryName);
}

//--------------------------------------------------------------------
//	DeleteObjectPart from Placed, represents DELETE key or button
//		when a part item is selected in the placed menu.
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::DeleteObjectPart(const nameString& i_Name, 
							  const std::string i_PartName, 
							  const std::string i_CategoryName) const
{
	chtrOperations::DeleteObjectPart(i_Name, i_PartName, i_CategoryName);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	chtrDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void chtrDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	// Adds an item for "browsing" for a filename in any location:
	fsLocator path;
	path.Push(c_SystemName);
	io_FileList.AddFilename(path, c_BrowseObject);

	//bga - no more searching for available objects, no more Available tab in product
	//
	//// Searches through the data directory for geometry files:
	//fsysFileList system_filelist;
	//chtrGeomList::BuildFileList(system_filelist);

	////	massage the data
	//system_filelist.StripPathsBefore(itString("Data"),true);
	//system_filelist.PrependOnPaths(itString(c_SystemName));
	//
	//// Append to the list
	//io_FileList.AppendFromFileList(system_filelist);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int chtrDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	if (i_Path.GetNumNames() == 4)
	{	
		// Path to part name. Allow delete if the category is "Controls"
		if (i_Path.GetName(2) == itString("Controls") &&
			i_Path.GetName(3) != itString(chtrPartConstants::c_NewControlString) )
		{
			return (  e_DIAllow_Edit
					| e_DIAllow_Del
					);
		}
		else if (i_Path.GetName(2) ==itString(chtrPartConstants::c_MaterialCategoryName) &&
			i_Path.GetName(3) != itString(chtrPartConstants::c_MaterialOverrideString) )
		{
			return (  e_DIAllow_Edit
					| e_DIAllow_Del
					| e_DIAllow_Reload
					);
		}
		else if (   (i_Path.GetName(2) == itString(chtrPartConstants::c_ExpressionCategoryName) 
			     &&	 i_Path.GetName(3) != itString(chtrPartConstants::c_NewExpressionSingleString)
			     &&	 i_Path.GetName(3) != itString(chtrPartConstants::c_NewExpressionDualString)
			     &&	 i_Path.GetName(3) != itString(chtrPartConstants::c_NewExpressionQuadString) ) )
		{
			return (  e_DIAllow_Edit
					| e_DIAllow_Del
					);
		}
		else
		{
			return (  e_DIAllow_Edit );
		}
	}
	else
	{
		return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				| e_DIAllow_Del
				| e_DIAllow_Reload
				| e_DIAllow_Dupe
				);
	}

}