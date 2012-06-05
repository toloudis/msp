/*****************************************************************************
**	scCompoundObject.cpp
**
**		scCompoundObject is a scene object made up of multiple
**	fragments which can be moved around together but does not
**	supply any functionality for animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scCompoundObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//	The constructor requires a pointer to an array of fragments
//  to make the model from.  The model will initially be
//	positioned at (0, 0, 0), with unit scale and no rotation.
//--------------------------------------------------------------------
scCompoundObject::scCompoundObject( const std::vector<g3dFragment*>& i_Fragments,
									int i_NumLowRes,
									int i_NumHighRes )
{
	SetPeriod( 11 );

	int nFrags = i_Fragments.size();
	g3dSceneNode *pNode, *pBaseNode = GetBase();

	// Do low resolution if we have a mixed set of fragments
	// (some high res and some low res)
	m_bHasLowRes = ( (i_NumLowRes > 0) && (i_NumLowRes<nFrags) ) ||
				   ( (i_NumHighRes > 0) && (i_NumHighRes<nFrags) );

	for( int i = 0; i < nFrags; ++i )
	{
		pNode = new g3dSceneNode();
		pNode->SetFragment( i_Fragments[i] );
		pBaseNode->AddChild( pNode );

		if (m_bHasLowRes)
		{
			bool high_res = (i >= nFrags-i_NumHighRes);
			if (high_res)
				pNode->SetContentResolution( g3dSceneNode::e_HighRes );
			else
			{
				bool low_res = (i >= nFrags-i_NumHighRes-i_NumLowRes);
				if (low_res)
					pNode->SetContentResolution( g3dSceneNode::e_LowRes );
				else
					pNode->SetContentResolution( g3dSceneNode::e_Mixed );
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scCompoundObject::~scCompoundObject()
{
}


//--------------------------------------------------------------------
// If implementations have low resolution models, return true here.
//	Default returns false.
//--------------------------------------------------------------------
bool scCompoundObject::HasLowResolutionModel() const
{
	return m_bHasLowRes;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
scLODCompoundObject::scLODCompoundObject( const std::vector<std::vector<g3dFragment*> >& i_FragmentLists )
: scCompoundObject( (i_FragmentLists[0]) )
{
	SetPeriod( 11 );

	int nFragLists = i_FragmentLists.size();

	m_LODBaseNodes.resize(nFragLists);

	for (int j = 0; j < nFragLists; j++)
	{
		int nFrags = i_FragmentLists[j].size();
		g3dSceneNode *pNode, *pBaseNode = new g3dSceneNode;

		m_LODBaseNodes[j] = pBaseNode;

		for( int i = 0; i < nFrags; ++i )
		{
			pNode = new g3dSceneNode();
			pNode->SetFragment( (i_FragmentLists[j])[i] );
			pBaseNode->AddChild( pNode );
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scLODCompoundObject::~scLODCompoundObject()
{
	envSTLHelpers::DeleteContainer(m_LODBaseNodes);
}

