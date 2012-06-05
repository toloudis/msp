/****************************************************************************\
**	g3dSceneTraverse.hpp
**
**	A g3dSceneTraverse is a utility class that recursively follows a 
**  tree depth-first, calling a given function pointer at each node.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENETRAVERSE_HPP
#error g3dSceneTraverse.hpp multiply included
#endif
#define G3D_SCENETRAVERSE_HPP

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNodeProcessor
{
public:
	//--------------------------------------------------------------------
	//	return true if traversal should continue. this will be called
	//	before children are traversed. can push state here.
	//--------------------------------------------------------------------
	virtual bool ProcessNode(g3dSceneNode* i_pNode) = 0;

	//--------------------------------------------------------------------
	//	after children are traversed, we might want to do something with 
	//	the node again (e.g. pop some state)
	//--------------------------------------------------------------------
	virtual void PostProcessNode(g3dSceneNode* i_pNode) {}
};


//============================================================================
//============================================================================
namespace g3dSceneTraverse
{
	//--------------------------------------------------------------------
	//	traverse nodes recursively, depth-first.
	//--------------------------------------------------------------------
	void Traverse(g3dSceneNode *i_Node, g3dSceneNodeProcessor* i_pFunctor);

} // namespace
