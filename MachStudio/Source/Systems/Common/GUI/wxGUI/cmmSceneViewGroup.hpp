/****************************************************************************\
**	cmmSceneViewGroup.hpp
**
**		This class converts the cmmDialogDataList for the placed objects
**	into a tree structure of cmmSceneTreeNodes in order to display
**	the scene manager objects grouped by parent transforms.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEVIEWGROUP_HPP
#error cmmSceneViewGroup.hpp multiply included
#endif
#define CMM_SCENEVIEWGROUP_HPP

#ifndef CMM_SCENEVIEW_HPP
#include "Systems/Common/GUI/wxGUI/cmmSceneView.hpp"
#endif 

//============================================================================
//============================================================================
class xfrmTransformGroup;


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class cmmSceneViewGroup : public cmmSceneView
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmSceneViewGroup();

	//------------------------------------------------------------------------
	// Create a tree view for the given objects that is sorted by category.
	//------------------------------------------------------------------------
	virtual void CreateTreeView( const cmmDialogDataList &i_DataList,
						 std::vector< shared_ptr<twcTreeNode> >& o_TreeView );

	//------------------------------------------------------------------------
	// Return true if the given tree ietm in this view can be dragged
	//------------------------------------------------------------------------
	virtual bool CanDragItem( shared_ptr<twcTreeNode> i_TreeNode );

	//------------------------------------------------------------------------
	// Hande release of the mouse drag on top of the given node
	//------------------------------------------------------------------------
	virtual void DragReleased( shared_ptr<twcTreeNode> i_TreeNode );

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	typedef std::map<nameString, const cmmDialogData*> DialogDataMap;
	
	//--------------------------------------------------------------------
	// Create a subtree for given transform node and its children
	//--------------------------------------------------------------------
	shared_ptr<cmmSceneTreeNode> create_subtree_for_group(xfrmTransformGroup* i_pGroupNode,
														   const DialogDataMap &i_GroupingNodes,
												 		   const DialogDataMap &i_LeafNodes);
};

#endif //USE_WXWIDGETS
