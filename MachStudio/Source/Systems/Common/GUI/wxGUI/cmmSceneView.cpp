/****************************************************************************\
**	cmmSceneView.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneView.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"

#include "Systems/Common/GUI/cmmSceneOperations.hpp"
#include "Support/vis/visMgr.hpp"


#ifdef USE_WXWIDGETS

namespace
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
cmmSceneView::cmmSceneView()
: m_CheckStyle(e_Visible)
{

}

//------------------------------------------------------------------------
// Handle check box state change for selected tree nodes.
// Default implementation changes visibility state.
//------------------------------------------------------------------------
void cmmSceneView::CheckChanged( const std::vector<shared_ptr<twcTreeNode> >& i_TreeNodes,
							     bool i_bChecked )
{
	for (int i=0; i<i_TreeNodes.size(); ++i)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_TreeNodes[i].get());
		//DBG_LOG("Check changed: " << pNode->m_Name);
		if (pNode->GetNodeType() == cmmSceneTreeNode::e_System)
		{
			cmmSceneOperations::SetSystemVisibility(pNode->GetSystemName(), i_bChecked);
		}
		else if (pNode->GetNodeType() == cmmSceneTreeNode::e_Object)
		{
			//bga - this would be better if it used the nameString
			visMgr::SetVisibleInEditor(pNode->GetObjectName().GetString(), i_bChecked);
		}
	}
}

//--------------------------------------------------------------------
// Create a subtree for the given object and its parts
//--------------------------------------------------------------------
shared_ptr<cmmSceneTreeNode> cmmSceneView::CreateSubTreeForObject(const cmmDialogData& i_Data,
																  bool i_bShowObjectParts)
{
	//	build the string for the object from name and description
	std::string title;
	if (i_Data.m_Desc.empty()) 
		title = i_Data.m_Name.GetString();
	else
		title = i_Data.m_Name.GetString() + " - " + i_Data.m_Desc;
	//DBG_LOG3("System: %s object: %s index: %d", i_Data.m_SystemName.c_str(), title.c_str(), index);
	
	shared_ptr<cmmSceneTreeNode> object(new cmmSceneTreeNode(cmmSceneTreeNode::e_Object, title, i_Data.m_pPickObject));
	
	// Some tree views use different states in check boxes
	if (m_CheckStyle == e_Enabled)
		object->SetChecked(i_Data.m_bEnabled);
	else
		object->SetChecked(i_Data.m_bVisible);

	object->SetObjectName(i_Data.m_Name);
	object->SetSystemName(i_Data.m_SystemName);

	if (i_bShowObjectParts)
	{
		// Display categories of object parts
		int cat_index = 0;
		std::map<std::string, std::vector<cmmDialogPartData> >::const_iterator cat_it;
		for (cat_it = i_Data.m_Parts.begin(); cat_it != i_Data.m_Parts.end(); ++cat_it, ++cat_index)
		{
			// load an image icon for the part to make categories easier to see
			int image_index = cmmPlacedImages::e_FirstPartIndex + cat_index;
			if (image_index >= cmmPlacedImages::e_NumPlacedImageStates)
				image_index = -1;
			
			shared_ptr<cmmSceneTreeNode> category(new cmmSceneTreeNode(cmmSceneTreeNode::e_Category, cat_it->first, image_index));
			object->m_Children.push_back(category);

			std::vector<cmmDialogPartData>::const_iterator part_it;
			for (part_it = cat_it->second.begin(); part_it != cat_it->second.end(); ++part_it)
			{
				shared_ptr<cmmSceneTreeNode> part(new cmmSceneTreeNode(cmmSceneTreeNode::e_Part, part_it->m_Name, image_index, part_it->m_pPickObject));
				part->SetObjectName(i_Data.m_Name);
				part->SetSystemName(i_Data.m_SystemName);
				part->SetPartName(part_it->m_Name);
				part->SetCategoryName(cat_it->first);
				category->m_Children.push_back(part);
			}
		}
	}

	return object;
}

#endif // USE_WXWIDGETS
