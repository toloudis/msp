/*****************************************************************************
**	twcTreeNode.hpp
**
**	twcTreeNode class is used to describe tree data structure to the
**	twcTreeView class. Users of the tree control should create a
**	tree structure from derivations fo this class and then these nodes
**	will be given to the caller in callbacks.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_TREENODE_HPP
#error twcTreeNode.hpp multiply included
#endif
#define TWC_TREENODE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include <vector>


//------------------------------------------------------------------------
// The tree node structure should be given to the tree view control
// by constructing a structure of these TreeNode classes and then
// giving the roots to the control.
//------------------------------------------------------------------------
class twcTreeNode
{
public:
	//--------------------------------------------------------------------
	// Return the string to display for this item in the tree view
	//--------------------------------------------------------------------
	virtual wxString GetDisplayString() const = 0;

	//--------------------------------------------------------------------
	// Should this item have a checkbox?
	//--------------------------------------------------------------------
	virtual bool IsCheckable() const { return true; }

	//--------------------------------------------------------------------
	// For nodes that are not checkable, use this function to set the
	// state image index to use. A return value of "0" means "no checkbox".
	//--------------------------------------------------------------------
	virtual int GetStateIndex() const { return 0; }

	//--------------------------------------------------------------------
	// Indepenedent of the checkbox, use this function to set the
	// image index to use. A return value of "-1" means "use no image".
	//--------------------------------------------------------------------
	virtual int GetImageIndex() const { return -1; }

	//--------------------------------------------------------------------
	// Initial checked state of this item
	//--------------------------------------------------------------------
	virtual bool IsChecked() const = 0;

	//--------------------------------------------------------------------
	// Should the children of this mode be sorted alphabetically?
	//--------------------------------------------------------------------
	virtual bool ShouldSortChildren() const { return true; }

	//--------------------------------------------------------------------
	// Convenience function for adding a new child tree node
	//--------------------------------------------------------------------
	void AddChild(shared_ptr<twcTreeNode> i_Node)
		{ m_Children.push_back(i_Node); }

	std::vector< shared_ptr<twcTreeNode> > m_Children;
};


#endif // USE_WXWIDGETS
