/*****************************************************************************
**	trfnOperations.cpp
**
**	Utility for operations that are undoable in trfn system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Undo/trfnOperations.hpp"
#include "Systems/Transforms/Undo/trfnActualOperations.hpp"
#include "Systems/Transforms/Undo/trfnObjectOperation.hpp"
#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"
#include "Systems/Common/GUI/cmmSceneOperations.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Core/undo/undoMultipleOperationBlock.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
namespace trfnOperations
{
	namespace
	{
		int l_ObjectCounter = 0;

		const char *c_AddOperationDisplayName = "Add Parent";
		typedef cmmAddOperationTemplate< trfnScriptData, trfnActualOperations> trfnAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Parent";
		typedef cmmDeleteOperationTemplate< trfnScriptData, trfnActualOperations> trfnDeleteOperation;

		const char *c_DuplicateOperationDisplayName = "Duplicate Parent";

		void get_components_from_selection(std::list<nameString> &o_SelectedNames)
		{
			const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList(); 
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
				{
					nameString item_name = pNamedObject->GetName();
					o_SelectedNames.push_back( item_name );
				}
			}
		}

	}	// end of namespace


//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new transform
	//--------------------------------------------------------------------
	void  AddObject()
	{
		trfnScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const trfnScriptData& i_Data)
	{
		int index = trfnActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_Name.GetString().c_str());
		// Need new name given to new object when making udo operation
		std::ostringstream oss;
		oss << c_AddOperationDisplayName <<" - "<<i_Data.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new trfnAddOperation(index, trfnObjectMgr::GetScriptData(index), displaytext.c_str()));
	}

	//--------------------------------------------------------------------
	//  Select transform with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		trfnObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = trfnObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			trfnObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = trfnObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			trfnObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		nameString parent_name = trfnObjectMgr::GetName(i_Index);
		// For now, cannot delete tranforms that have children.
		// User will have to delete the children first.
		std::vector<nameString> children;
		xfrmTransformMgr::GetNodesInTransform(parent_name, children);
		if (children.empty())
		{
			//char displaytext[128];
			//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, trfnObjectMgr::GetData(i_Index).m_Name.GetString().c_str());
			std::ostringstream oss;
			oss << c_DeleteOperationDisplayName <<" - "<<trfnObjectMgr::GetBaseData(i_Index).m_Name.GetString();
			std::string displaytext(oss.str());
			
			undoUndoMgr::AddOperation(new trfnDeleteOperation(i_Index, trfnObjectMgr::GetScriptData(i_Index), displaytext.c_str()));
			trfnActualOperations::DeleteObject(i_Index);
		}
		else
		{
			std::string msg = "Parent nodes with children cannot be deleted. Remove or delete the children first";
			guiMessageBox::Show(msg.c_str(), "Cannot delete parent node");
		}
	}

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index,
								std::map<nameString, nameString> &o_DuplicateNameMap)
	{
		trfnScriptData clone_data = trfnObjectMgr::GetScriptData(i_Index);
		nameString org_name = clone_data.m_BaseData.m_Name.GetValue(); // preserve original name
		// clear out name, let object mgr make new one
		//clone_data.m_BaseData.m_Name = nameString(); 
		// Create new name for the duplicated object
		nameString basename, newname;
		if (PrefsMgr::Data().m_bDuplicateObjectsName.GetValue())
		{
			basename.SetString(clone_data.m_BaseData.m_Name.GetString());
		}
		CreateNewObjectName(basename, newname);
		clone_data.m_BaseData.m_Name = newname;

		// Make a copy of the names of the children of the original object.
		// We will make a duplicate of each child and then add their new names
		// as the children of the new duplicated parent node.
		std::vector<nameString> children = clone_data.m_BaseData.m_Objects;
		clone_data.m_BaseData.m_Objects.clear();
		for (int i=0; i<children.size(); ++i)
		{
			nameString dup_name = cmmSceneOperations::DuplicateObject(children[i], o_DuplicateNameMap);
			if (!dup_name.IsEmpty())
				clone_data.m_BaseData.m_Objects.push_back(dup_name);
		}

		int index = trfnActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();	// if there is a problem loading the object, return immediately

		std::string displayText(c_DuplicateOperationDisplayName);
		if(org_name.GetString() != "")
			displayText += " - ";
		displayText += org_name.GetString();

		undoUndoMgr::AddOperation(new trfnAddOperation(index, clone_data, displayText.c_str()));

		trfnTransformObject *new_obj = trfnObjectMgr::GetObject(index)->GetPickObject();
		return new_obj->GetName();
	}

	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString)
	{
		if (i_Filename.IsEmpty())
		{
			trfnObjectMgr::create_default_name(itString("Parent"), o_NameString, l_ObjectCounter);
		}
		else
		{
			trfnObjectMgr::create_duplicate_name(itString(i_Filename.GetString().c_str()), o_NameString);
		}
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddNodeToTransform(const nameString& i_TransformName, 
							  const nameString& i_ObjectName)
	{
		// Need to make sure that the object is not a parent node of the
		// given transform (would make a circular graph)
		if (!xfrmTransformMgr::IsAncestorOfNode(i_TransformName, i_ObjectName))
		{
			undoUndoMgr::AddOperation(new trfnAddObjectOperation(i_ObjectName));
			trfnActualOperations::AddNodeToTransform(i_TransformName, i_ObjectName);
		}
		else
		{
			std::string msg = "Could not add node " + i_ObjectName.GetString() + " to parent " + i_TransformName.GetString();
			guiMessageBox::Show(msg.c_str(), "Potential cycle in graph");
		}
	}

	//--------------------------------------------------------------------
	//	Remove object from its parent
	//--------------------------------------------------------------------
	void  RemoveNodeFromParent(const nameString& i_ObjectName)
	{
		nameString parent_name;
		if (xfrmTransformMgr::GetParentForNode(i_ObjectName, parent_name))
		{
			undoUndoMgr::AddOperation(new trfnRemoveObjectOperation(i_ObjectName));
			trfnActualOperations::RemoveNodeFromParent(i_ObjectName);
		}
	}

	//--------------------------------------------------------------------
	//  Create new parent transform from selection
	//--------------------------------------------------------------------
	void  CreateTransformFromSelection()
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;

		// Use helper to begin and end the multiple undo operation block
		undoMultipleOperationBlock multiple_undo_block("Create Parent from Selection");

		std::list<nameString> selected_names;
		get_components_from_selection(selected_names);

		// Look to see if all nodes have the same parent.
		// If so, insert new parent in the hierarchy at same place.
		bool bHasCommonParent = false;
		nameString common_parent;
		std::list<nameString>::const_iterator it;
		for (it = selected_names.begin(); it != selected_names.end(); ++it)
		{
			nameString parent_name;
			if (xfrmTransformMgr::GetParentForNode(*it, parent_name))
			{
				if (common_parent.IsEmpty())
				{
					common_parent = parent_name;
					bHasCommonParent = true;
				}
				else if (parent_name != common_parent)
				{
					bHasCommonParent = false;
					break;
				}
			}
			else
			{
				bHasCommonParent = false;
				break;
			}
		}

		// Clear the selection as part of the multiple undo block
		// so that the old selection can be restored when undoing the operation.
		sel3dMgr::CreateUndoOperation();
		sel3dMgr::ClearSelection();
		
		// Create new parent
		AddObject();

		// Get index of just created parent
		int index = trfnObjectMgr::GetNumObjects()-1;
		if (index >= 0)
		{
			nameString parent = trfnObjectMgr::GetName(index);

			// If we are inserting the new parent into the hierarchy at the 
			// point of the children, do it now
			if (bHasCommonParent)
				AddNodeToTransform(common_parent, parent);

			// As each node is moved, it will be checked against the 
			// the hierarchy to prevent circular links within AddNodeToTransform
			std::list<nameString>::const_iterator nit;
			for (nit = selected_names.begin(); nit != selected_names.end(); ++nit)
			{
				AddNodeToTransform(parent, *nit);
			}

			// For convenience, center the pivot on the objects that were just added
			trfnObjectMgr::GetPickObject(index)->CenterPivot();
		}
	}

	//--------------------------------------------------------------------
	//	Add selected objects to given parent transform
	//--------------------------------------------------------------------
	void  AddSelectedToTransform(const nameString& i_TransformName)
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;

		// Make sure that the given name is the name of a transform node in our system
		int index = trfnObjectMgr::GetIndexForObject(i_TransformName);
		if (index >= 0)
		{
			// Use helper to begin and end the multiple undo operation block
			undoMultipleOperationBlock multiple_undo_block("");

			// As each node is moved, it will be checked against the 
			// the hierarchy to prevent circular links within AddNodeToTransform
			std::list<nameString> selected_names;
			get_components_from_selection(selected_names);
			std::list<nameString>::const_iterator nit;
			for (nit = selected_names.begin(); nit != selected_names.end(); ++nit)
			{
				AddNodeToTransform(i_TransformName, *nit);
			}
		}
	}

	//--------------------------------------------------------------------
	//	Remove selected objects from parent transforms (move to root)
	//--------------------------------------------------------------------
	void  RemoveSelectedFromParents()
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;
		
		// Use helper to begin and end the multiple undo operation block
		undoMultipleOperationBlock multiple_undo_block("");

		// RemoveNodeFromParent will check to see if the node has a parent
		std::list<nameString> selected_names;
		get_components_from_selection(selected_names);
		std::list<nameString>::const_iterator nit;
		for (nit = selected_names.begin(); nit != selected_names.end(); ++nit)
		{
			RemoveNodeFromParent(*nit);
		}
	}

}	// end of namespace
