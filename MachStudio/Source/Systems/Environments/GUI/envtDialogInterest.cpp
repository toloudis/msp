/****************************************************************************\
**	envtDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/envtDialogInterest.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"


namespace
{
	const char* c_SystemName = "Environment Lights";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtDialogInterest::envtDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void envtDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("envtDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		envtOperations::AddObject();
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
void envtDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = envtObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		envtOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void envtDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	check if the path signifies this system or not
	//	if so, add the object
	//
	//if (strcmp(i_SystemName.c_str(), "PointLights") == 0)
	//{
	//	int index = envtObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		envtOperations::ReloadPointLight(index);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void envtDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	envtOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  envtDialogInterest::DeselectObject(const nameString& i_Name) const
{
	envtOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void envtDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	envtDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void envtDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString("Environment Lights");
	io_FileList.AddFilename(path,fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int envtDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	fsLocator defobj;
	defobj.Push("Environment Lights");
	defobj.Push("Default");
	if (defobj == i_Path)
	{
		return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				| e_DIAllow_Del
				| e_DIAllow_Reload
				//| e_DIAllow_Dupe
				);
	}
	else
	{
		return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				| e_DIAllow_Del
				| e_DIAllow_Reload
				//| e_DIAllow_Dupe
				);
	}
}

