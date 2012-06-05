/*****************************************************************************
**	rpnOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "Features/RenderPanels/rpnOperations.hpp"
#include "Features/RenderPanels/GUI/rpnDialogUtil.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"

#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"

// Includes from old managed file
#define RPN_PANELLAYOUT_HPP
#ifndef RPN_RENDERPANE_HPP
#include "Features/RenderPanels/rpnRenderPane.hpp"
#endif

//============================================================================
//============================================================================
namespace rpnOperations
{
	namespace
	{
	}	// end of namespace

	//--------------------------------------------------------------------
	//  Select camera object in active view
	//--------------------------------------------------------------------
	void  SelectActiveCamera()
	{

#ifdef USE_WXWIDGETS
		if (rpnPanelGrid::Instance)
		{
			for (int i=0; i<4; i++)
			{
				rpnRenderPanel *pRenderPane = rpnPanelGrid::Instance->GetRenderPanel(i);
				if (pRenderPane != NULL && pRenderPane->IsActiveView())
				{
					pRenderPane->SelectCamera();
				}
			}
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Select camera object
	//--------------------------------------------------------------------
	void  SelectCamera(int i_Index)
	{
		camsCameraMgr::SelectCamera(i_Index);
	}
	//--------------------------------------------------------------------
	//  Set selected index in camera list GUI to this camera
	//--------------------------------------------------------------------
	void  HighlightCamera(camCamera *i_pCamera)
	{
		rpnDialogUtil::HighlightCamera(i_pCamera);
	}

	//--------------------------------------------------------------------
	//  Change camera in the active render pane
	//--------------------------------------------------------------------
	void  ChangeCamera(gpxCamera *i_pCamera, const std::string& i_Label)
	{


#ifdef USE_WXWIDGETS
		if (rpnPanelGrid::Instance)
		{
			for (int i=0; i<4; i++)
			{
				rpnRenderPanel *pRenderPane = rpnPanelGrid::Instance->GetRenderPanel(i);
				if (pRenderPane != NULL && pRenderPane->IsActiveView())
				{
					pRenderPane->SetCamera( i_pCamera, i_Label );
				}
			}
		}
#else
		tma3dRenderView *pRenderView = tma3dRenderView::GetActiveRenderView();
		if (pRenderView)
		{
			pRenderView->SetCameraProxy( i_pCamera );

			// Set our camera into cam3dMgr in order to activate the 
			//	camera manipulator
			pRenderView->ActivateCameraManipulator();
		}
#endif


	}

	//--------------------------------------------------------------------
	// Change camera in active render pane to the editor camera.
	// Used to localize the "Editor" string name.
	//--------------------------------------------------------------------
	void ChangeCameraToEditor()
	{
		ChangeCamera(cam3dMgr::GetEditorCameraProxy(), "Editor");
	}

	//--------------------------------------------------------------------
	// Set editor camera to current view's position
	//--------------------------------------------------------------------
	void SetEditCam()
	{
		//	set the editor cam to the current camera settings
		//
		camCamera* pCurCam = &cam3dMgr::GetCamera();
		camCamera* pEditCam = &cam3dMgr::GetEditorCamera();

		if (pCurCam != pEditCam)
		{
			//// should be able to just use operator=() to copy over all fields
			//(*pEditCam) = (*pCurCam);

			//// The operator=() function copies the callbacks also, so
			//// we need to clear them out of the editor camera.
			//pEditCam->ClearCallbacks();

			// Use the cameras system to do the set edit camera.
			int cam_index = camsCameraMgr::GetIndexForCamera(pCurCam);
			if (cam_index >= 0)
			{
				// bga - we don't want the "original values" of the camera, we want
				// current animated values, which are in the property object directly:
				//cmraCameraData cam_data = cmraObjectMgr::GetBaseData(cam_index);
				cmraCameraData cam_data = cmraObjectMgr::GetPickObject(cam_index)->GetData();

				cmraCameraObject* pEditCamObject = cmraObjectMgr::GetEditorCameraObject();
				cam_data.m_Name = pEditCamObject->GetPropertyName().GetValue();
				cam_data.m_Description = pEditCamObject->GetPropertyDescription().GetValue();
				pEditCamObject->SetData( cam_data );
			}
			
			ChangeCameraToEditor();
		}
	}

	//--------------------------------------------------------------------
	// Move view to next or previous camera in the list
	//--------------------------------------------------------------------
	void NextCamera()
	{
		int index = camsCameraMgr::GetIndexForCamera( &cam3dMgr::GetCamera() );
		index++;
		if (index >= 0 && index < camsCameraMgr::GetNumCameras())
		{
			nameString camName;
			camsCameraMgr::GetCameraName(index, camName);
			ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
			camsCameraMgr::SelectCamera(index);
		}
		else
		{
			ChangeCameraToEditor();
		}

	}
	void PreviousCamera()
	{
		int index = camsCameraMgr::GetIndexForCamera( &cam3dMgr::GetCamera() );
		const int num_cameras = camsCameraMgr::GetNumCameras();
		if (index < 0)
			index = num_cameras-1;
		else
			index--;

		if (index >= 0 && index < camsCameraMgr::GetNumCameras())
		{
			nameString camName;
			camsCameraMgr::GetCameraName(index, camName);
			ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
			camsCameraMgr::SelectCamera(index);
		}
		else
		{
			ChangeCameraToEditor();
		}

	}

	//--------------------------------------------------------------------
	//  Select directors cut object
	//--------------------------------------------------------------------
	void  SelectDirectorsCut(camsDirectorsCut *i_pDirectorsCut)
	{
		int index = camsDirectorsCutMgr::GetIndexForDirectorsCut( i_pDirectorsCut );
		if (index >= 0 && index < camsDirectorsCutMgr::GetNumDirectorsCuts())
		{
			camsDirectorsCutMgr::SelectDirectorsCut(index);
		}
	}

	//--------------------------------------------------------------------
	//  Set selected index in camera list GUI to this directors cut
	//--------------------------------------------------------------------
	void  HighlightDirectorsCut(camsDirectorsCut *i_pDirectorsCut)
	{
		rpnDialogUtil::HighlightDirectorsCut(i_pDirectorsCut);
	}

	//--------------------------------------------------------------------
	//  Change directors cut in the active render pane
	//--------------------------------------------------------------------
	void  ChangeDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label)
	{

#ifdef USE_WXWIDGETS
		if (rpnPanelGrid::Instance)
		{
			for (int i=0; i<4; i++)
			{
				rpnRenderPanel *pRenderPane = rpnPanelGrid::Instance->GetRenderPanel(i);
				if (pRenderPane != NULL && pRenderPane->IsActiveView())
				{
					pRenderPane->SetDirectorsCut( i_pDirectorsCut, i_Label );
				}
			}
		}
#endif
	}

}	// end of namespace
