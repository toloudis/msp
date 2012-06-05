/****************************************************************************\
**	cmmSceneTreeNode.hpp
**
**		This class defines the derivation of twcTreeNode that is used
**	in the scene manager tree views.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENETREENODE_HPP
#error cmmSceneTreeNode.hpp multiply included
#endif
#define CMM_SCENETREENODE_HPP

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
// cmmSceneTreeNode is the base class for all tree nodes used in the scene
// manager. There are different types of nodes for objects, parts, categories
// and system nodes.
//============================================================================
class cmmSceneTreeNode : public twcTreeNode
{
public:
	//------------------------------------------------------------------------
	// Different nodes types
	//------------------------------------------------------------------------
	enum NodeType
	{
		e_System = 0,
		e_Object,
		e_Category,
		e_Part
	};

	//------------------------------------------------------------------------
	// System and Object constructor (nodes that have checkboxes)
	//------------------------------------------------------------------------
	cmmSceneTreeNode(NodeType i_NodeType, 
					 const std::string & i_DisplayString,
					 sel3dObject* i_pSceneObject = NULL);

	//------------------------------------------------------------------------
	// Category and ObjectPart constructor (nodes with image icons)
	//------------------------------------------------------------------------
	cmmSceneTreeNode(NodeType i_NodeType, 
					 const std::string & i_DisplayString, 
					 int i_StateIndex,
					 sel3dObject* i_pSceneObject = NULL);

	//--------------------------------------------------------------------
	// Return the string to display for this item in the tree view
	//--------------------------------------------------------------------
	virtual wxString GetDisplayString() const;

	//--------------------------------------------------------------------
	// Should this item have a checkbox?
	//--------------------------------------------------------------------
	virtual bool IsCheckable() const;

	//--------------------------------------------------------------------
	// For nodes that are not checkable, use this function to set the
	// state image index to use. A return value of "0" means "no checkbox".
	//--------------------------------------------------------------------
	virtual int GetStateIndex() const;

	//--------------------------------------------------------------------
	// Independent of the checkbox, use this function to set the
	// image index to use. A return value of "-1" means "use no image".
	//--------------------------------------------------------------------
	virtual int GetImageIndex() const;
	void SetImageIndex(int i_Index);

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

	//--------------------------------------------------------------------
	// System Name for this tree node
	//--------------------------------------------------------------------
	void SetSystemName(const std::string& i_SystemName);
	const std::string& GetSystemName() const;

	//--------------------------------------------------------------------
	// Object Name for this tree node
	//--------------------------------------------------------------------
	void SetObjectName(const nameString& i_ObjectName);
	const nameString& GetObjectName() const;

	//--------------------------------------------------------------------
	// Category Name for this tree node
	//--------------------------------------------------------------------
	void SetCategoryName(const std::string& i_CategoryName);
	const std::string& GetCategoryName() const;

	//--------------------------------------------------------------------
	// Part Name for this tree node
	//--------------------------------------------------------------------
	void SetPartName(const std::string& i_PartName);
	const std::string& GetPartName() const;

	//--------------------------------------------------------------------
	// Scene Object for this tree node
	//--------------------------------------------------------------------
	sel3dObject* GetSceneObject() { return m_pSceneObject; }

	//--------------------------------------------------------------------
	// Initial checked state of this item
	//--------------------------------------------------------------------
	bool IsGroupNode() const;
	void SetGroupNode(bool i_bGroup);

private:
	NodeType		m_NodeType;

	//bga - this could be a wxString, or we could change the GetDisplayString()
	// function so that twcTreeNode isn't really wxWidgets dependent...
	std::string		m_DisplayString;

	//bga - Note there is a lot of duplication here of the system and category names.
	// Looking towards a future where we can organize any tree view in any way, I wanted
	// each cmmSceneTreeNode to have all of the information it needs instead of relying
	// on the tree structure to get information about groupings. However, there could be a 
	// more memory efficient way to represent the system and category names through a 
	// shared const char* instead of making a std::string copy.

	std::string		m_SystemName;
	nameString		m_ObjectName;
	std::string		m_CategoryName;
	std::string		m_PartName;
	int				m_StateIndex;
	int				m_ImageIndex;
	bool			m_bVisible;
	sel3dObject*	m_pSceneObject;
	bool			m_bIsGroupNode;
};

#endif //USE_WXWIDGETS
