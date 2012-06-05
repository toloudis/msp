/****************************************************************************\
**  mdlFragInfoSinkKDTree.cpp
**
**      mdlFragInfoSinkKDTree is a mdlFragInfoSink which adds it's triangles
**	to the kdTree it was initialized with.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlFragInfoSinkKDTree.hpp"

#include "Core/geo/geoKDTree.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlFragInfoSinkKDTree::mdlFragInfoSinkKDTree(geoKDTree& o_Tree)
:	m_Tree(o_Tree)
{
}

//--------------------------------------------------------------------
//	i_Fragments is an array of fragments that was created for the
//	given frag info.  It is possible the array will be empty
//	for single skin and jointed objects.
//--------------------------------------------------------------------
void mdlFragInfoSinkKDTree::ReceiveFragInfo( const std::vector<g3dFragment*> &i_Fragments,
											 const entFragInfo& i_FragInfo )
{
	// Maybe we can move this to ent?

	const mdlFragInfo* mayfrag_info = dynamic_cast<const mdlFragInfo*>(&i_FragInfo);
	if (mayfrag_info)
	{
		//	we ignore the fragment handles here
		m_Tree.AddTriangles( &( mayfrag_info->m_Vertices[0] ),
							mayfrag_info->m_Vertices.size(),
							&( mayfrag_info->m_Indices[0] ),
							mayfrag_info->m_Indices.size() );
	}
}
