/*****************************************************************************
**  SceneIterator.cpp
**
**   Class for traversing transformation hierarchies for exporting, only
**	traverses transforms with surfaces as children.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <SceneIterator.hpp>

#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>

#include <vector>

//========================================================================
//========================================================================
SceneIterator::SceneIterator()
{

}

//========================================================================
//========================================================================
//virtual 
SceneIterator::~SceneIterator()
{

}

//========================================================================
// Traverses hierarchy rooted at the given transform node.
//========================================================================
//virtual 
void SceneIterator::Traverse(MFnTransform &i_Transform)
{
	this->Visit(i_Transform.object());
}

//========================================================================
// Virtual function for visiting a node in the hierarchy.
// Override this in the derived classes in order to process hierarchy.
// Call the base class implementation to traverse the children.
//========================================================================
//virtual 
void SceneIterator::Visit(MObject &i_Object)
{
	MStatus status;
	MFnTransform transform(i_Object, &status);
	if (status)
	{
		const int numChildren = transform.childCount();

		std::vector<int> visible;
		for (int i = 0; i < numChildren; i++) 
		{
			if (this->ShouldVisit(transform.child(i), i_Object)) 
				visible.push_back(i);
		}

		const int numVisible = visible.size();
		if (numVisible > 0) 
		{
			for (int i = 0; i < numVisible; i++) 
			{
				MObject obj = transform.child(visible[i]);
				this->Visit(obj);
			}
		}
	}
}


////========================================================================
//// Virtual function for deciding whether to visit a node.
////========================================================================
////virtual 
//bool  MeshIterator::ShouldVisit(MObject &i_Object)
//{
//	// Write transforms with mesh as children
//	return (MayaUtil::HasTypeAsChild(i_Object, MFn::kMesh));
//}
//
////========================================================================
//// Virtual function for deciding whether to visit a node.
////========================================================================
////virtual 
//bool  JointIterator::ShouldVisit(MObject &i_Object)
//{
//	// All joints should write out, but only transforms with mesh in
//	// their subgraph should write out 
//	return ( i_Object.hasFn(MFn::kJoint) ||
//			 MayaUtil::HasTypeAsChild(i_Object, MFn::kMesh) );
//}
