/*****************************************************************************
**	api3dNodeReference.cpp
**
**	Reference that represents an attachment to a node within the scene graph
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/private/api3dNodeReference.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dNodeReference::api3dNodeReference(g3dSceneNode &i_Node)
: m_Node(i_Node)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dNodeReference::~api3dNodeReference()
{
}

//--------------------------------------------------------------------
// GetMatrix for this named refence
//--------------------------------------------------------------------
//virtual
maMatrix4x4 api3dNodeReference::GetMatrix() const
{
	// If everything was cached and used dirty bits and stuff we 
	// could do this more efficiently. But this value is often
	// not up to date:
	//return m_Node.GetTotalTransform();

	if (GetUseBoundingBox())
	{
		// Update transforms and get the world space bbox from the node
		m_Node.UpdateTotalTransform();
		const maAxisBox &world_bbox = m_Node.GetWorldBox();

		maMatrix4x4 transform;
		if (!world_bbox.IsEmpty())
			transform.MakeTranslate(world_bbox.GetCenter());
		return transform;
	}
	else
	{
		// Accumulate matrices from node to parent
		maMatrix4x4	transform = m_Node.GetTransform();
		if( !m_Node.GetIgnoreParentTransform() )
		{
			const g3dSceneNode* cur_node = m_Node.GetParent();
			while( cur_node )
			{
				transform *= cur_node->GetTransform();
				if( cur_node->GetIgnoreParentTransform() )
					break;
				cur_node = cur_node->GetParent();
			}
		}
		return transform;
	}
}

