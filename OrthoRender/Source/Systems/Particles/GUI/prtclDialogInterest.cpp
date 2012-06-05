/****************************************************************************\
**	prtclDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/GUI/prtclDialogInterest.hpp"

#include "Systems/Particles/GUI/prtclDialogDataUtil.hpp"
#include "Systems/Particles/GUI/prtclGeomList.hpp"
#include "Systems/Particles/Object/prtclObjectMgr.hpp"
#include "Systems/Particles/Undo/prtclOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
//#include "Core/it/itString.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Particles";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDialogInterest::prtclDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("prtclDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		prtclOperations::AddObject( prtclScriptData( i_FileName ) );
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
void prtclDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = prtclObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		prtclOperations::RemoveParticle(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = prtclObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		prtclOperations::DuplicateObject(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = prtclObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		prtclOperations::ReloadObject(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	prtclOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  prtclDialogInterest::DeselectObject(const nameString& i_Name) const
{
	prtclOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	prtclDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void prtclDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	//	get the files
	fsysFileList system_filelist;
	prtclGeomList::BuildFileList(system_filelist);

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
int prtclDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			);
}

