/****************************************************************************\
**	chtrDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrDialogInterest.hpp"

#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"
#include "Systems/Character/GUI/chtrPartConstants.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"

namespace
{
	const char* c_SystemName = "Characters";
	const char* c_BrowseObject = "Browse";
	const char* c_BrowseInitDir = "C:\\projects";
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
		std::string addObjectName;
		addObjectName = itStringUtil::GetStdString(i_FileName);
		if(addObjectName == c_BrowseObject)
		{
			itString browseFileName;
			if( BrowseObject( browseFileName ) )
			{

				std::string characterText;
				characterText = itStringUtil::GetStdString(browseFileName);
				DBG_WARNING1("Character model: %s", characterText.c_str());
				chtrOperations::AddObject( chtrScriptData( browseFileName ) );
			}
		}
		else
		{
			std::string characterText;
			characterText = itStringUtil::GetStdString(i_FileName);
			DBG_WARNING1("Character model: %s", characterText.c_str());
			chtrOperations::AddObject( chtrScriptData( i_FileName ) );
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
void chtrDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = chtrObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		chtrOperations::DuplicateObject(index);
	}
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
							  bool i_bAppend) const
{
	chtrOperations::SelectObjectPart(i_Name, i_PartName, i_CategoryName, i_bAppend);
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
	
	//	get the files
	fsysFileList system_filelist;
	chtrGeomList::BuildFileList(system_filelist);

	//	massage the data
	system_filelist.StripPathsBefore(itString("Data"),true);
	system_filelist.PrependOnPaths(itString(c_SystemName));
	
	//	append the list
	//first allow for users to browse for characters
	const itString browseItString = itString(c_BrowseObject);
	fsLocator path;
	path.Push(c_SystemName);
#ifdef ZERO
	//eliminates browse string in available list
	io_FileList.AddFilename(path, browseItString);
#endif
	//now append the rest of the list
	io_FileList.AppendFromFileList(system_filelist);
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
//--------------------------------------------------------------------
//	BrowseObject - returns an itString File identifier based on the users file browse
//  for a particular character file.
//--------------------------------------------------------------------
//virtual
bool chtrDialogInterest::BrowseObject(itString& browseFile) const
{
	//i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*", "chx" for character models
	std::string fileFilter = "Model Files (*.*x*)|*.*x*|Character Files (*.chx)|*.chx|All files (*.*)|*.*";
	fsLocator browsePath;
	browsePath.Push(c_BrowseInitDir);

	fsLocator chosenFile;
	
	if(guiFileDialogUtils::GetOpenFileName(fileFilter, browsePath, chosenFile))
	{
		chosenFile.RemoveBefore(chosenFile.GetLastName());
		fsFileUtil::LocatorToUnicodeString(chosenFile, browseFile);
		browseFile.RemoveCharAt(browseFile.GetLength() - 1);
		return true;
	}
	else
		return false;
}