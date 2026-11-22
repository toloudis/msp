/*****************************************************************************
**	tqcTreeNode.hpp
**
**		Node of data shown in a tqcTreeView
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_TREENODE_HPP
#error tqcTreeNode.hpp multiply included
#endif
#define TQC_TREENODE_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif

#ifdef USE_QT

#include <vector>

//============================================================================
//============================================================================
class tqcTreeNode
{
public:
	virtual ~tqcTreeNode() {}

	//--------------------------------------------------------------------
	// Text shown for the node
	//--------------------------------------------------------------------
	virtual QString GetDisplayString() const = 0;

	//--------------------------------------------------------------------
	// Checkable nodes show the checked state images; other nodes show
	// the state image given by GetStateIndex (0 for none).
	//--------------------------------------------------------------------
	virtual bool IsCheckable() const { return true; }
	virtual int GetStateIndex() const { return 0; }

	//--------------------------------------------------------------------
	// Index into the tree's image list, -1 for no image
	//--------------------------------------------------------------------
	virtual int GetImageIndex() const { return -1; }

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual bool IsChecked() const = 0;
	virtual bool ShouldSortChildren() const { return true; }

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddChild(shared_ptr<tqcTreeNode> i_Node)
		{ m_Children.push_back(i_Node); }

	std::vector< shared_ptr<tqcTreeNode> > m_Children;
};

#endif // USE_QT
