/*****************************************************************************
**	scCompoundObject.hpp
**
**		scCompoundObject is a scene object made up of multiple
**	fragments which can be moved around together but does not
**	supply any functionality for animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_COMPOUNDOBJECT_HPP
#error scCompoundObject.hpp multiply included
#endif
#define SC_COMPOUNDOBJECT_HPP

#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;


//============================================================================
//============================================================================
class scCompoundObject : public scObject
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//		Low resolution fragments are assumed to be the last 
		//		"i_NumLowRes" number of fragments in the list.
		//--------------------------------------------------------------------
		scCompoundObject( const std::vector<g3dFragment*>& i_Fragments,
						  int i_NumLowRes = 0,
						  int i_NumHighRes = 0);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~scCompoundObject();

		//--------------------------------------------------------------------
		// If implementations have low resolution models, return true here.
		//	Default returns false.
		//--------------------------------------------------------------------
		virtual bool HasLowResolutionModel() const;

	private:
		bool m_bHasLowRes;
};


//============================================================================
//	scLODCompoundObject is the same as above, but with support for
//	multiple LODs of said object
//============================================================================
class scLODCompoundObject : public scCompoundObject
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		scLODCompoundObject( const std::vector< std::vector<g3dFragment*> >& i_FragmentLists );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~scLODCompoundObject();

	private:
		std::vector< g3dSceneNode* >	m_LODBaseNodes;
};
