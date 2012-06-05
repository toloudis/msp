/****************************************************************************\
**	g3dSceneTraverse.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSceneTraverse.hpp"


//============================================================================
//============================================================================
namespace g3dSceneTraverse
{
	//--------------------------------------------------------------------
	//	traverse nodes recursively, depth-first.
	//--------------------------------------------------------------------
	void Traverse(g3dSceneNode *i_Node, g3dSceneNodeProcessor* i_pFunctor)
	{
		// can i ignore null nodes here? or should it be left to the nodeprocessor?
		if (i_Node != NULL)
		{
			bool ok = i_pFunctor->ProcessNode(i_Node);
			if (ok)
			{
				for (int i = 0; i < i_Node->GetNumChildren(); i++)
				{
					Traverse(i_Node->GetChild(i), i_pFunctor);
				}
			}
			i_pFunctor->PostProcessNode(i_Node);
		}
	}

} // namespace

