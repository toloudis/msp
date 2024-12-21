/*****************************************************************************
**	smdlMeshStaticGroup.hpp
**
**		smdlMeshStaticGroup manages the visibility of a group of fragments
**	within the scene graph hierarchy.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlMeshStaticGroup.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maRotation.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//	Find nodes with fragments within this hierarchy.
	//	Returns true if there are some LowRes nodes.
	//--------------------------------------------------------------------
	void gather_nodes( g3dSceneNode *i_pNode, 
					   std::vector<g3dSceneNode*> &io_ParentNodes,
					   bool &o_bHaveLowRes,
					   bool &o_bHaveHighRes)
	{
		o_bHaveLowRes = false;
		o_bHaveHighRes = false;

		if (i_pNode->GetFragment() != NULL)
		{
			if (i_pNode->GetContentResolution() == g3dSceneNode::e_LowRes)
				o_bHaveLowRes = true;
			else if (i_pNode->GetContentResolution() == g3dSceneNode::e_HighRes)
				o_bHaveHighRes = true;

			io_ParentNodes.push_back(i_pNode);
		}

		const int num_children = i_pNode->GetNumChildren();
		for (int i=0; i<num_children; ++i)
		{
			bool bLowRes = false, bHighRes = false;
			gather_nodes(i_pNode->GetChild(i), io_ParentNodes, bLowRes, bHighRes);
			o_bHaveLowRes |= bLowRes;
			o_bHaveHighRes |= bHighRes;
		}
	}
	
	//--------------------------------------------------------------------
	// If we have some low-res fragments, then make sure that
	// the other fragments are marked as "high-res" instead of "mixed"
	//--------------------------------------------------------------------
	void mark_high_res(std::vector<g3dSceneNode*> &io_ParentNodes)
	{
		for (int i=0; i<io_ParentNodes.size(); i++)
		{
			if (io_ParentNodes[i]->GetContentResolution() == g3dSceneNode::e_Mixed)
				io_ParentNodes[i]->SetContentResolution( g3dSceneNode::e_HighRes );
		}
	}
}

//--------------------------------------------------------------------
//	smdlMeshStaticGroup requires the root scene node of the skeleton.
//	It will traverse the hierarchy and grab pointers to the nodes
//	that contain static fragments.
//--------------------------------------------------------------------
smdlMeshStaticGroup::smdlMeshStaticGroup( g3dSceneNode *i_pNode )
: m_bVisible(true), m_bHasLowRes(false)
{
	bool bHasHighRes = false;
	gather_nodes( i_pNode, m_ParentNodes, m_bHasLowRes, bHasHighRes );

	if (m_bHasLowRes && !bHasHighRes)
	{
		// If we have some low-res fragments, but no high-res fragments,
		// then make sure that the other fragments are marked as "high-res" 
		// instead of "mixed"
		mark_high_res( m_ParentNodes );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMeshStaticGroup::~smdlMeshStaticGroup()
{
}

//--------------------------------------------------------------------
// Adds in node to list of managed nodes of parents of static meshes
//--------------------------------------------------------------------
void smdlMeshStaticGroup::AddNode(g3dSceneNode* i_pNode)
{
	// Adjust resolution settings
	if (m_bHasLowRes)
	{
		// If we had some low resolution nodes and this is marked
		// mixed, then we need to mark this a high-res.
		if (i_pNode->GetContentResolution() == g3dSceneNode::e_Mixed)
			i_pNode->SetContentResolution( g3dSceneNode::e_HighRes );
	}
	else if (i_pNode->GetContentResolution() == g3dSceneNode::e_LowRes)
	{
		// If this is our first low resolution fragment, then
		// mark all of our old nodes that are mixed to now be high-res.
		mark_high_res( m_ParentNodes );
	}

	m_ParentNodes.push_back(i_pNode);
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlMeshStaticGroup::SetVisible(bool i_bVisible)
{
	m_bVisible = i_bVisible;
	std::for_each(m_ParentNodes.begin(), m_ParentNodes.end(), 
		std::bind(std::mem_fn(&g3dSceneNode::SetRenderable), std::placeholders::_1, i_bVisible));
}
//virtual 
bool smdlMeshStaticGroup::GetVisible() const
{
	return m_bVisible;
}

//--------------------------------------------------------------------
//	Returns true if some of the fragments in this group are
//	marked as the low-resolution model.
//--------------------------------------------------------------------
bool smdlMeshStaticGroup::HasLowResolution() const
{
	return m_bHasLowRes;
}
