/****************************************************************************\
**	ptltDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/GUI/ptltDialogInterest.hpp"

#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Point Lights";
	const char* c_CurrentViewLabel = "Point Light At Camera Position";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltDialogInterest::ptltDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("ptltDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		if (i_FileName == itString(c_CurrentViewLabel))
			ptltOperations::AddLightAtCameraPosition();
		else
			ptltOperations::AddObject();
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
void ptltDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = ptltObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		ptltOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = ptltObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		ptltOperations::DuplicateObject(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = ptltObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		ptltOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	ptltOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  ptltDialogInterest::DeselectObject(const nameString& i_Name) const
{
	ptltOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	ptltDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void ptltDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);

	// Basic creation of Point Light
	io_FileList.AddFilename(path, itString("Point Light"));

	// Create Point Light at current view
	io_FileList.AddFilename(path, itString(c_CurrentViewLabel));
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int ptltDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			);
}

