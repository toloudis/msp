/*****************************************************************************
**	cmraOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Undo/cmraOperations.hpp"

#include "Systems/Cameras/Undo/cmraActualOperations.hpp"
#include "Systems/Cameras/Data/cmraFollowUtil.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCut.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"


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
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, default_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new cmraAddOperation(index, default_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select camera with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		cmraObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = cmraObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			cmraObjectMgr::SelectObject(index, i_bAppend);
		}
		else if (i_Name.GetString() == cmraObjectMgr::GetEditorCameraObjectName())
		{
			cmraObjectMgr::SelectEditorCameraObject(i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = cmraObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			cmraObjectMgr::DeselectObject(index);
		}
		else if (i_Name.GetString() == cmraObjectMgr::GetEditorCameraObjectName())
		{
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
					pick3dPickObject *pPickObj = dynamic_cast<pick3dPickObject*>(pDCut);
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
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, cmraObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new cmraDeleteOperation(i_Index, cmraObjectMgr::GetScriptData(i_Index), displaytext));

		cmraActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		cmraScriptData clone_data = cmraObjectMgr::GetScriptData(i_Index);

		// clear out name, let Object Mgr make new one
		nameString basename, newname;
		cmraObjectMgr::GenerateNewCameraName( basename, newname, 
			clone_data.m_BaseData.m_bOrthographic.GetValue() );
		clone_data.m_BaseData.m_Name = newname;

		//	add the new object
		int index = cmraActualOperations::AddObject(clone_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new cmraAddOperation(index, clone_data, displaytext));
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
			char displaytext[128];
			sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_Name.GetString().c_str());
			l_LastOp = new cmraSetOperation(l_SelectedIndex, cmraObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
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
			char displaytext[128];
			sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, cmraObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetString().c_str());
			l_LastOp = new cmraSetOperation(l_SelectedIndex, cmraObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current Description, replace the Description string, and then set the Description
		nameString Description;// = cmraObjectMgr::GetBaseData(l_SelectedIndex).m_Description;
		Description.SetString(i_Description.c_str());
		cmraActualOperations::SetDescription(l_SelectedIndex, Description);
	}

}	// end of namespace
