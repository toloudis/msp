/****************************************************************************\
**	xfrmSurfaceTreeNode.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/xfrm/wxGUI/xfrmSurfaceTreeNode.hpp"


#ifdef USE_WXWIDGETS

//------------------------------------------------------------------------
// Object constructor 
//------------------------------------------------------------------------
xfrmSurfaceTreeNode::xfrmSurfaceTreeNode(const std::string & i_Name, 
										 bool i_bChecked)
: m_Name(i_Name), 
  m_bChecked(i_bChecked), 
  m_NodeType(e_Object), 
  m_SurfaceIndex(-1)  
{
}

//------------------------------------------------------------------------
// Surface constructor 
//------------------------------------------------------------------------
xfrmSurfaceTreeNode::xfrmSurfaceTreeNode(const std::string & i_Name, 
										 bool i_bChecked, 
										 int i_SurfaceIndex)
: m_Name(i_Name), 
  m_bChecked(i_bChecked), 
  m_NodeType(e_Surface), 
  m_SurfaceIndex(i_SurfaceIndex) 
{

}


//--------------------------------------------------------------------
// Return the string to display for this item in the tree view
//--------------------------------------------------------------------
//virtual 
wxString xfrmSurfaceTreeNode::GetDisplayString() const
{ 
	return wxString(m_Name.c_str(), wxConvUTF8); 
}

//--------------------------------------------------------------------
// Should this item have a checkbox?
//--------------------------------------------------------------------
//virtual 
bool xfrmSurfaceTreeNode::IsCheckable() const
{ 
	return true;
}

//--------------------------------------------------------------------
// Initial checked state of this item
//--------------------------------------------------------------------
//virtual 
bool xfrmSurfaceTreeNode::IsChecked() const
{ 
	return m_bChecked; 
}
void xfrmSurfaceTreeNode::SetChecked(bool i_bChecked)
{
	m_bChecked = i_bChecked;
}

//--------------------------------------------------------------------
// Should the children of this mode be sorted alphabetically?
//--------------------------------------------------------------------
//virtual 
bool xfrmSurfaceTreeNode::ShouldSortChildren() const 
{ 
	// Always sort children in surface tree view
	return true; 
}

//--------------------------------------------------------------------
// Object Name for this tree node
//--------------------------------------------------------------------
nameString xfrmSurfaceTreeNode::GetObjectName() const
{
	nameString objectName;
	if (m_NodeType == e_Surface)
	{
		// Get the object name by getting the name of the parent tree node
		if (m_pParent)
			objectName.SetString(m_pParent->m_Name);
	}
	else
		objectName.SetString(m_Name);
	return objectName;
}

//--------------------------------------------------------------------
// Is item a group node?
//--------------------------------------------------------------------
//bool xfrmSurfaceTreeNode::IsGroupNode() const
//{
//	return m_bIsGroupNode;
//}
//void xfrmSurfaceTreeNode::SetGroupNode(bool i_bGroup)
//{
//	m_bIsGroupNode = i_bGroup;
//}

#endif // USE_WXWIDGETS
