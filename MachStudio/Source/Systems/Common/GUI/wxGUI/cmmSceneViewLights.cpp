/****************************************************************************\
**	cmmSceneViewLights.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneViewLights.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"
#include "Systems/Common/Gui/cmmDialogInterestMgr.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"

#ifdef USE_WXWIDGETS

namespace
{
	enum eIcons
	{
		e_LightIcon = 0,
		e_LightSetIcon = 1
	};
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneViewLights::cmmSceneViewLights()
{
	// Light view uses checkboxes to signify enabled state (isolated, actually)
	// instead of visible state
	m_CheckStyle = e_Enabled;

}

//------------------------------------------------------------------------
// Create a tree view for the given objects that is sorted by category.
//------------------------------------------------------------------------
void cmmSceneViewLights::CreateTreeView( const cmmDialogDataList &i_DataList,
										   std::vector< shared_ptr<twcTreeNode> >& o_TreeView )
{
	o_TreeView.clear();

	// Start by grabbing pointers to all of the objects out of the 
	// cmmDialogDataList, for faster access later.
	DialogDataMap objects;
	cmmDialogDataList::const_iterator it;
	for (it = i_DataList.begin(); it != i_DataList.end(); it++)
	{
		const cmmDialogData &dialog_data = (*it);
		objects[it->m_Name] = &dialog_data;
	}

	// Get sets from light set manager 
	std::vector<nameString> set_names;
	ltstLightSetMgr::GetLightSetNames(set_names);

	// Light Sets
	const int num_set_names = set_names.size();
	for (int i=0; i<num_set_names; ++i)
	{
		nameString lset_name = set_names[i];

		// Create hierarchy for this light set
		shared_ptr<cmmSceneTreeNode> tree_node = create_subtree_for_set(lset_name, objects);
		if (tree_node)
		{
			tree_node->SetImageIndex(e_LightSetIcon);
			o_TreeView.push_back( tree_node );
		}
	}

	// Note: could also have a "Unassigned" node which groups the unassigned lights also

	// Unassigned Lights
	const nameString null_name;
	std::vector<nameString> unassigned;
	ltstLightSetMgr::GetLightsInSet(null_name, unassigned);
	const int num_lights = unassigned.size();
	for (int i=0; i<num_lights; ++i)
	{
		nameString light_name = unassigned[i];

		// Create hierarchy for this light	
		DialogDataMap::const_iterator data_it = objects.find(light_name);
		if (data_it != objects.end())
		{
			shared_ptr<cmmSceneTreeNode> light_node = this->CreateSubTreeForObject(*data_it->second);
			if (light_node)
			{	
				light_node->SetImageIndex(e_LightIcon);
				o_TreeView.push_back( light_node );
			}
		}
	}

}

//--------------------------------------------------------------------
// Create a subtree for given transform node and its children
//--------------------------------------------------------------------
shared_ptr<cmmSceneTreeNode> cmmSceneViewLights::create_subtree_for_set(const nameString& i_LightSetName,
													   const DialogDataMap &i_ObjectMap)
{
	shared_ptr<cmmSceneTreeNode> set_node;

	// Create tree for just this light set's data
	DialogDataMap::const_iterator data_it = i_ObjectMap.find(i_LightSetName);
	if (data_it != i_ObjectMap.end())
	{
		set_node = this->CreateSubTreeForObject(*data_it->second);
		//set_node->SetGroupNode(true);

		std::vector<nameString> light_names;
		ltstLightSetMgr::GetLightsInSet(i_LightSetName, light_names);

		int num_kids = light_names.size();
		for (int k=0; k<num_kids; ++k)
		{
			nameString light_name = light_names[k];

			DialogDataMap::const_iterator child_data_it = i_ObjectMap.find(light_name);
			if (child_data_it != i_ObjectMap.end())
			{
				shared_ptr<cmmSceneTreeNode> leaf_node = this->CreateSubTreeForObject(*child_data_it->second);
				if (leaf_node)
				{
					leaf_node->SetImageIndex(e_LightIcon);
					set_node->AddChild( leaf_node );
				}
			}
			
		}
	}

	return set_node;
}

//------------------------------------------------------------------------
// Return true if the given tree ietm in this view can be dragged
//------------------------------------------------------------------------
bool cmmSceneViewLights::CanDragItem( shared_ptr<twcTreeNode> i_TreeNode )
{
	// Light nodes in this view can be dragged
	cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNode.get());
	if (pNode && (pNode->GetNodeType() == cmmSceneTreeNode::e_Object))
	{
		if (ltstLightSetMgr::IsLight(pNode->GetObjectName()))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
// Handle release of the mouse drag on top of the given node
//------------------------------------------------------------------------
void cmmSceneViewLights::DragReleased( shared_ptr<twcTreeNode> i_TreeNode )
{
	// Only light set nodes can receive the mouse drag
	cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNode.get());
	if (pNode && (pNode->GetNodeType() == cmmSceneTreeNode::e_Object))
	{
		if (ltstLightSetMgr::IsLightSet(pNode->GetObjectName()))
			cmmDialogInterestMgr::ReceiveDrag(pNode->GetObjectName(), pNode->GetSystemName());
	}
	else if (!pNode)
	{
		// Drag to root (NULL tree node) in the Lights view means 
		// to send the drag event to the LightSets system with a null object.
		const char* c_LightSetSystemName = "LightSets";
		const nameString null_name;
		cmmDialogInterestMgr::ReceiveDrag(null_name, c_LightSetSystemName);
	}
}
//------------------------------------------------------------------------
// Handle check box state change for selected tree nodes.
// Default implementation changes visibility state.
//------------------------------------------------------------------------
void cmmSceneViewLights::CheckChanged( const std::vector<shared_ptr<twcTreeNode> >& i_TreeNodes,
							     bool i_bChecked )
{
	for (int i=0; i<i_TreeNodes.size(); ++i)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNodes[i].get());
		if (pNode->GetNodeType() == cmmSceneTreeNode::e_Object)
		{		
			if (ltstLightSetMgr::IsLightSet(pNode->GetObjectName()))
			{
				// If check changed on light set, turn on or off all lights in set
				std::vector<nameString> light_names;
				ltstLightSetMgr::GetLightsInSet(pNode->GetObjectName(), light_names);
				for (int l=0; l<light_names.size(); ++l)
				{
					ltstIsolateMgr::SetIsolated(light_names[l], i_bChecked);
				}
			}
			else
			{
				// Otherwise, isolate the single light
				ltstIsolateMgr::SetIsolated(pNode->GetObjectName(), i_bChecked);
			}
		}
	}
}


#endif // USE_WXWIDGETS
