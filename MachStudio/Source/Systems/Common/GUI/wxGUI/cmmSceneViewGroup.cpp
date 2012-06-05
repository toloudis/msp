/****************************************************************************\
**	cmmSceneViewGroup.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneViewGroup.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"
#include "Systems/Common/Gui/cmmDialogInterestMgr.hpp"

#include "Support/xfrm/xfrmTransformGroup.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#ifdef USE_WXWIDGETS

namespace
{
	//--------------------------------------------------------------------
	// return image index based on system name
	//--------------------------------------------------------------------
  	int get_system_image_index(const std::string& i_SystemName)
	{
		if (i_SystemName == "Cameras")
			return cmmPlacedImages::e_Camera;
		else if ((i_SystemName == "Point Lights") || (i_SystemName == "Projected Lights"))
			return cmmPlacedImages::e_Light;
		else if ((i_SystemName == "Objects") || (i_SystemName == "Billboards"))
			return cmmPlacedImages::e_Geometry;
		else
			return cmmPlacedImages::e_NoImage;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneViewGroup::cmmSceneViewGroup()
{

}

//------------------------------------------------------------------------
// Create a tree view for the given objects that is sorted by category.
//------------------------------------------------------------------------
void cmmSceneViewGroup::CreateTreeView( const cmmDialogDataList &i_DataList,
										   std::vector< shared_ptr<twcTreeNode> >& o_TreeView )
{
	o_TreeView.clear();

	// Start by grabbing pointers to all of the grouping and leaf nodes out of the 
	// cmmDialogDataList, for faster access later.
	DialogDataMap grouping_nodes, leaf_nodes;
	cmmDialogDataList::const_iterator it;
	for (it = i_DataList.begin(); it != i_DataList.end(); it++)
	{
		const cmmDialogData &dialog_data = (*it);
		if (it->m_GroupState == cmmDialogData::e_GroupingNode)
			grouping_nodes[it->m_Name] = &dialog_data;
		else if (it->m_GroupState == cmmDialogData::e_LeafNode)
			leaf_nodes[it->m_Name] = &dialog_data;
	}

	// Get root nodes from transform manager (root grouping nodes and 
	// leaf nodes that have no parents in two separate lists)
	std::vector<xfrmTransformGroup*> root_nodes;
	xfrmTransformMgr::GetRootTransforms(root_nodes);
	std::vector<nameString> root_objects;
	xfrmTransformMgr::GetUngroupedObjects(root_objects);

	// Top level transforms
	const int num_root_nodes = root_nodes.size();
	for (int i=0; i<num_root_nodes; ++i)
	{
		xfrmTransformGroup* pGroupNode = root_nodes[i];

		// Create hiearachy for this grouping node
		shared_ptr<cmmSceneTreeNode> tree_node = create_subtree_for_group(pGroupNode, grouping_nodes, leaf_nodes);
		if (tree_node)
		{
			tree_node->SetImageIndex(cmmPlacedImages::e_Parent);
			o_TreeView.push_back( tree_node );
		}
	}

	// Top level objects not in a hierarchy
	const int num_root_objects = root_objects.size();
	for (int i=0; i<num_root_objects; ++i)
	{
		nameString object_name = root_objects[i];
		DialogDataMap::const_iterator data_it = leaf_nodes.find(object_name);
		if (data_it != leaf_nodes.end())
		{
			// Create tree for this object and its child parts
			shared_ptr<cmmSceneTreeNode> tree_node = CreateSubTreeForObject(*data_it->second);
			if (tree_node)
			{
				tree_node->SetImageIndex(get_system_image_index(data_it->second->m_SystemName));
				o_TreeView.push_back( tree_node );
			}
		}
	}

}

//--------------------------------------------------------------------
// Create a subtree for given transform node and its children
//--------------------------------------------------------------------
shared_ptr<cmmSceneTreeNode> cmmSceneViewGroup::create_subtree_for_group(xfrmTransformGroup* i_pGroupNode,
													   const DialogDataMap &i_GroupingNodes,
												 	   const DialogDataMap &i_LeafNodes)
{
	shared_ptr<cmmSceneTreeNode> group_node;

	if (i_pGroupNode)
	{
		// Create tree for just this transform node's data
		DialogDataMap::const_iterator data_it = i_GroupingNodes.find(i_pGroupNode->m_pNameObject->GetName());
		if (data_it != i_GroupingNodes.end())
		{
			group_node = this->CreateSubTreeForObject(*data_it->second);
			group_node->SetGroupNode(true);
		}

		// Add in trees for the children of this node
		if (group_node)
		{
			int num_kids = i_pGroupNode->m_ChildNodes.size();
			for (int k=0; k<num_kids; ++k)
			{
				xfrmTransformNode *pChildNode = i_pGroupNode->m_ChildNodes[k];

				// could organize the child node related functions into the base class
				// and avoid the dynamic cast...
				xfrmTransformGroup *pChildGroup = dynamic_cast<xfrmTransformGroup*>(pChildNode);
				if (pChildGroup)
				{
					shared_ptr<cmmSceneTreeNode> tree_node = create_subtree_for_group(pChildGroup, i_GroupingNodes, i_LeafNodes);
					if (tree_node)
					{
						tree_node->SetImageIndex(cmmPlacedImages::e_Parent);
						group_node->AddChild( tree_node );
					}
				}
				else
				{
					DialogDataMap::const_iterator child_data_it = i_LeafNodes.find(pChildNode->m_pNameObject->GetName());
					if (child_data_it != i_LeafNodes.end())
					{
						shared_ptr<cmmSceneTreeNode> leaf_node = this->CreateSubTreeForObject(*child_data_it->second);
						if (leaf_node)
						{
							leaf_node->SetImageIndex(get_system_image_index(child_data_it->second->m_SystemName));
							group_node->AddChild( leaf_node );
						}
					}
				}
			}
		}
	}
	return group_node;
}

//------------------------------------------------------------------------
// Return true if the given tree ietm in this view can be dragged
//------------------------------------------------------------------------
bool cmmSceneViewGroup::CanDragItem( shared_ptr<twcTreeNode> i_TreeNode )
{
	// Object nodes in this view can be dragged
	cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNode.get());
	if (pNode && (pNode->GetNodeType() == cmmSceneTreeNode::e_Object))
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
// Handle release of the mouse drag on top of the given node
//------------------------------------------------------------------------
void cmmSceneViewGroup::DragReleased( shared_ptr<twcTreeNode> i_TreeNode )
{
	// Only group nodes can receive the mouse drag
	cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNode.get());
	if (pNode && pNode->IsGroupNode())
	{
		cmmDialogInterestMgr::ReceiveDrag(pNode->GetObjectName(), pNode->GetSystemName());
	}
	else if (!pNode)
	{
		// Drag to root (NULL tree node) in the Group view means 
		// to send the drag event to the Parents system with a null object.
		const char* c_TransformSystemName = "Parents";
		const nameString null_name;
		cmmDialogInterestMgr::ReceiveDrag(null_name, c_TransformSystemName);
	}
}


#endif // USE_WXWIDGETS
