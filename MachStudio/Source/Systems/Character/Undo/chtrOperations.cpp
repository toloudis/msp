/*****************************************************************************
**	chtrOperations.cpp
**
**	Utility for doing operations that are undoable in SystemCharacters
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationFull.hpp"
#include "Systems/Character/Drivers/chtrDriverCreator.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrActualOperations.hpp"
#include "Systems/Common/Gui/cmmAddDriverOperation.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"


//============================================================================
//============================================================================
namespace chtrOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;
		int l_ObjectCounter = 0;

		const char *c_AddOperationDisplayName = "Add Object";
		typedef cmmAddOperationTemplate< chtrScriptData, chtrActualOperations> chtrAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Object";
		typedef cmmDeleteOperationTemplate< chtrScriptData, chtrActualOperations> chtrDeleteOperation;

		// Maybe this should be different for what changed (i.e. Prop Position, etc.)
		const char *c_SetOperationDisplayName = "Object Settings";
		typedef cmmSetOperationTemplate< chtrScriptData, chtrActualOperations, chtrObjectMgr> chtrSetOperation;

		const char* c_MultipleLoadOperationName = "Load Objects";
		const char* c_GeometryDirCategory = "Geometry";
		
		//--------------------------------------------------------------------
		// browse_for_objects - allow multiple file selection to 
		// load new geometry
		//--------------------------------------------------------------------
		bool browse_for_objects(std::vector<fsLocator> &o_ChosenLocators) 
		{
			//i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
#ifdef HAIR_SUPPORTED
			std::string fileFilter = "Model Files (*.*x*;*.hair)|*.*x*;*.hair|All files (*.*)|*.*";
#else
			std::string fileFilter = "Model Files (*.*x*)|*.*x*|All files (*.*)|*.*";
#endif

			// Use the geometry directory category here
			return (guiFileDialogUtils::GetOpenFileNames(fileFilter, c_GeometryDirCategory, o_ChosenLocators));
		}
	}

	//------------------------------------------------------------------------
	// Let user search for objects to load.
	//------------------------------------------------------------------------
	void BrowseLoadObjects()
	{
		std::vector<fsLocator> browseFileNames;
		
		if( browse_for_objects( browseFileNames ) )
		{
			const int num_files = browseFileNames.size();
			if (num_files > 0)
				undoUndoMgr::BeginMultipleOperationBlock(c_MultipleLoadOperationName);
			for (int i=0; i<num_files; i++)
			{
				try
				{
					chtrOperations::AddObject( chtrScriptData( browseFileNames[i] ) );
				}catch (const g2dOutOfSystemMemoryX&)
				{
					// out of memory exception occurred
					// should we do any cleanup here?
					std::string msg = "Could not load geometry: Out of System Memory";
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Geometry Load Error");
					break;
				}
			}
			if (num_files > 0)
				undoUndoMgr::EndMultipleOperationBlock();
		}
	}

	//------------------------------------------------------------------------
	//  LoadAnimation onto the character with the given index.
	//------------------------------------------------------------------------
	void  LoadAnimation(int i_Index, const fsLocator &i_AnimLocator)
	{
		chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(i_Index);
		if (pCharacter)
		{
			// Create Animation Driver
			chtrDriverAnimationFull* pDriver = chtrDriverCreator::CreateAnimationDriver(pCharacter);
			if (pDriver)
			{
				pCharacter->AddDriver( pDriver );

				// GetDriverInfo returns a copy that we need to delete
				std::unique_ptr<tmlnDriverInfo> pInfo( pDriver->GetDriverInfo() );
				if (tmlnDriverAnimationFullInfo *pAnimInfo = dynamic_cast<tmlnDriverAnimationFullInfo*>(pInfo.get()))
				{
					pAnimInfo->m_Info.m_AnimFilename = i_AnimLocator;
					pAnimInfo->m_Info.m_bUseAnimStart = true;
					pDriver->SetDriverInfo( *pAnimInfo );

					// The SetDriverInfo call above is going to mess up the frame rate,
					// so restore it to the value from the animation file.
					pDriver->SetFrameRateFromAnimData();
				}

				// Create an undo operation for this driver
				undoUndoMgr::AddOperation( new cmmAddDriverOperation(pCharacter->CreateReferenceToSelf(), pDriver) );
				pCharacter->NotifyDriverChanged();

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
					
				chnlDialogUtil::ObjectSelected(pCharacter);
			}
		}
	}

	//--------------------------------------------------------------------
	//  Add new item to Character
	//--------------------------------------------------------------------
	void  AddObject(const chtrScriptData &i_Item)
	{
		int index = chtrActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_AddOperationDisplayName <<" - "<<i_Item.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation( new chtrAddOperation( index, chtrObjectMgr::GetScriptData(index), displaytext.c_str() ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Character
	//--------------------------------------------------------------------
	void  RemoveCharacter(int i_Index)
	{
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, chtrObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_DeleteOperationDisplayName <<" - "<<chtrObjectMgr::GetBaseData(i_Index).m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation( new chtrDeleteOperation( i_Index, chtrObjectMgr::GetScriptData(i_Index), displaytext.c_str() ) );
		chtrActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index)
	{
		chtrScriptData clone_data = chtrObjectMgr::GetScriptData(i_Index);

		nameString basename, newname;
		if (PrefsMgr::Data().m_bDuplicateObjectsName.GetValue())
		{
			basename.SetString(clone_data.m_BaseData.m_Name.GetString());
		}
		CreateNewObjectName(basename, newname);
		clone_data.m_BaseData.m_Name = newname;

		//// clear out name, let object mgr make new one
		//clone_data.m_BaseData.m_Name = nameString(); 
		int index = chtrActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[256];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_AddOperationDisplayName <<" - "<<clone_data.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		undoUndoMgr::AddOperation(new chtrAddOperation(index, clone_data, displaytext.c_str()));

		chtrObject *new_obj = chtrObjectMgr::GetObject(index)->GetPickObject();
		return new_obj->GetName();
	}

	//--------------------------------------------------------------------
	//  Reload geometry of character with given index
	//--------------------------------------------------------------------
	void  ReloadCharacter(int i_Index)
	{
		chtrObjectMgr::ReloadCharacter(i_Index);
	}

	//--------------------------------------------------------------------
	//  Select character with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		chtrObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			chtrObjectMgr::SelectObject(index, i_bAppend);
			SetSelectedIndex(index);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			chtrObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//	Select object part, like surface or material
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName, 
						  bool i_bAppend)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
			pCharacter->SelectObjectPart(i_PartName, i_CategoryName, i_bAppend);
			SetSelectedIndex(index);
		}
	}
	void ToggleObjectPartSelection(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
			pCharacter->ToggleObjectPartSelection(i_PartName, i_CategoryName);
			SetSelectedIndex(index);
		}

	}

	//--------------------------------------------------------------------
	//	ActivateObjectPart from Placed, represents a double-click
	//		on a part item in the placed menu.
	//--------------------------------------------------------------------
	void ActivateObjectPart(const nameString& i_Name, 
							const std::string i_PartName, 
							const std::string i_CategoryName)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
			pCharacter->ActivateObjectPart(i_PartName, i_CategoryName);
		}
	}

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	void DeleteObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName)
	{
		// we could do undo here
		chtrActualOperations::DeleteObjectPart(i_Name, i_PartName, i_CategoryName);
	}

	//--------------------------------------------------------------------
	//  Keep track of index of character being edited so that calls to
	//  ChangeCharacterData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const chtrScriptData& i_Data)
	{
		// we could do undo here
		chtrActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	void SetActiveRenderLayer(int i_Index, bool i_bActive)
	{
		chtrObjectMgr::SetActiveRenderLayer(i_Index, i_bActive);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of character with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		chtrObjectMgr::SetEditorVisible(i_Index, i_bVisible);
	}

	//--------------------------------------------------------------------
	//  Change geometry of character with given index
	//--------------------------------------------------------------------
	void  ChangeFilename(const nameString& i_Name, const fsLocator& i_Filename)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrActualOperations::SetFilename(index, i_Filename);
		}
	}

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			//char displaytext[256];
			//sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Name.c_str());
			std::ostringstream oss;
			oss << c_SetOperationDisplayName <<" - "<<i_Name;
			std::string displaytext(oss.str());
		
			l_LastOp = new chtrSetOperation(l_SelectedIndex, chtrObjectMgr::GetScriptData(l_SelectedIndex), displaytext.c_str());
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current name, replace the name string, and then set the name
		nameString name = chtrObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetValue();
		name.SetString(i_Name);
		chtrActualOperations::SetName(l_SelectedIndex, name);
	}

	//--------------------------------------------------------------------
	// Set the weight for the expression with the given name
	//--------------------------------------------------------------------
	/*void SetExpressionWeight(const std::string& i_Name, float i_Weight)
	{
		chtrActualOperations::SetExpressionWeight(l_SelectedIndex, i_Name, i_Weight);
	}*/

	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString)
	{
		if (i_Filename.IsEmpty())
		{
			chtrObjectMgr::create_default_name(itString("Object"), o_NameString, l_ObjectCounter);
		}
		else
		{
			chtrObjectMgr::create_duplicate_name(itString(i_Filename.GetString().c_str()), o_NameString);
		}
	}

}	// end of namespace
