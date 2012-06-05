/*****************************************************************************
**	smdlSkeletonObject.hpp
**
**		smdlSkeletonObject is abstract base class for objects that contain
**	a joint skeleton.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SKELETONOBJECT_HPP
#error smdlSkeletonObject.hpp multiply included
#endif
#define SMDL_SKELETONOBJECT_HPP

#ifndef SMDL_HIERARCHYOBJECT_HPP
#include "Graphics/smdl/smdlHierarchyObject.hpp"
#endif


//============================================================================
//============================================================================
//class smdlJoint;


//============================================================================
//============================================================================
class smdlSkeletonObject : public smdlHierarchyObject
{
	protected:
		//--------------------------------------------------------------------
		//	smdlSkeletonObject requires the scene graph hierarchy.
		//  It will look for joints within the hierarchy.
		//--------------------------------------------------------------------
		smdlSkeletonObject(	g3dSceneNode* i_pRootNode );

	public:
		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~smdlSkeletonObject();

		//--------------------------------------------------------------------
		//  GetJointTree
		//--------------------------------------------------------------------
		//inline smdlJoint* GetJointTree();
		//inline const smdlJoint* GetJointTree() const;

		//--------------------------------------------------------------------
		// GetNumJoints - return number of animating joints in joint tree.
		//	Does not count ínserted control nodes.
		//--------------------------------------------------------------------
		inline int GetNumJoints() const;

		//--------------------------------------------------------------------
		// GetJointByIndex - return the joint with the given index.
		//	Note that this is a slow search implementation, don't call
		//	this often.
		//--------------------------------------------------------------------
		g3dSceneNode* GetJointByIndex( int i_BoneIndex );

		//--------------------------------------------------------------------
		//	Inserts node into hierarchy to allow control animation
		//	to alter matrix
		//--------------------------------------------------------------------
//		virtual g3dSceneNode* InsertControlNode(const char* i_Name);

		//--------------------------------------------------------------------
		// Remove Control Node that was added earlier. Caller takes
		//	ownership and needs to delete the node.
		//--------------------------------------------------------------------
//		virtual void RemoveControlNode(g3dSceneNode* i_pNode);

	protected:
		//--------------------------------------------------------------------
		//	BuildMatrices - gather matrices for skinning into local
		//	array. Returns pointer into matrix array.
		//--------------------------------------------------------------------
		maMatrix4x4* BuildMatrices();

	private:
//		smdlJoint* m_pRootJoint;
		int m_nNumJoints;
		std::vector<maMatrix4x4> m_BoneMatrices;
};


//--------------------------------------------------------------------
//  GetJointTree
//--------------------------------------------------------------------
//inline smdlJoint* smdlSkeletonObject::GetJointTree()
//{
//	return m_pRootJoint;
//}
//
//inline const smdlJoint* smdlSkeletonObject::GetJointTree() const
//{
//	return m_pRootJoint;
//}


//--------------------------------------------------------------------
// GetNumJoints - return number of animating joints in joint tree.
//--------------------------------------------------------------------
inline int smdlSkeletonObject::GetNumJoints() const
{
	return m_nNumJoints;
}