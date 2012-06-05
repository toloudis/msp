/*****************************************************************************
**	rlyrRenderLayerObject.hpp
**
**	An object that is eligible to exist in a render layer.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef RLYR_RENDERLAYEROBJECT_HPP
#error rlyrRenderLayerObject.hpp multiply included
#endif
#define RLYR_RENDERLAYEROBJECT_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>
#include <vector>

//============================================================================
//	Forward References
//============================================================================
class api3dObjectSingle;
class nameObject;
class rlyrRenderLayer;
class rlyrRenderLayerNode;

//----------------------------------------------------------------------------
// A named 3d object that can be put in a render layer
//----------------------------------------------------------------------------
class rlyrObject
{
public:
	nameObject* m_pNameObj;
	api3dObjectSingle* m_pObject;

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~rlyrObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//nameString GetName();
	const nameString& GetName() const;

	//----------------------------------------------------------------------------
	// Get number of fragment nodes
	//----------------------------------------------------------------------------
	int GetNumNodes() const;

	//----------------------------------------------------------------------------
	// Get name of fragment nodes with given index
	//----------------------------------------------------------------------------
	const std::string& GetNodeName(int i_Index) const;

	//----------------------------------------------------------------------------
	// Get fragment node with given index
	//----------------------------------------------------------------------------
	rlyrRenderLayerNode* Node(int i_Index);

	//----------------------------------------------------------------------------
	// One special node represents the root of the object
	//----------------------------------------------------------------------------
	rlyrRenderLayerNode* RootNode();

private:
	rlyrRenderLayerNode* m_pRootNode;
	std::vector<rlyrRenderLayerNode*> m_Nodes;
};


