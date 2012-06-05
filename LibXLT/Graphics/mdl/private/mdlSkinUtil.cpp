/****************************************************************************\
**	mdlSkinUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlSkinUtil.hpp"

#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include <algorithm>
#include <set>


//============================================================================
//	Any of these mdlSkinUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSkinUtil
{
	namespace
	{
		//------------------------------------------------------------------------
		// Figures out the set of joint indices used by a surface
		//------------------------------------------------------------------------
		void find_set_of_influences(const std::vector<mdlSkinUtil::JointInfluence>& i_JointInfluences,
									std::vector<envType::UInt32> &o_BoneIndices)
		{
			// Insert all joint indices into a set in order to figure out
			// which joints are needed.
			std::set<envType::UInt32> joint_index_set;
			std::vector<mdlSkinUtil::JointInfluence>::const_iterator it1;
			for (it1 = i_JointInfluences.begin(); it1 != i_JointInfluences.end(); ++it1)
			{
				joint_index_set.insert( it1->m_nJointIndex );
			}

			// Add the unique indices to the return vector
			o_BoneIndices.clear();
			std::set<envType::UInt32>::const_iterator it2;
			for (it2 = joint_index_set.begin(); it2 != joint_index_set.end(); ++it2)
			{
				o_BoneIndices.push_back( *it2 );
			}
		}
	}


	//------------------------------------------------------------------------
	// Remap influences from Maya indexing to our indexing.
	//------------------------------------------------------------------------
	//void BuildBoneVertices(	const std::vector<mdlSkinUtil::JointInfluence>& i_JointInfluences,
	//						int i_NumVertices,
	//						const std::multimap<int, int> &i_VertexRemap,
	//						mdlSkinWeights& o_SkinInfo )
	//{
	//	// Find which joints are needed
	//	find_set_of_influences(i_JointInfluences, o_SkinInfo.m_BoneIndices);
	//	int nJoints = o_SkinInfo.m_BoneIndices.size();

	//	// Create a map the other way around, from absolute joint index to index within
	//	// the shorter used list we just made.
	//	std::map<int, int> joint_map;
	//	for (int j=0; j<nJoints; j++)
	//		joint_map[o_SkinInfo.m_BoneIndices[j]] = j;

	//	typedef std::multimap<int, int>::const_iterator MapIt;

	//	int nJointInfluences = i_JointInfluences.size();
	//	
	//	// Expand out the weights array, which is ordered by vertex with one weight per 
	//	// bone in the m_BoneIndices array.
	//	o_SkinInfo.m_NumVertices = i_NumVertices;
	//	if (i_NumVertices > 0 && nJoints > 0)
	//	{
	//		o_SkinInfo.m_Weights.resize(i_NumVertices * nJoints, 0.0f);
	//		for( int i = 0; i < nJointInfluences; ++i )
	//		{
	//			const JointInfluence& joint_infl = i_JointInfluences[i];

	//			std::pair<MapIt, MapIt> range = i_VertexRemap.equal_range( joint_infl.m_nVertexIndex );

	//			while( range.first != range.second )
	//			{
	//				int nNewVertexIndex = range.first->second;
	//				int bone_index = joint_map[joint_infl.m_nJointIndex];
	//				o_SkinInfo.Weight(nNewVertexIndex, bone_index) = joint_infl.m_fWeight;
	//				++range.first;
	//			}
	//		}
	//	}
	//}

	//------------------------------------------------------------------------
	// Remap influences from Maya indexing to our indexing.
	//------------------------------------------------------------------------
	void BuildBoneVertices(	const std::vector<mdlSkinUtil::JointInfluence>& i_JointInfluences,
							int i_NumVertices,
							const std::multimap<int, int> &i_VertexRemap,
							std::vector<smdlBoneVertex>& o_BoneVertices )
	{
		typedef std::multimap<int, int>::const_iterator MapIt;

		int nJointInfluences = i_JointInfluences.size();

		o_BoneVertices.clear();
		o_BoneVertices.resize( i_NumVertices );

		for( int i = 0; i < nJointInfluences; ++i )
		{
			const JointInfluence& joint_infl = i_JointInfluences[i];

			std::pair<MapIt, MapIt> range = i_VertexRemap.equal_range( joint_infl.m_nVertexIndex );

			while( range.first != range.second )
			{
				int nNewVertexIndex = range.first->second;

				smdlBoneVertex& bone_vertex = o_BoneVertices[ nNewVertexIndex ];
				//bone_vertex.m_Position = i_FragInfo.m_Vertices[ nNewVertexIndex ];
				//bone_vertex.m_Normal = i_FragInfo.m_Normals[ nNewVertexIndex ];

				scBoneInfluence bone_infl;
				bone_infl.m_BoneIndex = joint_infl.m_nJointIndex;
				bone_infl.m_fWeight = joint_infl.m_fWeight;
				bone_vertex.m_Influences.push_back( bone_infl );

				++range.first;
			}
		}
		
		// Debug bone vertices
		//for (int i=0; i<i_NumVertices; i++)
		//{
		//	smdlBoneVertex& bone_vertex = o_BoneVertices[ i ];
		//	DBG_LOG5("Vert %d: Pos: %4f %4f %4f Num Infl: %d", i, bone_vertex.m_Position.m_X, bone_vertex.m_Position.m_Y, bone_vertex.m_Position.m_Z, bone_vertex.m_Influences.size() );
		//	for (int w=0; w< bone_vertex.m_Influences.size(); w++)
		//	{
		//		DBG_LOG2("    BoneIndex %d Weight %f", bone_vertex.m_Influences[w].m_BoneIndex, bone_vertex.m_Influences[w].m_fWeight ); 
		//	}
		//}
	}

	//------------------------------------------------------------------------
	// BuildMorphTarget - remap positions or deltas from a morph target
	// into a sorted vertex list that matches the fragment info.
	//------------------------------------------------------------------------
	void BuildMorphTarget(const std::vector<maPoint3d> &i_PositionVecs,
							const std::vector<envType::UInt32>& i_SparseIndices,
							int i_NumVertices,
							const std::multimap<int, int> &i_VertexRemap,
							smdlMorphTarget& o_MorphTarget)
	{
		o_MorphTarget.m_Offsets.resize( i_NumVertices );

		const maPoint3d* pVertices = &i_PositionVecs[0];
		std::vector<maPoint3d> unsparse;
		if (!i_SparseIndices.empty())
		{
			unsparse.resize(i_NumVertices, maPoint3d(0,0,0));
			for (int i=0; i<i_SparseIndices.size(); ++i)
			{
				unsparse[i_SparseIndices[i]] = i_PositionVecs[i];
			}
			pVertices = &unsparse[0];
		}

		std::multimap<int, int>::const_iterator it;
		for( it = i_VertexRemap.begin(); it != i_VertexRemap.end(); ++it )
		{
			int nOldVertexIndex = it->first;
			int nNewVertexIndex = it->second;

			o_MorphTarget.m_Offsets[ nNewVertexIndex ] = pVertices[ nOldVertexIndex ];
		}	
	}

}

