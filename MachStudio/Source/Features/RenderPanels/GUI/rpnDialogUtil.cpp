/*****************************************************************************
**	rpnDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/GUI/rpnDialogUtil.hpp"

#include "Features/RenderPanels/wxGUI/rpnCameraChoice.hpp"
//#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"
//#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"

// Includes from old managed file
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef CAMS_CAMERAMGR_HPP
#include "Support/cams/camsCameraMgr.hpp"
#endif
#ifndef CAMS_DIRECTORSCUTMGR_HPP
#include "Support/cams/camsDirectorsCutMgr.hpp"
#endif
//#ifndef CMRA_SCRIPTDATA_HPP
//#include "Systems/Cameras/Data/cmraScriptData.hpp"
//#endif
#ifndef CAM3D_MGR_HPP
#include "Tool/cam3d/cam3dMgr.hpp"
#endif
#ifndef RPN_OPERATIONS_HPP
#include "Features/RenderPanels/rpnOperations.hpp"
#endif

//============================================================================
//============================================================================
namespace
{
//	cmraCameraData l_CurData;

	const char *c_CamerasToolbarName = "Cameras";

	bool l_bSystemPageAdded		= false;

	// Callbacks for when camera list and director's cut list need
	// to be updated.
	class CameraListCallback : public camsCameraMgr::CameraListChangedCallback
	{
	public:
		virtual void CameraListChanged()
		{
	
#ifdef USE_WXWIDGETS
			if (rpnCameraChoice::Instance != NULL)
			{
				rpnCameraChoice::Instance->Update();
			}
#endif
		}
	};
	CameraListCallback l_CameraCallback;

	class DCutListCallback : public camsDirectorsCutMgr::DirectorsCutListChangedCallback
	{
	public:
		virtual void DirectorsCutListChanged()
		{

#ifdef USE_WXWIDGETS
			if (rpnCameraChoice::Instance != NULL)
			{
				rpnCameraChoice::Instance->Update();
			}
#endif
		}
	};
	DCutListCallback l_DCutCallback;

	// Callback when any system dialog requests data
	//
	class MySystemDialogInterest : public cmmSystemDialogInterest
	{
	public:
		//--------------------------------------------------------------------
		//	SceneDialogOpen - perform tasks (like adding tabs) relating
		//	to the scene/system dialog opening.  These tasks happen each time
		//	the scene dialog is launched.
		//--------------------------------------------------------------------
		//virtual 
		void SceneDialogOpen()
		{

#ifdef USE_WXWIDGETS
			if (rpnCameraChoice::Instance == NULL)
			{
				rpnDialogUtil::SetupCameraDialog();
			}
			rpnCameraChoice::Instance->Update();
#endif
		}

		//--------------------------------------------------------------------
		//	SceneDialogClose - perform tasks (like adding tabs) relating
		//	to the scene/system dialog closing.  These tasks happen each time
		//	the scene dialog is closed.
		//--------------------------------------------------------------------
		//virtual 
		void SceneDialogClose()
		{

		}
	};
	
	MySystemDialogInterest	l_SystemDialogInterest;

}	// end of namespace


//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  rpnDialogUtil::Init()
{
	SetupCameraDialog();

	camsCameraMgr::AddCallback(&l_CameraCallback);
	camsDirectorsCutMgr::AddCallback(&l_DCutCallback);
	cmmSystemDialogUtil::RegisterInterest(&l_SystemDialogInterest);
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  rpnDialogUtil::CleanUp()
{
	cmmSystemDialogUtil::UnRegisterInterest(&l_SystemDialogInterest);
	camsCameraMgr::RemoveCallback(&l_CameraCallback);
	camsDirectorsCutMgr::RemoveCallback(&l_DCutCallback);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
void rpnDialogUtil::SetupCameraDialog()
{

#ifdef USE_WXWIDGETS
	if (rpnCameraChoice::Instance == NULL)
	{
		rpnCameraChoice::Instance = new rpnCameraChoice(twxToolbarMgr::GetToolbarByName(c_CamerasToolbarName)); 
		rpnCameraChoice::Instance->Update();
		twxToolbarMgr::AddControlToToolBar(c_CamerasToolbarName, rpnCameraChoice::Instance);
	}
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const char* rpnDialogUtil::GetCamerasToolbarName()
{
	return c_CamerasToolbarName;
}

//--------------------------------------------------------------------
// When render panel changes its active camera, it calls this
// function to change the selected index in the camera list.
//--------------------------------------------------------------------
//static 
void rpnDialogUtil::HighlightCamera(camCamera *i_pCamera)
{
	int cam_index = -1;
	if (i_pCamera == &cam3dMgr::GetEditorCamera())
	{
		cam_index = 0;
	}
	else
	{
		int index = camsCameraMgr::GetIndexForCamera( i_pCamera );
		if (index >= 0)
			cam_index = (index+1);
		else
			cam_index = -1;
	}

#ifdef USE_WXWIDGETS
	if (rpnCameraChoice::Instance)
		rpnCameraChoice::Instance->SetSelectedIndex( cam_index );
#endif
}

//--------------------------------------------------------------------
// When render panel changes to a directors cut, it calls this
// function to change the selected index in the camera list.
//--------------------------------------------------------------------
void rpnDialogUtil::HighlightDirectorsCut(camsDirectorsCut *i_pDirectorsCut)
{
	int index = camsDirectorsCutMgr::GetIndexForDirectorsCut( i_pDirectorsCut );

	// Directors Cut indices start counting after all of the cameras
	int cam_index = -1;
	if (index >= 0)
		cam_index = (index+1+camsCameraMgr::GetNumCameras());


#ifdef USE_WXWIDGETS
	if (rpnCameraChoice::Instance)
		rpnCameraChoice::Instance->SetSelectedIndex( cam_index );
#endif

}

//--------------------------------------------------------------------
// Switch between editor camera and last selected scripted camera
//--------------------------------------------------------------------
void rpnDialogUtil::DoSwapCam()
{

#ifdef USE_WXWIDGETS
	if (rpnCameraChoice::Instance)
		rpnCameraChoice::Instance->DoSwapCam();
#endif
}
