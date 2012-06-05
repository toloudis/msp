/****************************************************************************\
**	cmmSceneViewCategory.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneViewCategory.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneViewCategory::cmmSceneViewCategory()
{

}

//------------------------------------------------------------------------
// Create a tree view for the given objects that is sorted by category.
//------------------------------------------------------------------------
void cmmSceneViewCategory::CreateTreeView( const cmmDialogDataList &i_DataList,
										   std::vector< shared_ptr<twcTreeNode> >& o_TreeView )
{
	o_TreeView.clear();

	std::string last_system_name;
	cmmDialogDataList::const_iterator it;
	for (it = i_DataList.begin(); it != i_DataList.end(); it++)
	{
		// The category view does not display grouping nodes
		if (it->m_GroupState != cmmDialogData::e_GroupingNode)
		{
			if (it->m_SystemName != last_system_name)
			{
				// Add new root for the system name
				shared_ptr<cmmSceneTreeNode> system(new cmmSceneTreeNode(cmmSceneTreeNode::e_System, it->m_SystemName));
				system->SetSystemName(it->m_SystemName);
				o_TreeView.push_back(system);
				last_system_name = it->m_SystemName;
			}

			// Create node for the object and its subparts
			const bool bShowObjectParts = true;
			shared_ptr<cmmSceneTreeNode> object = this->CreateSubTreeForObject(*it, bShowObjectParts);
			if (object)
				o_TreeView.back()->m_Children.push_back(object);
		}
	}
}

//------------------------------------------------------------------------
// Return true if the given tree ietm in this view can be dragged
//------------------------------------------------------------------------
bool cmmSceneViewCategory::CanDragItem( shared_ptr<twcTreeNode> i_TreeNode )
{
	return false;
}

//------------------------------------------------------------------------
// Hande release of the mouse drag on top of the given node
//------------------------------------------------------------------------
void cmmSceneViewCategory::DragReleased( shared_ptr<twcTreeNode> i_TreeNode )
{

}

#endif // USE_WXWIDGETS
