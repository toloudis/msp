/****************************************************************************\
**	cmmSceneViewLights.hpp
**
**		This class converts the cmmDialogDataList for the placed objects
**	into a tree structure of cmmSceneTreeNodes in order to display
**	the scene manager lights grouped by light set.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEVIEWLIGHTS_HPP
#error cmmSceneViewLights.hpp multiply included
#endif
#define CMM_SCENEVIEWLIGHTS_HPP

#ifndef CMM_SCENEVIEW_HPP
#include "Systems/Common/GUI/wxGUI/cmmSceneView.hpp"
#endif 

//============================================================================
//============================================================================
class nameString;


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class cmmSceneViewLights : public cmmSceneView
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmSceneViewLights();

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

	//------------------------------------------------------------------------
	// Handle check box state change for selected tree nodes.
	// Lighting implementation changes isolated state.
	//------------------------------------------------------------------------
	virtual void CheckChanged( const std::vector<shared_ptr<twcTreeNode> >& i_TreeNodes,
							   bool i_bChecked );

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	typedef std::map<nameString, const cmmDialogData*> DialogDataMap;
	
	//--------------------------------------------------------------------
	// Create a subtree for given transform node and its children
	//--------------------------------------------------------------------
	shared_ptr<cmmSceneTreeNode> create_subtree_for_set(const nameString& i_LightSetName,
														   const DialogDataMap &i_ObjectMap);
};

#endif //USE_WXWIDGETS
