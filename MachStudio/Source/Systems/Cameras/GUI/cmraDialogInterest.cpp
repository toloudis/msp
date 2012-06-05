/****************************************************************************\
**	cmraDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/GUI/cmraDialogInterest.hpp"

#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_SystemName = "Cameras";
	const char* c_PerspectiveName = "Perspective Camera";
	const char* c_OrthographicName = "Orthographic Camera";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDialogInterest::cmraDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}


//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	//std::string dir1, dir2;
	//dir1 = itStringUtil::GetStdString(i_Path.GetName(0));
	//dir2 = itStringUtil::GetStdString(i_Path.GetName(1));
	//DBG_LOG2("cmraDialogInterest %s-%s", dir1.c_str(), dir2.c_str());

	bool bOrthographic = (i_FileName == itString(c_OrthographicName));
	cmraOperations::AddObject(bOrthographic);

	// change focus to the main app
	guiMainWindow::Focus();
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::DeleteObject(const nameString& i_Name)
{
	int index = cmraObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		cmraOperations::DeleteObject(index);
	}
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
nameString cmraDialogInterest::DuplicateObject(const nameString& i_Name)
{
	int index = cmraObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		return cmraOperations::DuplicateObject(index);
	}
	return nameString();
}

//--------------------------------------------------------------------
//	Second part of duplication step, remap all internal name
//	attachments so that the new objects are attached to each other
//  and not to the original objects anymore.
//--------------------------------------------------------------------
void cmraDialogInterest::RemapNames(const nameString& i_Name, 
								   const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	int index = cmraObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		cmraObjectMgr::RemapNames(index, i_DuplicateNameMap);
	}
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::ReloadObject(const nameString& i_Name)
{
	//	int index = cmraObjectMgr::GetIndexForObject(i_Name);
	//	if (index >= 0)
	//	{
	//		cmraOperations::ReloadCamera(index);
	//	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{
	//	change the view to the selected camera ONLY if the current view isn't the editor camera
	//
	if (cam3dMgr::IsUsingScriptedCamera())
	{
		int index = cmraObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			//cmraOperations::SelectFollowCamera(index);
			rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index),
										i_Name.GetString());
		}
	}

	//	after the view is changed, select the object
	//	if done before, the view won't be changed
	//
	cmraOperations::SelectObject(i_Name, i_bAppend);
}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  cmraDialogInterest::DeselectObject(const nameString& i_Name) const
{
	cmraOperations::DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{
	cmraDialogDataUtil::RebuildListData(io_DataList);
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void cmraDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	itString fname;
	fname = itString(c_PerspectiveName);
	io_FileList.AddFilename(path, fname);
	fname = itString(c_OrthographicName);
	io_FileList.AddFilename(path, fname);
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int cmraDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	fsLocator defobj;
	defobj.Push("Cameras");
	defobj.Push("Editor");
	if (defobj == i_Path)
	{
		return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				//| e_DIAllow_Del
				//| e_DIAllow_Dupe
				);
	}
	else
	{
		return (  e_DIAllow_Edit
				| e_DIAllow_Add 
				| e_DIAllow_Del
				| e_DIAllow_Dupe
				);
	}
}

