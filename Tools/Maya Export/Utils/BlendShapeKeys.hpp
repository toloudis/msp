/*****************************************************************************
**  BlendShapeKeys.hpp
**
**   Namespace for baking animations from blend shape weights  
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef BLEND_SHAPE_KEYS_HPP
#error BlendShapeKeys.hpp multiply included
#endif
#define BLEND_SHAPE_KEYS_HPP

#ifndef ANIM_KEYS_HPP
#include "AnimKeys.hpp"
#endif

#include <maya/MFnBlendShapeDeformer.h>

#include <list>

class MDagPath;
class MFnMesh;
class MFnSubd;

namespace BlendShapeKeys
{
//============================================================================
// Supporting data structures
//============================================================================

	struct BlendKeys
	{
		AnimKeys::BlendKeys	m_BlendKeys;
		//MFnBlendShapeDeformer m_Blend;
		MObject m_BlendShapeObject;
		int m_WeightIndex;
	};

//============================================================================
// Blend shape baking
//============================================================================

	//========================================================================
	// Gathers list of blend shapes for this subdiv
	//========================================================================
	//void GatherBlendShapes(MFnSubd &i_Subdiv,
	//					   std::list<BlendKeys> &o_Keys);

	//========================================================================
	// Gathers list of blend shapes for this mesh
	//========================================================================
	void GatherBlendShapes(MFnMesh &i_Mesh,
						   std::list<BlendKeys> &o_Keys);

	//========================================================================
	// Gets current state of blend shape weights and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<BlendKeys> &io_Keys, 
					float i_CurrentTime);

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer,
						const std::list<BlendKeys> &o_Keys,
						float i_TimeOffset);

}
