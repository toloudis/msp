/****************************************************************************\
**	xfrmSurfaceView.hpp
**
**		This class constructs a hierarchy of tree nodes based on 
**	the parent transform hierarchy of the scene and the surfaces within
**	these objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef XFRM_SURFACEVIEW_HPP
#error xfrmSurfaceView.hpp multiply included
#endif
#define XFRM_SURFACEVIEW_HPP

#ifndef TWC_TREENODE_HPP
#include "ToolUIWx/twc/twcTreeNode.hpp"
#endif 

//============================================================================
//============================================================================
class xfrmTransformGroup;
class xfrmSurfaceTreeNode;
class nameString;

#include <set>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class xfrmSurfaceView 
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	xfrmSurfaceView();

	//------------------------------------------------------------------------
	// Create a tree view for the given objects that is sorted by 
	// parent hierarchy.
	//------------------------------------------------------------------------
	void CreateTreeView( const std::vector<nameString>& i_AllObjects,
						 const std::vector<nameString>& i_CheckedObjects,
						 std::vector< shared_ptr<twcTreeNode> >& o_TreeView );
	void CreateTreeView( const std::set<nameString>& i_AllObjects,
						 const std::set<nameString>& i_CheckedObjects,
						 std::vector< shared_ptr<twcTreeNode> >& o_TreeView );

private:
	//--------------------------------------------------------------------
	// Create a subtree for given object and its surfaces
	//--------------------------------------------------------------------
	shared_ptr<xfrmSurfaceTreeNode> create_subtree_for_object(const nameString& i_ObjectName,
											const std::set<nameString>& i_CheckedObjects);

	//--------------------------------------------------------------------
	// Create a subtree for given transform node and its children
	//--------------------------------------------------------------------
	shared_ptr<xfrmSurfaceTreeNode> create_subtree_for_group(xfrmTransformGroup* i_pGroupNode,
														  const std::set<nameString>& i_AllObjects,
														  const std::set<nameString>& i_CheckedObjects);
};

#endif //USE_WXWIDGETS
