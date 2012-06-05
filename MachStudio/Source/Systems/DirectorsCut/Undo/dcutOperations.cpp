/*****************************************************************************
**	dcutOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/DirectorsCut/Undo/dcutOperations.hpp"

#include "Systems/DirectorsCut/Undo/dcutActualOperations.hpp"
//#include "dcutFollowUtil.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"

#include "Support/cams/camsFollowUtil.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace dcutOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add DirectorsCut";
		typedef cmmAddOperationTemplate< dcutScriptData, dcutActualOperations> dcutAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete DirectorsCut";
		typedef cmmDeleteOperationTemplate< dcutScriptData, dcutActualOperations> dcutDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "DirectorsCut Settings";
		typedef cmmSetOperationTemplate< dcutScriptData, dcutActualOperations, dcutObjectMgr> dcutSetOperation;

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
	void  AddObject()
	{
		dcutScriptData default_data;
		//dcutFollowUtil::GetCurrentCamera(default_data.m_BaseData.m_Position, default_data.m_BaseData.m_Target);
		//dcutFollowUtil::GetCurrentCamera( (default_data.m_BaseData) );

		int index = dcutActualOperations::AddObject(default_data);

		l_LastOp = NULL;

		std::string displayText(c_AddOperationDisplayName);
		if(default_data.m_BaseData.m_Name.GetString() != "")
			displayText += " - ";
		displayText += default_data.m_BaseData.m_Name.GetString();

		undoUndoMgr::AddOperation(new dcutAddOperation(index, default_data, displayText.c_str()));
	}

	//--------------------------------------------------------------------
	//  Select camera with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		dcutObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = dcutObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			dcutObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = dcutObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			dcutObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//	Select the camera and set the data (no matter what)
	//--------------------------------------------------------------------
	//void SelectObjectAndSetData( int i_Index )
	//{
	//	sel3dMgr::CreateUndoOperation();
	//	dcutObjectMgr::SelectObject(i_Index);
	//	dcutActualOperations::UpdateCameraData( i_Index );
	//}

	//--------------------------------------------------------------------
	//  Select follow camera to given index
	//--------------------------------------------------------------------
	void  SelectFollowCamera(int i_Index)
	{
		camsFollowUtil::SetFollowIndex(i_Index);

		// Select the camera also when changing to scripted camera
		if (i_Index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			dcutObjectMgr::SelectObject(i_Index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete camera with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		
		std::string displayText(c_DeleteOperationDisplayName);
		if(dcutObjectMgr::GetBaseData(i_Index).m_Name.GetString() != "")
			displayText += " - ";
		displayText += dcutObjectMgr::GetBaseData(i_Index).m_Name.GetString();

		undoUndoMgr::AddOperation(new dcutDeleteOperation(i_Index, dcutObjectMgr::GetScriptData(i_Index), displayText.c_str()));
		dcutActualOperations::DeleteObject(i_Index);
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
	// Update individual basic dcuterties
	//--------------------------------------------------------------------
	void ChangeBaseData(const dcutCueData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			std::string displayText(c_SetOperationDisplayName);
			if(i_Data.m_Name.GetString() != "")
				displayText += " - ";
			displayText += i_Data.m_Name.GetString();

			l_LastOp = new dcutSetOperation(l_SelectedIndex, dcutObjectMgr::GetScriptData(l_SelectedIndex), displayText.c_str());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dcutActualOperations::SetBaseData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual camera properties
	//--------------------------------------------------------------------
	void ChangeCameraData(const dcutScriptData& i_Data)
	{
		if ( l_SelectedIndex >= 0 )
		{
			if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
			{
				std::string displayText(c_SetOperationDisplayName);
				if(i_Data.m_BaseData.m_Name.GetString() != "")
					displayText += " - ";
				displayText += i_Data.m_BaseData.m_Name.GetString();

				l_LastOp = new dcutSetOperation(l_SelectedIndex, dcutObjectMgr::GetScriptData(l_SelectedIndex), displayText.c_str());
				undoUndoMgr::AddOperation(l_LastOp);
			}
		}

		dcutActualOperations::SetData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual camera driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const dcutScriptData& i_Data)
	{
		// we could do undo here
		dcutActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Change the Description
	//--------------------------------------------------------------------
	//void ChangeDescription(const std::string& i_Description)
	//{
	//	if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
	//	{
	//		char displaytext[128];
	//		sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, dcutObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetString().c_str());
	//		l_LastOp = new dcutSetOperation(l_SelectedIndex, dcutObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
	//		undoUndoMgr::AddOperation(l_LastOp);
	//	}

	//	//	get the current Description, replace the Description string, and then set the Description
	//	nameString Description;// = dcutObjectMgr::GetBaseData(l_SelectedIndex).m_Description;
	//	Description.SetString(i_Description.c_str());
	//	dcutActualOperations::SetDescription(l_SelectedIndex, Description);
	//}

}	// end of namespace
