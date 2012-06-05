/*****************************************************************************
**	smdlBoneDisplay.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlBoneDisplay.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maRotation.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/smdl/private/smdlBoneFragment.hpp"


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	// Insert fragments into hierarchy, returning new nodes into passed array
	// in order to be able to set them visible/invisible later
	//----------------------------------------------------------------------------
	void insert_bones(g3dSceneNode *i_pNode, std::vector<g3dSceneNode*> &io_InsertedNodes)
	{
		// Find the joint direction from this node
		//maVector3d joint_dir(0,1,0);
		//if (const maMatrix4x4 *pOrient = i_pNode->GetOrientation())
		//{
		//	pOrient->TransformDir(joint_dir);
		//}

		bool bNodeIsJoint = i_pNode->GetIsJoint();

		// Traverse children first in order to gather some data
		// and so that we don't traverse our new nodes
		const int num_children = i_pNode->GetNumChildren();
		int num_joint_kids = 0;
		//float avg_trans = 0.0;
		maVector3d avg_trans;
		for (int i=0; i< num_children; i++)
		{
			g3dSceneNode *pChild = i_pNode->GetChild(i);

			if (bNodeIsJoint && pChild->GetIsJoint())
			{
				const maMatrix4x4 &xform = pChild->GetTransform();
				// Get out y translation of child node in order 
				// to guess at the bone length
				//avg_trans += xform.m_Mat[13];
				maVector3d trans(xform.m_Mat[12], xform.m_Mat[13], xform.m_Mat[14]);
				//avg_trans += trans * joint_dir;
				avg_trans += trans;
				num_joint_kids++;
			}

			insert_bones(pChild, io_InsertedNodes);
		}

		if (bNodeIsJoint && (num_joint_kids > 0))
		{

			// Now add a node to hold the shared bone fragment
			g3dSceneNode *pNode = new g3dSceneNode(smdlBoneFragment::GetSharedFragment());
			pNode->SetDrawStyle(g3dSceneNode::e_Wireframe);
			pNode->SetSkipAnim(true);
			pNode->SetRenderable(false); // not visible by default
			io_InsertedNodes.push_back(pNode);

			// guess at the bone length
			avg_trans /= float(num_joint_kids);
			//pNode->GetTransform().ScaleBy(1, avg_trans, 1);

			float bone_len = avg_trans.Length();
			if (bone_len > 0)
			{
				maRotation rot;
				rot.SetValue(maVector3d(0,1,0), avg_trans);

				maMatrix4x4 xform;
				xform.MakeScale( 1, bone_len, 1 );
				xform *= rot.GetMatrix();
				pNode->SetTransform(xform);
			}
			else
			{
				// with a bone_len of 0, this becomes a disc.
				maMatrix4x4 xform = pNode->GetTransform();
				xform.ScaleBy(1, bone_len, 1);
				pNode->SetTransform(xform);
			}

			i_pNode->AddChild( pNode );
		}
	}

}

//--------------------------------------------------------------------
//	smdlBoneDisplay requires the root scene node of the skeleton.
//	It will traverse the hierarchy and insert nodes to hold the
//	bone fragments which will display the joints.
//--------------------------------------------------------------------
smdlBoneDisplay::smdlBoneDisplay( g3dSceneNode *i_pNode )
: m_bVisible(false)
{
	insert_bones( i_pNode, m_InsertedNodes );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlBoneDisplay::~smdlBoneDisplay()
{
	// Inserted nodes are not owned, they will be deleted when the
	// scene graph is destroyed.
}


//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlBoneDisplay::SetVisible(bool i_bVisible)
{
	m_bVisible = i_bVisible;
	std::for_each(m_InsertedNodes.begin(), m_InsertedNodes.end(), 
		std::bind2nd(std::mem_fun(&g3dSceneNode::SetRenderable), i_bVisible));
}

//----------------------------------------------------------------------------
//virtual 
//----------------------------------------------------------------------------
bool smdlBoneDisplay::GetVisible() const
{
	return m_bVisible;
}
