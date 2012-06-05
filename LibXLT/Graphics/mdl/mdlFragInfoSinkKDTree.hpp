/****************************************************************************\
**	mdlFragInfoSinkKDTree.hpp
**
**		mdlFragInfoSinkKDTree is a mdlFragInfoSink which adds it's triangles
**	to the kdTree it was initialized with.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_FRAGINFOSINKKDTREE_HPP
#error mdlFragInfoSinkKDTree.hpp multiply included
#endif
#define MDL_FRAGINFOSINKKDTREE_HPP

#ifndef ENT_FRAGINFOSINK_HPP
#include "Graphics/ent/entFragInfoSink.hpp"
#endif


//============================================================================
//============================================================================
class geoKDTree;


//============================================================================
//============================================================================
class mdlFragInfoSinkKDTree : public entFragInfoSink
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mdlFragInfoSinkKDTree(geoKDTree& o_Tree);

		//--------------------------------------------------------------------
		//	i_Fragments is an array of fragments that was created for the
		//	given frag info.  It is possible the array will be empty
		//	for single skin and jointed objects.
		//--------------------------------------------------------------------
		virtual void ReceiveFragInfo( const std::vector<g3dFragment*> &i_Fragments,
									  const entFragInfo& i_FragInfo );

	private:
		geoKDTree& m_Tree;
};
