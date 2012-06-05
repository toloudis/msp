/*****************************************************************************
**  SceneIterator.hpp
**
**   Class for traversing transformation hierarchies for exporting, only
**	traverses transforms with surfaces as children.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef SCENE_ITERATOR_HPP
#error SceneIterator.hpp multiply included
#endif
#define SCENE_ITERATOR_HPP


class MDagPath;
class MFnDependencyNode;
class MObject;
class MFnTransform;

//============================================================================
//============================================================================
class SceneIterator
{
public:
	//========================================================================
	//========================================================================
	SceneIterator();
	
	//========================================================================
	//========================================================================
	virtual ~SceneIterator();

	//========================================================================
	// Traverses hierarchy rooted at the given transform node.
	//========================================================================
	virtual void Traverse(MFnTransform &i_Transform);

protected:
	//========================================================================
	// Virtual function for visiting a node in the hierarchy.
	// Override this in the derived classes in order to process hierarchy.
	// Call the base class implementation to traverse the children.
	//========================================================================
	virtual void Visit(MObject &i_Object);

	//========================================================================
	// Virtual function for deciding whether to visit a node. 
	// Pass in MObject for the node to consider and also the
	// parent node that was already assumed to be visited.
	// Return true if this node should be visited.
	//========================================================================
	virtual bool ShouldVisit(MObject &i_Object, MObject &i_ParentObject) = 0;

};


//============================================================================
//============================================================================
//class MeshIterator : public SceneIterator
//{
//protected:
//	//========================================================================
//	// Virtual function for deciding whether to visit a node.
//	//========================================================================
//	virtual bool ShouldVisit(MObject &i_Object);
//};
//
////============================================================================
////============================================================================
//class JointIterator : public SceneIterator
//{
//protected:
//	//========================================================================
//	// Virtual function for deciding whether to visit a node.
//	//========================================================================
//	virtual bool ShouldVisit(MObject &i_Object);
//};
