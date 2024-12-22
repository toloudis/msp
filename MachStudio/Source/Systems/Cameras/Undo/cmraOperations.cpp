/*****************************************************************************
**	cmraOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Undo/cmraOperations.hpp"

#include "Systems/Cameras/Undo/cmraActualOperations.hpp"
#include "Systems/Cameras/Data/cmraFollowUtil.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScript.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCreator.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCut.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"

#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/Gui/cmmAddDriverOperation.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace cmraOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Camera";
		typedef cmmAddOperationTemplate< cmraScriptData, cmraActualOperations> cmraAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Camera";
		typedef cmmDeleteOperationTemplate< cmraScriptData, cmraActualOperations> cmraDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "Camera Settings";
		typedef cmmSetOperationTemplate< cmraScriptData, cmraActualOperations, cmraObjectMgr> cmraSetOperation;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	Ptlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	void  AddPerspectiveCamera()
	{
		AddObject(false);
	}
	void  AddOrthographicCamera()
	{
		AddObject(true);
	}
	void  AddObject(bool i_bOrthographic)
	{
		cmraScriptData default_data;
		//cmraFollowUtil::GetCurrentCamera(default_data.m_BaseData.m_Position, default_data.m_BaseData.m_Target);
		cmraFollowUtil::GetCurrentCamera( (default_data.m_BaseData) );
		default_data.m_BaseData.m_bOrthographic = i_bOrthographic;

		//	set-up a new name
		nameString basename, newname;
		cmraObjectMgr::GenerateNewCameraName( basename, newname, i_bOrthographic );
		default_data.m_BaseData.m_Name = newname;

		//
		int index = cmraActualOperations::AddObject(default_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, default_data.m_BaseData.m_Name.GetString().c_str());
		
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << c_AddOperationDisplayName <<" - " << default_data.m_BaseData.m_Name.GetString().c_str() ;
		std::string displaytext(oss.str());
		
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation(new cmraAddOperation(index, cmraObjectMgr::GetScriptData(index), displaytext.c_str()));
	}

	//------------------------------------------------------------------------
	//  LoadAnimation onto the character with the given index.
	//------------------------------------------------------------------------
	void  LoadAnimation(int i_Index, const fsLocator &i_AnimLocator)
	{
		cmraScriptObject *pCamera = cmraObjectMgr::GetObject(i_Index);
		if (pCamera)
		{
			// Create Animation Driver
			cmraDriverMayaScript* pDriver = cmraDriverCreator::CreateAnimationDriver(pCamera);
			if (pDriver)
			{
				pCamera->AddDriver( pDriver );

				// GetDriverInfo returns a copy that we need to delete
				std::unique_ptr<tmlnDriverInfo> pInfo( pDriver->GetDriverInfo() );
				if (cmraDriverMayaScriptInfo *pAnimInfo = dynamic_cast<cmraDriverMayaScriptInfo*>(pInfo.get()))
				{
					pAnimInfo->m_AnimFilename = i_AnimLocator;
					pAnimInfo->m_bUseAnimStart = false;
					pAnimInfo->m_bDriverToAnimLength = false;
					pDriver->SetDriverInfo( *pAnimInfo );

					// The SetDriverInfo call above is going to mess up the frame rate,
					// so restore it to the value from the animation file.
					pDriver->SetFrameRateFromAnimData();

					// There is a problem here where the SetDriverInfo thinks the begin and
					// end time are correct, but we really wanted it to resize to the driver
					// anim length. So, we are going have it off on the SetDriverInfo and
					// then turn it on by hand afterwards.
					pDriver->SetDriverToAnimLengthFlag(true);
					pDriver->SetUseAnimStartFlag(true);

					// Now check to see if camera animtion had stereo keys also.
					// If so, attach the driver to the stereo channels
					if (pDriver->HasStereoAnimation())
					{
						pDriver->AttachToStereoParams(&(pCamera->StereoFDChannel()), &(pCamera->StereoIODChannel()));
						// Let the driver attach to the channels
						//pCamera->StereoFDChannel().AddDriver( pDriver );
						//pCamera->StereoIODChannel().AddDriver( pDriver );
					}
				}

				// Create an undo operation for this driver
				undoUndoMgr::AddOperation( new cmmAddDriverOperation(pCamera->CreateReferenceToSelf(), pDriver) );
				pCamera->NotifyDriverChanged();

				// Make sure timeline displays the new driver,
				// Note, this needs to be part of the undo operation also
				maTime min_time = tmlnTimeLine::GetMinimum();
				maTime max_time = tmlnTimeLine::GetMaximum();
				if (pDriver->GetEndTime() > max_time)
				{
					max_time = pDriver->GetEndTime();
					tmlnTimeLine::SetTimeRange(min_time, max_time);
				}
				if (pDriver->GetBeginTime() < min_time)
				{
					min_time = pDriver->GetBeginTime();
					tmlnTimeLine::SetTimeRange(min_time, max_time);
				}
					
				chnlDialogUtil::ObjectSelected(pCamera);
			}
		}
	}


	//--------------------------------------------------------------------
	//  Select camera with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		cmraObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = cmraObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			cmraObjectMgr::SelectObject(index, i_bAppend);
		}
		else if (i_Name.GetString() == cmraObjectMgr::GetEditorCameraObjectName())
		{
			sel3dMgr::CreateUndoOperation();
			cmraObjectMgr::SelectEditorCameraObject(i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = cmraObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			cmraObjectMgr::DeselectObject(index);
		}
		else if (i_Name.GetString() == cmraObjectMgr::GetEditorCameraObjectName())
		{
			sel3dMgr::CreateUndoOperation();
			cmraObjectMgr::DeselectEditorCameraObject();
		}
	}

	//--------------------------------------------------------------------
	//  Select follow camera to given index
	//--------------------------------------------------------------------
	void  SelectFollowCamera(int i_Index)
	{
		camsFollowUtil::SetFollowIndex(i_Index);

		// Select the camera also when changing to scripted camera
		if (i_Index >= 0)
		{
			if (i_Index < cmraObjectMgr::GetNumObjects())
			{
				sel3dMgr::CreateUndoOperation();
				cmraObjectMgr::SelectObject(i_Index);
			}
			else
			{
				// Kind of tricky to select directors cut object, since
				// it isn't in this system.
				int dcut_ind = i_Index - cmraObjectMgr::GetNumObjects();
				if (dcut_ind < camsDirectorsCutMgr::GetNumDirectorsCuts())
				{
					camsDirectorsCut* pDCut = camsDirectorsCutMgr::GetDirectorsCut(dcut_ind);
					sel3dObject *pPickObj = dynamic_cast<sel3dObject*>(pDCut);
					if (pPickObj)
					{
						sel3dMgr::CreateUndoOperation();
						sel3dMgr::Select(pPickObj);
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//  Delete camera with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, cmraObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << c_DeleteOperationDisplayName <<" - " <<  cmraObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str() ;
		std::string displaytext(oss.str());
		undoUndoMgr::AddOperation(new cmraDeleteOperation(i_Index, cmraObjectMgr::GetScriptData(i_Index), displaytext.c_str()));

		cmraActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index)
	{
		cmraScriptData clone_data = cmraObjectMgr::GetScriptData(i_Index);

		// clear out name, let Object Mgr make new one
		nameString basename, newname;
		if (PrefsMgr::Data().m_bDuplicateObjectsName.GetValue())
		{
			basename.SetString(clone_data.m_BaseData.m_Name.GetString());
		}
		cmraObjectMgr::GenerateNewCameraName( basename, newname, 
				clone_data.m_BaseData.m_bOrthographic.GetValue() );
		clone_data.m_BaseData.m_Name = newname;

		//	add the new object
		int index = cmraActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << c_AddOperationDisplayName << " - " <<  clone_data.m_BaseData.m_Name.GetString().c_str() ;
		std::string displaytext(oss.str());
		undoUndoMgr::AddOperation(new cmraAddOperation(index, clone_data, displaytext.c_str()));

		cmraCameraObject *new_obj = cmraObjectMgr::GetObject(index)->GetPickObject();
		return new_obj->GetName();
	}

	//--------------------------------------------------------------------
	//  Keep track of index of camera being edited so that calls to
	//  ChangeCameraData affect the right camera
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual basic cmraerties
	//--------------------------------------------------------------------
	void ChangeBaseData(const cmraCameraData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			//char displaytext[256];
			//sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_Name.GetString().c_str());
			
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss << c_SetOperationDisplayName << " - " << i_Data.m_Name.GetString().c_str();
			std::string displaytext(oss.str());
			
			l_LastOp = new cmraSetOperation(l_SelectedIndex, cmraObjectMgr::GetScriptData(l_SelectedIndex), displaytext.c_str());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		cmraActualOperations::SetBaseData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual camera driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const cmraScriptData& i_Data)
	{
		// we could do undo here
		cmraActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Change the Description
	//--------------------------------------------------------------------
	void ChangeDescription(const std::string& i_Description)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			//char displaytext[256];
			//sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, cmraObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetString().c_str());
			
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss << c_SetOperationDisplayName << " - " <<  cmraObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetString().c_str();
			std::string displaytext(oss.str());
			l_LastOp = new cmraSetOperation(l_SelectedIndex, cmraObjectMgr::GetScriptData(l_SelectedIndex), displaytext.c_str());
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current Description, replace the Description string, and then set the Description
		nameString Description;// = cmraObjectMgr::GetBaseData(l_SelectedIndex).m_Description;
		Description.SetString(i_Description.c_str());
		cmraActualOperations::SetDescription(l_SelectedIndex, Description);
	}

}	// end of namespace
