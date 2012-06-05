/****************************************************************************\
**	xfrmSurfaceView.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/xfrm/wxGUI/xfrmSurfaceView.hpp"
#include "Support/xfrm/wxGUI/xfrmSurfaceTreeNode.hpp"

#include "Support/xfrm/xfrmTransformGroup.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#ifdef USE_WXWIDGETS

namespace
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xfrmSurfaceView::xfrmSurfaceView()
{

}

//------------------------------------------------------------------------
// Create a tree view for the given objects that is sorted by 
// parent hierarchy.
//------------------------------------------------------------------------
void xfrmSurfaceView::CreateTreeView( const std::vector<nameString>& i_AllObjects,
					 const std::vector<nameString>& i_CheckedObjects,
					 std::vector< shared_ptr<twcTreeNode> >& o_TreeView )
{
	// Start by converting vectors into sets for faster access.
	// Is this worth the conversion?
	std::set<nameString> all_objects, checked_objects;

	std::vector<nameString>::const_iterator it;
	for (it = i_AllObjects.begin(); it != i_AllObjects.end(); ++it)
		all_objects.insert(*it);
	for (it = i_CheckedObjects.begin(); it != i_CheckedObjects.end(); ++it)
		checked_objects.insert(*it);

	CreateTreeView(all_objects, checked_objects, o_TreeView);
}
void xfrmSurfaceView::CreateTreeView( const std::set<nameString>& i_AllObjects,
					 const std::set<nameString>& i_CheckedObjects,
					 std::vector< shared_ptr<twcTreeNode> >& o_TreeView )
{
	o_TreeView.clear();

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

		if (i_AllObjects.find(pGroupNode->m_pNameObject->GetName()) != i_AllObjects.end())
		{
			// Create hiearachy for this grouping node
			shared_ptr<xfrmSurfaceTreeNode> tree_node = create_subtree_for_group(pGroupNode, i_AllObjects, i_CheckedObjects);
			if (tree_node)
				o_TreeView.push_back( tree_node );
		}
	}

	// Top level objects not in a hierarchy
	const int num_root_objects = root_objects.size();
	for (int i=0; i<num_root_objects; ++i)
	{
		nameString object_name = root_objects[i];
		
		if (i_AllObjects.find(object_name) != i_AllObjects.end())
		{
			// Create tree for this object and its child parts
			shared_ptr<xfrmSurfaceTreeNode> tree_node = create_subtree_for_object(object_name, i_CheckedObjects);
			if (tree_node)
				o_TreeView.push_back( tree_node );
		}
	}

}

//--------------------------------------------------------------------
// Create a subtree for given object and its surfaces
//--------------------------------------------------------------------
shared_ptr<xfrmSurfaceTreeNode> xfrmSurfaceView::create_subtree_for_object(const nameString& i_ObjectName,
					 const std::set<nameString>& i_CheckedObjects)
{
	bool bChecked = (i_CheckedObjects.find(i_ObjectName) != i_CheckedObjects.end());
	// bga - probably should pass in nameString here, not std::string
	shared_ptr<xfrmSurfaceTreeNode> object_node(new xfrmSurfaceTreeNode(i_ObjectName.GetString(), bChecked));
	return object_node;
}

//--------------------------------------------------------------------
// Create a subtree for given transform node and its children
//--------------------------------------------------------------------
shared_ptr<xfrmSurfaceTreeNode> xfrmSurfaceView::create_subtree_for_group(xfrmTransformGroup* i_pGroupNode,
													      const std::set<nameString>& i_AllObjects,
														  const std::set<nameString>& i_CheckedObjects)
{
	shared_ptr<xfrmSurfaceTreeNode> group_node;

	if (i_pGroupNode)
	{
		// Create tree for just this transform node's data
		group_node = this->create_subtree_for_object(i_pGroupNode->m_pNameObject->GetName(), i_CheckedObjects);
		group_node->SetNodeType(xfrmSurfaceTreeNode::e_Parent);

		// Add in trees for the children of this node
		if (group_node)
		{
			int num_kids = i_pGroupNode->m_ChildNodes.size();
			for (int k=0; k<num_kids; ++k)
			{
				xfrmTransformNode *pChildNode = i_pGroupNode->m_ChildNodes[k];
				nameString child_name = pChildNode->m_pNameObject->GetName();

				if (i_AllObjects.find(child_name) != i_AllObjects.end())
				{
					// could organize the child node related functions into the base class
					// and avoid the dynamic cast...
					xfrmTransformGroup *pChildGroup = dynamic_cast<xfrmTransformGroup*>(pChildNode);
					if (pChildGroup)
					{
						shared_ptr<xfrmSurfaceTreeNode> tree_node = create_subtree_for_group(pChildGroup, i_AllObjects, i_CheckedObjects);
						if (tree_node)
							group_node->AddChild( tree_node );
					}
					else
					{
						shared_ptr<xfrmSurfaceTreeNode> leaf_node = this->create_subtree_for_object(child_name, i_CheckedObjects);
						if (leaf_node)
							group_node->AddChild( leaf_node );
					}
				}
			}
		}
	}
	return group_node;
}

#endif // USE_WXWIDGETS
