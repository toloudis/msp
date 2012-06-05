/****************************************************************************\
**	prjltDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/GUI/prjltDialogInterest.hpp"

#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
//#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	const char* c_SystemName = "Projected Lights";
	const char* c_CurrentViewLabel = "Projected Light At Current View";
	const char* c_SpotLightLabel = "Spot Light";
	const char* c_DirectionalLightLabel = "Directional Light";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltDialogInterest::prjltDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void prjltDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("prjltDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	try
	{
		if (i_FileName == itString(c_SpotLightLabel))
			prjltOperations::AddObject(prjltData::e_SpotLight);
		else if (i_FileName == itString(c_DirectionalLightLabel))
			prjltOperations::AddObject(prjltData::e_DirectionalLight);
		else if (i_FileName == itString(c_CurrentViewLabel))
			prjltOperations::AddLightAtCameraPosition();
		else
			prjltOperations::AddObject();
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
void prjltDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = prjltObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		prjltOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
nameString prjltDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = prjltObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		return prjltOperations::DuplicateObject(index);
	}
	return nameString();
}

//--------------------------------------------------------------------
//	Second part of duplication step, remap all internal name
//	attachments so that the new objects are attached to each other
//  and not to the original objects anymore.
//--------------------------------------------------------------------
void prjltDialogInterest::RemapNames(const nameString& i_Name, 
								   const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	int index = prjltObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		prjltObjectMgr::RemapNames(index, i_DuplicateNameMap);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void prjltDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = prjltObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		prjltOperations::ReloadProjectedLight(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void prjltDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	prjltOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  prjltDialogInterest::DeselectObject(const nameString& i_Name) const
{
	prjltOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void prjltDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	prjltDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void prjltDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);

	// Basic creation of Projected Light
	io_FileList.AddFilename(path, itString("Projected Light"));

	// Create Projected Light at current view
	io_FileList.AddFilename(path, itString(c_CurrentViewLabel));
}


//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int prjltDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	
	return (  e_DIAllow_Edit
			| e_DIAllow_Add 
			| e_DIAllow_Del
			| e_DIAllow_Dupe
			| e_DIAllow_Reload
			);

}

