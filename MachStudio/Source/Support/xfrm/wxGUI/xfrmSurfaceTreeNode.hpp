/****************************************************************************\
**	xfrmSurfaceTreeNode.hpp
**
**		This class defines the derivation of twcTreeNode that is used
**	in the light set and render layer object/surface assignment tree views.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef XFRM_SURFACETREENODE_HPP
#error xfrmSurfaceTreeNode.hpp multiply included
#endif
#define XFRM_SURFACETREENODE_HPP

#ifndef TWC_TREENODE_HPP
#include "ToolUIWx/twc/twcTreeNode.hpp"
#endif 
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 


#ifdef USE_WXWIDGETS

//============================================================================
//	Forward References
//============================================================================
class sel3dObject;

//============================================================================
// xfrmSurfaceTreeNode is the base class for all tree nodes used in the scene
// manager. There are different types of nodes for objects, parts, categories
// and system nodes.
//============================================================================
class xfrmSurfaceTreeNode : public twcTreeNode
{
public:
	//------------------------------------------------------------------------
	// Different nodes types
	//------------------------------------------------------------------------
	enum NodeType
	{
		e_Object,
		e_Surface,
		e_Parent
	};

	//------------------------------------------------------------------------
	// Object constructor 
	//------------------------------------------------------------------------
	xfrmSurfaceTreeNode(const std::string & i_Name, bool i_bChecked);

	//------------------------------------------------------------------------
	// Surface constructor 
	//------------------------------------------------------------------------
	xfrmSurfaceTreeNode(const std::string & i_Name, bool i_bChecked, int i_SurfaceIndex);

	//--------------------------------------------------------------------
	// Return the string to display for this item in the tree view
	//--------------------------------------------------------------------
	virtual wxString GetDisplayString() const;

	//--------------------------------------------------------------------
	// Should this item have a checkbox?
	//--------------------------------------------------------------------
	virtual bool IsCheckable() const;

	//--------------------------------------------------------------------
	// Initial checked state of this item
	//--------------------------------------------------------------------
	virtual bool IsChecked() const;
	void SetChecked(bool i_bVisible);

	//--------------------------------------------------------------------
	// Should the children of this mode be sorted alphabetically?
	//--------------------------------------------------------------------
	virtual bool ShouldSortChildren() const;
	
	//--------------------------------------------------------------------
	// Return tree node type
	//--------------------------------------------------------------------
	NodeType GetNodeType() const {	return m_NodeType;	}
	void SetNodeType(NodeType i_NodeType) {	m_NodeType = i_NodeType;	}

	//--------------------------------------------------------------------
	// Object Name for this tree node
	//--------------------------------------------------------------------
	nameString GetObjectName() const;

	//--------------------------------------------------------------------
	// Parent of this tree node, may be NULL
	//--------------------------------------------------------------------
	shared_ptr<xfrmSurfaceTreeNode> GetParentNode() { return m_pParent; }
	void SetParentNode(shared_ptr<xfrmSurfaceTreeNode> i_pParent) { m_pParent = i_pParent; }

	//--------------------------------------------------------------------
	// Surface index within containing object
	//--------------------------------------------------------------------
	int GetSurfaceIndex() const { return m_SurfaceIndex; }

	//--------------------------------------------------------------------
	// Initial checked state of this item
	//--------------------------------------------------------------------
	//bool IsGroupNode() const;
	//void SetGroupNode(bool i_bGroup);

private:
	NodeType		m_NodeType;
	shared_ptr<xfrmSurfaceTreeNode> m_pParent;
	std::string m_Name;
	bool m_bChecked;
	int m_SurfaceIndex;
};

#endif //USE_WXWIDGETS
