/*****************************************************************************
**	dynOperations.cpp
**
**	Interface for dialogs to change control info
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/GUI/dynOperations.hpp"

#include "Support/dyn/dynControlData.hpp"
#include "Support/dyn/dynScriptObject.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"

//============================================================================
//============================================================================
namespace dynOperations
{
	namespace
	{
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
										 nodeInfo.GetList(), 
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
		// needs undo operation
		i_pObject->AddControl(i_Data);
	}

	
	//--------------------------------------------------------------------
	//	Delete control with given name.
	//--------------------------------------------------------------------
	void  DeleteControl(dynScriptObject* i_pObject,
						const std::string& i_PartName)
	{
		// needs undo operation
		const bool bDeleteDrivers = true;
		i_pObject->DeleteControl(i_PartName, bDeleteDrivers);
	}
}	// end of namespace
