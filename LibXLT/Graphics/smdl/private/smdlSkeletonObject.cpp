/*****************************************************************************
**	smdlSkeletonObject.hpp
**
**		smdlSkeletonObject is abstract base class for objects that contain
**	a joint skeleton.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlSkeletonObject.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//  count_joints - count number of joints that
	//		are related to animation. This is used to resize
	//		the array of matrices used in skinning.
	//--------------------------------------------------------------------
	int count_joints(const g3dSceneNode* i_pNode)
	{
		int nJoints = 0;
		if (i_pNode->GetIsJoint() && !i_pNode->GetSkipAnim())
		{
			//DBG_LOG("Got joint named: " << i_pNode->GetName());
			nJoints = 1;
		}

		int nChildren = i_pNode->GetNumChildren();
		for( int i = 0 ; i < nChildren ; ++i )
		{
			nJoints += count_joints( i_pNode->GetChild(i) );
		}

		return nJoints;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dSceneNode* get_joint_by_index(g3dSceneNode* i_pNode, 
								  int i_Index, 
								  int &io_Count)
	{
		if (i_pNode->GetIsJoint() && !i_pNode->GetSkipAnim())
		{
			if (i_Index == io_Count)
				return i_pNode;
			else
				io_Count++;
		}

		int nChildren = i_pNode->GetNumChildren();
		for( int i = 0 ; i < nChildren ; ++i )
		{
			g3dSceneNode *pFound = get_joint_by_index( i_pNode->GetChild(i), i_Index, io_Count );
			if (pFound)
				return pFound;
		}

		return NULL;
	}

	//--------------------------------------------------------------------
	//  build_matrices - build array of skinning matrices.
	//		The pointer o_pMatrix should point to an array of matrices
	//		that is long enough based on the result of CountJoints.
	//--------------------------------------------------------------------
	void build_matrices( const g3dSceneNode* i_pNode, 
						  const maMatrix4x4& i_ParentTransform, 
						  maMatrix4x4*& o_pMatrix )
	{
		maMatrix4x4 cur_matrix = i_pNode->GetTransform() * i_ParentTransform;
		if (i_pNode->GetIsJoint() && !i_pNode->GetSkipInfluence())
			*o_pMatrix++ = i_pNode->GetInvBindPose() * cur_matrix;

		int nChildren = i_pNode->GetNumChildren();
		for( int i = 0 ; i < nChildren ; ++i )
		{
			build_matrices( i_pNode->GetChild(i), cur_matrix, o_pMatrix );
		}
	}

	//--------------------------------------------------------------------
	// find joint that uses this node
	//--------------------------------------------------------------------
	//g3dSceneNode* get_joint_for_node(g3dSceneNode* i_pNode, g3dSceneNode* i_pNode)
	//{
	//	if (i_pNode == i_pNode)
	//		return i_pNode;

	//	int nChildren = i_pNode->GetNumChildren();
	//	for( int i = 0 ; i < nChildren ; ++i )
	//	{
	//		g3dSceneNode* joint = get_joint_for_node(i_pNode->GetChild(i), i_pNode);
	//		if (joint) return joint;
	//	}
	//	return NULL;
	//}
}

//--------------------------------------------------------------------
//	smdlSkeletonObject requires the scene graph hierarchy.
//  It will look for joints within the hierarchy.
//--------------------------------------------------------------------
smdlSkeletonObject::smdlSkeletonObject(	g3dSceneNode* i_pRootNode )
:	smdlHierarchyObject( i_pRootNode )
{
//	this->SetLocalScaling(true);

	m_nNumJoints = count_joints( i_pRootNode );
	m_BoneMatrices.resize(m_nNumJoints);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSkeletonObject::~smdlSkeletonObject()
{
}

//--------------------------------------------------------------------
// GetJointByIndex - return the joint with the given index.
//	Note that this is a slow search implementation, don't call
//	this often.
//--------------------------------------------------------------------
g3dSceneNode* smdlSkeletonObject::GetJointByIndex( int i_BoneIndex )
{
	int count = 0;
	return get_joint_by_index(GetHierarchyRoot(), i_BoneIndex, count);
}

//--------------------------------------------------------------------
//	Inserts node into hierarchy to allow control animation
//	to alter matrix
//--------------------------------------------------------------------
//g3dSceneNode* smdlSkeletonObject::InsertControlNode(const char* i_Name)
//{
//	g3dSceneNode *pAttachment = m_pRootJoint->GetNamedJoint(i_Name);
//	DBG_ASSERT(pAttachment, "Can't find node to attach");
//
//	// Create new node and joint to hold it
//	g3dSceneNode *node = new g3dSceneNode();
//	node->SetSkipAnim(true);
//	g3dSceneNode *joint = new g3dSceneNode(pAttachment->GetInvBindPose(), node);
//	joint->SetSkipAnim(pAttachment->GetSkipAnim());
//	pAttachment->SetSkipAnim(true);	// trick it
//
//	// Insert new node between attachment node and its children
//	const int num_nkids = pAttachment->GetSceneNode()->GetNumChildren();
//	for (int ni=0; ni<num_nkids; ni++)
//	{
//		g3dSceneNode *child = pAttachment->GetSceneNode()->GetChild(0);
//		pAttachment->GetSceneNode()->RemoveChild(child);
//		node->AddChild(child);
//	}
//	const int num_jkids = pAttachment->GetNumChildren();
//	for (int ji=0; ji<num_jkids; ji++)
//	{
//		g3dSceneNode *child_joint = pAttachment->GetChild(0);
//		pAttachment->RemoveChild(child_joint);
//		joint->AddChild(child_joint);
//	}
//	pAttachment->GetSceneNode()->AddChild(node);
//	pAttachment->AddChild(joint);
//
//	return node;
//}

//--------------------------------------------------------------------
// Remove Control Node that was added earlier
//--------------------------------------------------------------------
//void smdlSkeletonObject::RemoveControlNode(g3dSceneNode* i_pNode)
//{
//	g3dSceneNode *pJoint = get_joint_for_node(m_pRootJoint, i_pNode);
//	DBG_ASSERT(pJoint, "Need joint to detach");
//
//	g3dSceneNode *parent = pJoint->GetParent();
//	DBG_ASSERT(parent, "Need parent node to detach");
//
//	// Remove control node between parent node and its children
//	parent->GetSceneNode()->RemoveChild(i_pNode);
//	parent->RemoveChild(pJoint);
//	parent->SetSkipAnim(pJoint->GetSkipAnim());
//	const int num_nkids = i_pNode->GetNumChildren();
//	for (int ni=0; ni<num_nkids; ni++)
//	{
//		g3dSceneNode *child = i_pNode->GetChild(0);
//		i_pNode->RemoveChild(child);
//		parent->GetSceneNode()->AddChild(child);
//	}
//	const int num_jkids = pJoint->GetNumChildren();
//	for (int ji=0; ji<num_jkids; ji++)
//	{
//		g3dSceneNode *child_joint = pJoint->GetChild(0);
//		pJoint->RemoveChild(child_joint);
//		parent->AddChild(child_joint);
//	}
//
//	// Now delete extra joint we added
//	delete pJoint;
//}

//--------------------------------------------------------------------
//	BuildMatrices - gather matrices for skinning into local
//	array. Returns pointer into matrix array.
//--------------------------------------------------------------------
maMatrix4x4* smdlSkeletonObject::BuildMatrices()
{
	if (m_BoneMatrices.empty())
		return NULL;

	// Build the matrices
	maMatrix4x4 transform;
	maMatrix4x4* bone_matrices = &m_BoneMatrices[0];
	build_matrices( GetHierarchyRoot(), transform, bone_matrices );

	// The variable bone_matrices has changed, so return
	// a new pointer to the beginning of the array
	return &m_BoneMatrices[0];
}
