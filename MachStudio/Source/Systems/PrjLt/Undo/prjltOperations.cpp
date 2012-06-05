/*****************************************************************************
**	prjltOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Systems/PrjLt/Undo/prjltActualOperations.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//============================================================================
//============================================================================
namespace prjltOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;
		int l_ObjectCounter = 0;

		const char *c_AddOperationDisplayName = "Add Projected Light";
		typedef cmmAddOperationTemplate< prjltScriptData, prjltActualOperations> prjltAddOperation;
		const char *c_DuplicateOperationDisplayName = "Duplicate Projected Light";

		const char *c_DeleteOperationDisplayName = "Delete Projected Light";
		typedef cmmDeleteOperationTemplate< prjltScriptData, prjltActualOperations> prjltDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "Projected Light Settings";
		typedef cmmSetOperationTemplate< prjltScriptData, prjltActualOperations, prjltObjectMgr> prjltSetOperation;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	PrjLt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	void  AddObject()
	{
		AddObject(prjltData::e_ProjectedLight);
	}
		
	//--------------------------------------------------------------------
	//  Add new shadow-casting light of given type
	//--------------------------------------------------------------------
	void  AddObject(prjltData::LightType i_LightType)
	{
		prjltScriptData default_data;
		default_data.m_BaseData.m_LightType = i_LightType;
		nameString objName;

		CreateNewObjectName( nameString(), objName, default_data.m_BaseData.m_LightType);
		default_data.m_BaseData.m_Name = objName;
		AddObject(default_data);
	}

	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	void  AddObject(const prjltScriptData& i_Data)
	{
		int index = prjltActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_AddOperationDisplayName <<" - " <<  i_Data.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		
		
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation(new prjltAddOperation(index, prjltObjectMgr::GetScriptData(index), displaytext.c_str()));
	}

	//--------------------------------------------------------------------
	// Add projected light at current camera position
	//--------------------------------------------------------------------
	void AddLightAtCameraPosition(prjltData::LightType i_LightType)
	{
		camCamera &current_cam = cam3dMgr::GetCamera();
		
		prjltScriptData new_data;
		new_data.m_BaseData.m_LightType = i_LightType;
		new_data.m_BaseData.m_Position = current_cam.GetPosition();
		new_data.m_BaseData.m_Target = current_cam.GetTarget();
		maRotation init_rotation;
		init_rotation.SetValue(maVector3d(0,0,1), current_cam.GetDirection());
		new_data.m_BaseData.m_Orientation.SetValue(init_rotation);
		AddObject(new_data);
	}

	//--------------------------------------------------------------------
	//  Select projected light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		prjltObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = prjltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			prjltObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = prjltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			prjltObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete projected light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, prjltObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_DeleteOperationDisplayName <<" - " << prjltObjectMgr::GetBaseData(i_Index).m_Name.GetString();
		std::string displaytext(oss.str());
		
		
		undoUndoMgr::AddOperation(new prjltDeleteOperation(i_Index, prjltObjectMgr::GetScriptData(i_Index), displaytext.c_str()));
		prjltActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index)
	{
		prjltScriptData clone_data = prjltObjectMgr::GetScriptData(i_Index);
		nameString org_name = clone_data.m_BaseData.m_Name.GetValue(); // preserve original name
		// clear out name, let object mgr make new one
		//clone_data.m_BaseData.m_Name = nameString(); 
		// Create new name for the duplicated object
		nameString basename, newname;
		if (PrefsMgr::Data().m_bDuplicateObjectsName.GetValue())
		{
			basename.SetString(clone_data.m_BaseData.m_Name.GetString());
		}
		CreateNewObjectName(basename, newname, clone_data.m_BaseData.m_LightType);
		clone_data.m_BaseData.m_Name = newname;
		int index = prjltActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();

		l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_DuplicateOperationDisplayName, org_name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_DuplicateOperationDisplayName <<" - " << org_name.GetString().c_str();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new prjltAddOperation(index, clone_data, displaytext.c_str()));
		
		prjltProjectedLightObject *new_obj = prjltObjectMgr::GetObject(index)->GetPickObject();

		// Find which light set the light was in and add the new one
		// to the same set
		nameString set_name;
		if (ltstLightSetMgr::GetSetNameFromLight(org_name, set_name))
		{
			DBG_ASSERT(new_obj, "DuplicateObject, do not have new light.");

			// Can't use the cloned data in order to get the new name, because it
			//	was probably changed when creating the object.
			ltstLightSetMgr::AddLightToSet(set_name, new_obj->GetName());
		}

		return new_obj->GetName();
	}

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual projected light properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const prjltData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			//char displaytext[128];
			//sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_Name.GetString().c_str());
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss <<c_SetOperationDisplayName <<" - " <<i_Data.m_Name.GetString();
			std::string displaytext(oss.str());
			
			l_LastOp = new prjltSetOperation(l_SelectedIndex, prjltObjectMgr::GetScriptData(l_SelectedIndex), displaytext.c_str() );
			undoUndoMgr::AddOperation(l_LastOp);
		}
		prjltActualOperations::SetBaseData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual projected light properties
	//--------------------------------------------------------------------
	void ChangeLightData(const prjltScriptData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			//char displaytext[128];
			//sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss <<c_SetOperationDisplayName <<" - " <<i_Data.m_BaseData.m_Name.GetString();
			std::string displaytext(oss.str());
			
			
			l_LastOp = new prjltSetOperation(l_SelectedIndex, prjltObjectMgr::GetScriptData(l_SelectedIndex), displaytext.c_str());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		prjltActualOperations::SetData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const prjltScriptData& i_Data)
	{
		// we could do undo here
		prjltActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}
	
	//------------------------------------------------ --------------------
	//  Reload the textures belonging to the selected projected light
	//--------------------------------------------------------------------
	void  ReloadTextures()
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		prjltScriptData light_data = prjltObjectMgr::GetScriptData(l_SelectedIndex);
		matTextureMgr::ReloadTexture(light_data.m_BaseData.m_ShaftTextureFilename.GetValue());
		matTextureMgr::ReloadTexture(light_data.m_BaseData.m_TextureFilename.GetValue());
	}

	//--------------------------------------------------------------------
	//  Reload the textures belonging to the selected projected light
	//--------------------------------------------------------------------
	void  ReloadTextures(const nameString& i_Name)
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		 int index = prjltObjectMgr::GetIndexForObject(i_Name);
		 if(index > -1)
		 {
			prjltScriptData light_data = prjltObjectMgr::GetScriptData(index);
			matTextureMgr::ReloadTexture(light_data.m_BaseData.m_ShaftTextureFilename.GetValue());
			matTextureMgr::ReloadTexture(light_data.m_BaseData.m_TextureFilename.GetValue());
		 }
	}

	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, 
							 nameString& o_NameString, 
							 prjltData::LightType i_LightType)
	{
		if (i_Filename.IsEmpty())
		{
			itString name_prefix;
			switch(i_LightType)
			{
			default:
			case prjltData::e_ProjectedLight:
				name_prefix = itString("ProjectedLight");
				break;
			case prjltData::e_SpotLight:
				name_prefix = itString("SpotLight");
				break;
			case prjltData::e_DirectionalLight:
				name_prefix = itString("DirectionalLight");
				break;
			}
			prjltObjectMgr::create_default_name(name_prefix, o_NameString, l_ObjectCounter);
		}
		else
		{
			prjltObjectMgr::create_duplicate_name(itString(i_Filename.GetString().c_str()), o_NameString);
		}
	}

}	// end of namespace
