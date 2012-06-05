/*****************************************************************************
**	dynOperations.cpp
**
**	Interface for dialogs to change control info
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/GUI/dynOperations.hpp"

#include "Support/dyn/dynControlData.hpp"
#include "Support/dyn/GUI/dynCreateOperation.hpp"
#include "Support/dyn/GUI/dynDeleteOperation.hpp"
#include "Support/dyn/dynScriptObject.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"

//============================================================================
//============================================================================
namespace dynOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Create Control";
		const char *c_DeleteOperationDisplayName = "Delete Control";

		void (*l_UpdatePlacedFunction)() = NULL;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	Prompt user for where to attach new control.
	//--------------------------------------------------------------------
	void  CreateNewControl(dynScriptObject* i_pObject)
	{				
		// Get list of nodes that can be attached to
		std::vector<std::string> ref_names;
		i_pObject->GetReferenceList(ref_names);
		if (ref_names.empty())
		{
			guiMessageBox::Show("No joints available to attach to.", "No nodes for controls");
			return;
		}


		// Configuration
		prtyText controlName("Control Name", "");
		prtyText controlNode("Control Node", ref_names[0]);

		// Setup dialog
		prtyPropertyUIInfoContainer nodeInfo;
		nodeInfo.Add(new prtyTextBoxUIInfo(&controlName));
		prtyComboBoxUIInfo *pCBUII = new prtyComboBoxUIInfo(&controlNode);
		pCBUII->m_List = ref_names;
		nodeInfo.Add(pCBUII);

		if (guiPropertyDialog::ShowModal("Attach new joint control", 
										 nodeInfo, 
										 "Choose name for control and node to attach to") == guiPropertyDialog::e_OK)
		{
			dynControlData data;
			data.m_Name = (controlName.GetValue().empty()) ? controlNode.GetValue() : controlName.GetValue();
			data.m_Node = controlNode.GetValue();
			CreateControl(i_pObject, data);
		}
	}

	//--------------------------------------------------------------------
	//	Create control based on given data
	//--------------------------------------------------------------------
	void  CreateControl(dynScriptObject* i_pObject,
						const dynControlData& i_Data)
	{
		i_pObject->AddControl(i_Data);
		undoUndoMgr::AddOperation(new dynCreateOperation(i_pObject, i_Data.m_Name.GetValue(), c_AddOperationDisplayName));
	}

	
	//--------------------------------------------------------------------
	//	Delete control with given name.
	//--------------------------------------------------------------------
	void  DeleteControl(dynScriptObject* i_pObject,
						const std::string& i_PartName)
	{
		// make the undo operation before the data is removed from the system
		undoUndoMgr::AddOperation(new dynDeleteOperation(i_pObject, i_PartName, c_DeleteOperationDisplayName));

		const bool bDeleteDrivers = true;
		i_pObject->DeleteControl(i_PartName, bDeleteDrivers);
	}

	//--------------------------------------------------------------------
	// Set the function pointer to update the scene dialog when a control
	// has gone through an undo/redo
	//--------------------------------------------------------------------
	void SetUpdatePlacedFunction(void (*i_CallbackFunction)())
	{
		l_UpdatePlacedFunction = i_CallbackFunction;
	}

	//--------------------------------------------------------------------
	// Call the update placed function
	//--------------------------------------------------------------------
	void UpdatePlacedDialog()
	{
		if(l_UpdatePlacedFunction)
			l_UpdatePlacedFunction();
	}
}	// end of namespace
