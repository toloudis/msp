/*****************************************************************************
**	rlyrRenderLayerObject.cpp
**
**	An object that is eligible to live in an environment.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/private/rlyrRenderLayerObject.hpp"

#include "Support/rlyr/private/rlyrRenderLayerNode.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace
{

	//====================================================================
	// gather nodes with fragments from scene node tree
	//====================================================================
	void gather_fragments(g3dSceneNode *i_Node, 
						  std::vector<rlyrRenderLayerNode*> &o_Nodes,
						  const std::string& i_LastStringName)
	{
		// Don't gather the low resolution fragments
		if (i_Node->GetContentResolution() == g3dSceneNode::e_LowRes)
			return;

		// Skip nodes with wireframe draw style because these flags
		// aren't relevant for non-solid draw styles, and because the 
		// joint display fragments shouldn't be displayed
		if ((i_Node->GetDrawStyle() != g3dSceneNode::e_Inherit) &&
			(i_Node->GetDrawStyle() != g3dSceneNode::e_Solid))
			return;

		// Track the last node name we encounter as we traverse down the tree
		// in order to share the node names when there are multiple materials
		std::string node_name = i_LastStringName;
		if (i_Node->GetName())
			node_name = i_Node->GetName();

		if (i_Node->GetFragment())
		{
			// Construct full name from fragment and material names
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss << "_Frag" << o_Nodes.size();
			std::string frag_name = node_name + oss.str();
			if (frag_name.empty())
			{
				//char buff[128];
				//::sprintf(buff, "Fragment #%d", o_Nodes.size());
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss<<"_Frag"<<o_Nodes.size();
				std::string buff(oss.str());
								
				frag_name = buff.c_str();
			}

			std::string mat_name;
			if (matMaterial *pMat = i_Node->GetFragment()->GetMaterial())
			{
				mat_name = std::string(" : ") + pMat->GetName();
			}

			//DBG_LOG2("Have fragment: %s%s", frag_name.c_str(), mat_name.c_str());

			// Gather fragment's node
			rlyrRenderLayerNode *pSetNode = new rlyrRenderLayerNode(i_Node, frag_name + mat_name);
			o_Nodes.push_back(pSetNode);
		}

		int num_kids = i_Node->GetNumChildren();
		for (int k=0; k<num_kids; k++)
		{
			gather_fragments(i_Node->GetChild(k), o_Nodes, node_name);
		}
	}

}  // end anonymous namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrObject::rlyrObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject)
:	m_pNameObj(i_pNameObj), 
	m_pObject(i_pObject),
	m_pRootNode(NULL)
{
	m_pRootNode = new rlyrRenderLayerNode(i_pObject->Object()->GetBase(), "Root");
	gather_fragments(i_pObject->Object()->GetBase(), m_Nodes, "");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rlyrObject::~rlyrObject()
{	
	delete m_pRootNode;
	envSTLHelpers::DeleteContainer(m_Nodes);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//nameString rlyrObject::GetName()
//{
//	return m_pNameObj->GetName();
//}
const nameString& rlyrObject::GetName() const
{
	return m_pNameObj->GetName();
}

//----------------------------------------------------------------------------
// Get number of fragment nodes that can be separately lit
//----------------------------------------------------------------------------
int rlyrObject::GetNumNodes() const
{
	return m_Nodes.size();
}

//----------------------------------------------------------------------------
// Get name of fragment nodes with given index
//----------------------------------------------------------------------------
const std::string& rlyrObject::GetNodeName(int i_Index) const
{
	DBG_ASSERT(i_Index < m_Nodes.size(), "Node index out of range: " << i_Index << " of " << m_Nodes.size());
	return m_Nodes[i_Index]->m_NodeName;
}

//----------------------------------------------------------------------------
// Get fragment node with given index
//----------------------------------------------------------------------------
rlyrRenderLayerNode* rlyrObject::Node(int i_Index)
{
	DBG_ASSERT(i_Index < m_Nodes.size(), "Node index out of range: " << i_Index << " of " << m_Nodes.size());
	return m_Nodes[i_Index];
}

//----------------------------------------------------------------------------
// One special node represents the root of the object
//----------------------------------------------------------------------------
rlyrRenderLayerNode* rlyrObject::RootNode()
{
	return m_pRootNode;
}