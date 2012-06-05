/****************************************************************************\
**	cmmSceneViewCategory.hpp
**
**		This class converts the cmmDialogDataList for the placed objects
**	into a tree structure of cmmSceneTreeNodes in order to display
**	the scene manager objects grouped by category.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEVIEWCATEGORY_HPP
#error cmmSceneViewCategory.hpp multiply included
#endif
#define CMM_SCENEVIEWCATEGORY_HPP

#ifndef CMM_SCENEVIEW_HPP
#include "Systems/Common/GUI/wxGUI/cmmSceneView.hpp"
#endif 



#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class cmmSceneViewCategory : public cmmSceneView
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmSceneViewCategory();

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

};

#endif //USE_WXWIDGETS
