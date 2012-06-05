/*****************************************************************************
**  mcpDirectController.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "mcpDirectController.hpp"

#include "mcpBoneFragment.hpp"

#include "dbgLog.hpp"
#include "g3dSceneNode.hpp"
#include "mcpHTRData.hpp"

namespace
{
	// Set values from rotation and translation into matrix in node
	void set_xform_values(g3dSceneNode* io_pSceneNode,
						const maRotation &i_Rotate,
						const maVector3d &i_Translate)
	{
		maMatrix4x4& transform = io_pSceneNode->GetTransform();
		transform = i_Rotate.GetMatrix();

		//const maMatrix4x4* pOrientation = io_pSceneNode->GetOrientation();
		//// Check for HJoint, need to use orientation here
		//if (pOrientation)
		//{
		//	transform *= *pOrientation;
		//}

		transform.TranslateBy(i_Translate.m_X, i_Translate.m_Y, i_Translate.m_Z);
	}

	// Recursive connection of segments to the tree rooted at this node
	void connect_subtree(g3dSceneNode *i_pParentNode,
					  int i_ParentIndex,
					  const std::vector<mcpHTRSegmentData> &i_Segments,
					  std::vector<g3dSceneNode*> &io_Connections)
	{
		int kid_index = 0;
		for (int i=0; i<i_Segments.size(); ++i)
		{
			const mcpHTRSegmentData &segment = i_Segments[i];
			if (segment.m_ParentIndex == i_ParentIndex)
			{
				// kid_index tracks which index to use for the next 
				// child of this parent. We get the current index and then increment
				// for the next use.
				int child = kid_index++;
				if (child >= 0 && child < i_pParentNode->GetNumChildren())
				{
					DBG_ASSERT1(io_Connections[i] == NULL, "Cycle in the tree at node %s?", segment.m_Name.c_str());

					//DBG_LOG4("Conecting %d, Parent index %d child %d out of %d", i, i_ParentIndex, child, i_pParentNode->GetNumChildren());
					io_Connections[i] = i_pParentNode->GetChild(child);
					//DBG_LOG1("  this node has %d children", m_Connections[i]->GetNumChildren());
					
					// do recursion from this node
					connect_subtree(io_Connections[i], i, i_Segments, io_Connections);
				}
				else
				{
					DBG_WARNING3("parent index %d, child index %d out of range %d", i_ParentIndex, child, i_pParentNode->GetNumChildren());
				}
			}
		}
	}
}


//--------------------------------------------------------------------
//	There is a hierarchy defined by the HTR SegmentData. This
//	constructor tries to match up the hierarchy with the 
//	scene graph hierarchy rooted at i_pRootNode.
//--------------------------------------------------------------------
mcpDirectController::mcpDirectController(const std::vector<mcpHTRSegmentData> &i_Segments,
					g3dSceneNode *i_pRootNode)
{
	const int num_segs = i_Segments.size();
	m_Connections.resize(num_segs, NULL);

	// Since we can't be guaranteed that the segments will come in order with
	// parents first and then children, we need to do our own searching
	// order based on finding the parent indexes

	// Root nodes will have parent index of -1
	for (int i=0; i<num_segs; ++i)
	{
		const mcpHTRSegmentData &segment = i_Segments[i];
		if (segment.m_ParentIndex == -1)
		{
			m_Connections[i] = i_pRootNode;

			// Use this node and index to connect the rest of the tree
			connect_subtree(i_pRootNode, i, i_Segments, m_Connections);
		}
	}

	// Now insert new nodes to display skeleton
	/*for (int i=0; i<num_segs; ++i)
	{
		if (m_Connections[i] != NULL)
		{
			const mcpHTRSegmentData &segment = i_Segments[i];

			g3dSceneNode *pNode = new g3dSceneNode(mcpBoneFragment::GetSharedFragment());
			pNode->SetDrawStyle(g3dSceneNode::e_Wireframe);
			pNode->GetTransform().ScaleBy(1, segment.m_BoneLength, 1);

			m_Connections[ i ]->AddChild( pNode );
		}
	}*/
}

//--------------------------------------------------------------------
// Set transforms of scene nodes to match the transforms in the
//	segment data.
//--------------------------------------------------------------------
bool mcpDirectController::Update(const std::vector<mcpHTRSegmentData> &i_Segments)
{
	const int num_nodes = i_Segments.size();
	if (num_nodes != m_Connections.size()) 
		return false;

	// First build hierarchy without fragments
	for (int i=0; i<num_nodes; ++i)
	{
		const mcpHTRSegmentData &segment = i_Segments[i];
		g3dSceneNode *pNode = m_Connections[i];
		if (pNode)
		{
			set_xform_values(pNode, segment.m_Rotation, segment.m_Position);
		}
	}
	return true;
}

//--------------------------------------------------------------------
// Check to make see how many segments have an connected node
//--------------------------------------------------------------------
int mcpDirectController::GetNumGoodConnections() const
{
	int count = 0;
	for (int i=0; i<m_Connections.size(); ++i)
	{
		if (m_Connections[i] != NULL)
			count++;
	}
	return count;
}

