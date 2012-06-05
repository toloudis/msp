/****************************************************************************\
**	mdlSkinUtil.hpp
**
**		mdlSkinUtil.hpp supplies functions to manipulate skinning information
**	into different formats.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_SKINUTIL_HPP
#error mdlSkinUtil.hpp multiply included
#endif
#define MDL_SKINUTIL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#include <map>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
struct smdlBoneVertex;
struct smdlMorphTarget;


//============================================================================
//	Any of these mdlSkinUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSkinUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct JointInfluence
	{
		int m_nJointIndex;
		envType::UInt32 m_nVertexIndex;
		envType::Float32 m_fWeight;
	};

	//------------------------------------------------------------------------
	// Remap influences from Maya indexing to our indexing.
	//------------------------------------------------------------------------
	void BuildBoneVertices(	const std::vector<mdlSkinUtil::JointInfluence>& i_JointInfluences,
							int i_NumVertices,
							const std::multimap<int, int> &i_VertexRemap,
							std::vector<smdlBoneVertex>& o_BoneVertices );
	//						mdlSkinInfo& o_SkinInfo );

	
	//------------------------------------------------------------------------
	// BuildMorphTarget - remap positions or deltas from a morph target
	// into a sorted vertex list that matches the fragment info.
	//------------------------------------------------------------------------
	void BuildMorphTarget(const std::vector<maPoint3d> &i_PositionVecs,
							const std::vector<envType::UInt32>& i_SparseIndices,
							int i_NumVertices,
							const std::multimap<int, int> &i_VertexRemap,
							smdlMorphTarget& o_MorphTarget);

}

