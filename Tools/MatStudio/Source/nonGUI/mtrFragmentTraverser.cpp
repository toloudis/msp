/*****************************************************************************
**  mtrFragmentTraverser.cpp
**
**	mtrFragmentTraverser applies operation to each fragment
**  in scObject. 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "mtrFragmentTraverser.hpp"

#include "g3dSceneNode.hpp"
#include "scObject.hpp"

namespace mtrFragmentTraverser
{

//========================================================================
//	TraverseFragment()
//
//	Traverse the tree and update each node.	
//========================================================================
void TraverseFragment( const maMatrix4x4& i_LastMatrix, 
					   g3dSceneNode& i_CurNode,
					   mtFragmentOp& i_FragmentOp )
{
	maMatrix4x4 cur_matrix = i_CurNode.GetTransform() * i_LastMatrix;
	g3dFragment* frag_ptr = i_CurNode.GetFragment();
	if ( frag_ptr )
	{
		i_FragmentOp.Apply( cur_matrix, frag_ptr );
	}

	//	process children
	int num_children = i_CurNode.GetNumChildren();
	int i;
	for( i = 0 ; i < num_children ; i++ )
	{
		TraverseFragment( cur_matrix, *( i_CurNode.GetChild(i) ), i_FragmentOp );
	}
}

//========================================================================
// TraverseFragments()
//========================================================================
void TraverseFragments( scObject* i_Object,
						mtFragmentOp& i_FragmentOp,
						const maMatrix4x4& i_Matx )
{
	g3dSceneNode* root = i_Object->GetBase();
	if ( root )
	{
		TraverseFragment( i_Matx, *root, i_FragmentOp );
	}
}

}	// end of namespace

