/****************************************************************************\
**	cmmSceneTreeNode.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"


#ifdef USE_WXWIDGETS


//------------------------------------------------------------------------
// System and Object constructor (nodes that have checkboxes)
//------------------------------------------------------------------------
cmmSceneTreeNode::cmmSceneTreeNode(NodeType i_NodeType, 
								   const std::string & i_DisplayString,
								   sel3dObject* i_pSceneObject)
:	m_NodeType(i_NodeType),
	m_pSceneObject(i_pSceneObject),
	m_DisplayString(i_DisplayString), 
	m_StateIndex(0),
	m_ImageIndex(-1),
	m_bVisible(true),
	m_bIsGroupNode(false)
{
}

//------------------------------------------------------------------------
// Category and ObjectPart constructor (nodes with image icons)
//------------------------------------------------------------------------
cmmSceneTreeNode::cmmSceneTreeNode(NodeType i_NodeType, 
								   const std::string & i_DisplayString, 
								   int i_StateIndex,
								   sel3dObject* i_pSceneObject)
:	m_NodeType(i_NodeType),
	m_pSceneObject(i_pSceneObject),
	m_DisplayString(i_DisplayString), 
	m_StateIndex(i_StateIndex),
	m_ImageIndex(-1),
	m_bVisible(true) ,
	m_bIsGroupNode(false)
{
}


//--------------------------------------------------------------------
// Return the string to display for this item in the tree view
//--------------------------------------------------------------------
//virtual 
wxString cmmSceneTreeNode::GetDisplayString() const
{ 
	return wxString(m_DisplayString.c_str(), wxConvUTF8); 
}

//--------------------------------------------------------------------
// Should this item have a checkbox?
//--------------------------------------------------------------------
//virtual 
bool cmmSceneTreeNode::IsCheckable() const
{ 
	return ((m_NodeType==e_System) || (m_NodeType==e_Object));
}

//--------------------------------------------------------------------
// For nodes that are not checkable, use this function to set the
// state image index to use. A return value of "0" means "no checkbox".
//--------------------------------------------------------------------
//virtual 
int cmmSceneTreeNode::GetStateIndex() const
{ 
	return m_StateIndex; 
}

//--------------------------------------------------------------------
// Independent of the checkbox, use this function to set the
// image index to use. A return value of "-1" means "use no image".
//--------------------------------------------------------------------
//virtual 
int cmmSceneTreeNode::GetImageIndex() const
{ 
	return m_ImageIndex; 
}
void cmmSceneTreeNode::SetImageIndex(int i_Index)
{ 
	m_ImageIndex = i_Index; 
}

//--------------------------------------------------------------------
// Initial checked state of this item
//--------------------------------------------------------------------
//virtual 
bool cmmSceneTreeNode::IsChecked() const
{ 
	return m_bVisible; 
}
void cmmSceneTreeNode::SetChecked(bool i_bVisible)
{
	m_bVisible = i_bVisible;
}

//--------------------------------------------------------------------
// Should the children of this mode be sorted alphabetically?
//--------------------------------------------------------------------
//virtual 
bool cmmSceneTreeNode::ShouldSortChildren() const 
{ 
	// Always sort children in scene manager
	return true; 
}

//--------------------------------------------------------------------
// System Name for this tree node
//--------------------------------------------------------------------
void cmmSceneTreeNode::SetSystemName(const std::string& i_SystemName)
{
	m_SystemName = i_SystemName;
}
const std::string& cmmSceneTreeNode::GetSystemName() const
{
	return m_SystemName;
}

//--------------------------------------------------------------------
// Object Name for this tree node
//--------------------------------------------------------------------
void cmmSceneTreeNode::SetObjectName(const nameString& i_ObjectName)
{
	m_ObjectName = i_ObjectName;
}
const nameString& cmmSceneTreeNode::GetObjectName() const
{
	return m_ObjectName;
}

//--------------------------------------------------------------------
// Category Name for this tree node
//--------------------------------------------------------------------
void cmmSceneTreeNode::SetCategoryName(const std::string& i_CategoryName)
{
	m_CategoryName = i_CategoryName;
}
const std::string& cmmSceneTreeNode::GetCategoryName() const
{
	return m_CategoryName;
}

//--------------------------------------------------------------------
// Part Name for this tree node
//--------------------------------------------------------------------
void cmmSceneTreeNode::SetPartName(const std::string& i_PartName)
{
	m_PartName = i_PartName;
}
const std::string& cmmSceneTreeNode::GetPartName() const
{
	return m_PartName;
}

//--------------------------------------------------------------------
// Initial checked state of this item
//--------------------------------------------------------------------
bool cmmSceneTreeNode::IsGroupNode() const
{
	return m_bIsGroupNode;
}
void cmmSceneTreeNode::SetGroupNode(bool i_bGroup)
{
	m_bIsGroupNode = i_bGroup;
}

#endif // USE_WXWIDGETS
