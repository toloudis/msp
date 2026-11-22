/****************************************************************************\
**	cmmSceneView.hpp
**
**		This class converts the cmmDialogDataList for the placed objects
**	into a tree structure of cmmSceneTreeNodes in order to display
**	the scene manager object grouped by category.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEVIEW_HPP
#error cmmSceneView.hpp multiply included
#endif
#define CMM_SCENEVIEW_HPP

#ifndef TQC_TREENODE_HPP
#include "ToolUIQt/tqc/tqcTreeNode.hpp"
#endif 
#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/Gui/cmmDialogInterest.hpp"
#endif 


#ifdef USE_QT

//============================================================================
//	Forward References
//============================================================================
class tqcTreeNode;
class cmmSceneTreeNode;

//============================================================================
//============================================================================
class cmmSceneView 
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmSceneView();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~cmmSceneView() {}

	//------------------------------------------------------------------------
	// Create a tree view for the given objects that is sorted by category.
	//------------------------------------------------------------------------
	virtual void CreateTreeView( const cmmDialogDataList &i_DataList,
						 std::vector< shared_ptr<tqcTreeNode> >& o_TreeView ) = 0;

	//------------------------------------------------------------------------
	// Return true if the given tree ietm in this view can be dragged
	//------------------------------------------------------------------------
	virtual bool CanDragItem( shared_ptr<tqcTreeNode> i_TreeNode ) = 0;

	//------------------------------------------------------------------------
	// Hande release of the mouse drag on top of the given node
	//------------------------------------------------------------------------
	virtual void DragReleased( shared_ptr<tqcTreeNode> i_TreeNode ) = 0;

	//------------------------------------------------------------------------
	// Handle check box state change for selected tree nodes.
	// Default implementation changes visibility state.
	//------------------------------------------------------------------------
	virtual void CheckChanged( const std::vector<shared_ptr<tqcTreeNode> >& i_TreeNodes,
							   bool i_bChecked );

protected:
	//--------------------------------------------------------------------
	// Create a subtree for the given object and its parts
	//--------------------------------------------------------------------
	shared_ptr<cmmSceneTreeNode> CreateSubTreeForObject(const cmmDialogData& i_Data,
														bool i_bShowObjectParts = false);

	enum CheckStyle
	{
		e_Visible,
		e_Enabled
	};
	CheckStyle m_CheckStyle;
};

#endif //USE_QT
