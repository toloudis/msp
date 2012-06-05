/*****************************************************************************
**  smdlInfluenceSorter.hpp
**
**      smdlInfluenceSorter inserts fragments into the hierarchy to represent
**	the joints in a character skeleton.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlInfluenceSorter.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/ma/maSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include <iterator>

//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	template<class T>
	envType::UInt32 join_vertices(mdlFragInfo &o_ToAlter,
					   const T& i_ToAdd)
	{
		envType::UInt32 vertex_base = o_ToAlter.m_Vertices.size();
		//int index_base = o_ToAlter.m_Indices.size();

		// Indices go into separate array to be sorted
		// (indices need to be offset because of vertex index changes)
		//std::transform(	i_ToAdd.m_Indices.begin(),
		//				i_ToAdd.m_Indices.end(),
		//				std::back_inserter(o_Indices),
		//				maAdder(vertex_base));

		// Vertex info is joined together into large list
		std::copy(	i_ToAdd.m_Vertices.begin(),
					i_ToAdd.m_Vertices.end(),
					std::back_inserter(o_ToAlter.m_Vertices));

		std::copy(	i_ToAdd.m_Normals.begin(),
					i_ToAdd.m_Normals.end(),
					std::back_inserter(o_ToAlter.m_Normals));

		//	if there are UVs in i_ToAdd but we didn't have any before,
		//	fill our array up to the right index
		if( (i_ToAdd.m_UVs.size() > 0) && (o_ToAlter.m_UVs.size() < vertex_base) )
			o_ToAlter.m_UVs.resize(vertex_base);

		std::copy(	i_ToAdd.m_UVs.begin(),
					i_ToAdd.m_UVs.end(),
					std::back_inserter(o_ToAlter.m_UVs));

		return vertex_base;
	}

}

//--------------------------------------------------------------------
//	smdlInfluenceSorter requires the root scene node of the skeleton.
//--------------------------------------------------------------------
smdlInfluenceSorter::smdlInfluenceSorter(g3dSceneNode *i_pRootNode)
{
	gather_joints(i_pRootNode, m_Joints, -1);

	m_JoinedFrag.m_Flags.m_bCastsShadow = false;
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
smdlInfluenceSorter::~smdlInfluenceSorter()
{
}

//--------------------------------------------------------------------
// Submit the given fragment and skin into the sorter
//--------------------------------------------------------------------
void smdlInfluenceSorter::Submit(const mdlFragInfo &i_FragInfo, 
								 const smdlCharacterSkin &i_Skin)
{
	Submit<mdlFragInfo>(i_FragInfo, i_Skin);
}
void smdlInfluenceSorter::Submit(const mdlSplitFragInfo &i_SplitFrag, 
								 const smdlCharacterSkin &i_Skin)
{
	Submit<mdlSplitFragInfo>(i_SplitFrag, i_Skin);
}
//--------------------------------------------------------------------
// private generic version
//--------------------------------------------------------------------
template<class T>
void smdlInfluenceSorter::Submit(const T &i_Frag, 
								 const smdlCharacterSkin &i_Skin)
{
	// Make sure rendering flags are the same,
	// use the first fragment to get the flags
	//if (m_JoinedFrag.m_Vertices.empty())
	//	m_JoinedFrag.m_Flags = i_SplitFrag.m_Flags;

	// Find which bone influences each vertex the most
	int num_verts = i_Skin.m_BoneVertices.size();
	if (num_verts != i_Frag.m_Vertices.size()) return;
	std::vector<int> bone_indices(num_verts, -1);
	for (int v=0; v<num_verts; v++)
	{
		const smdlBoneVertex &bone_vert = i_Skin.m_BoneVertices[v];
		if (!bone_vert.m_Influences.empty())
		{
			// Find largest influence
			float max = 0.0f;
			for (int i=0; i<bone_vert.m_Influences.size(); i++)
			{
				if (bone_vert.m_Influences[i].m_fWeight > max)
				{
					bone_indices[v] = bone_vert.m_Influences[i].m_BoneIndex;
					max = bone_vert.m_Influences[i].m_fWeight;
				}
			}	
		}
	}

	// Join all vertices together, return offset for 
	// indices into new array.
	envType::UInt32 index_offset = join_vertices<T>(m_JoinedFrag, i_Frag);

	// Now sort the triangles into the indices for each joint
	int num_indices = i_Frag.m_Indices.size();
	envType::UInt32 a, b, c;
	for (int i=0; i<num_indices; i+=3)
	{
		a = i_Frag.m_Indices[i];
		b = i_Frag.m_Indices[i+1];
		c = i_Frag.m_Indices[i+2];
		
		// Choose lowest index (highest joint in tree?)
		int ji = maFunctions::Lowest(bone_indices[a], bone_indices[b], bone_indices[c]);
		// If all 3 vertices are influenced by the same joint,
		// then add the triangle to that joint's list
		//int ji = bone_indices[a];
		//if (ji == bone_indices[b] 
		//	&& ji == bone_indices[c])
		{
			if (ji >= 0)
			{
				DBG_ASSERT(ji < m_Joints.size(), "Joint index out of range, " << ji << " < " << m_Joints.size());
				if (ji < m_Joints.size())
				{
					sJointGather &gather = m_Joints[ji];
					gather.m_Indices.push_back(a+index_offset);
					gather.m_Indices.push_back(b+index_offset);
					gather.m_Indices.push_back(c+index_offset);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// CreateFragments from the fragments that has been submitted
//--------------------------------------------------------------------
void smdlInfluenceSorter::CreateFragments(matMaterial *i_pLowResMat,
									 	  std::vector<g3dFragment*> &o_Fragments,
										  std::vector<g3dSceneNode*> &o_ParentNodes)
{			
	if (m_Joints.empty())
		return;
	DBG_TRACE("Joined, Num vertices: " << m_JoinedFrag.m_Vertices.size());

	// Find which joints have enough triangles to be 
	// worth making a fragment.
	// Note: threshold needs to at least remove joints with 0 indices
	//const int threshold = 300; // 300 triangles was threshold in version 2.9
	const int threshold = 60; // reducing threshold in version 3.0
	//const int threshold = 0;
	std::vector<int> worthwhile;
	// Move backwards through array in order to move
	// triangles up to parent index (which is always lower)
	for (int j=m_Joints.size()-1; j >= 0; j--)
	{
		if (m_Joints[j].m_Indices.size() > threshold)
		{
			worthwhile.push_back(j);
		}
		else if (m_Joints[j].m_ParentIndex >= 0)
		{
			// Throw all of our triangles up into our parent's gather
			std::copy(	m_Joints[j].m_Indices.begin(),
						m_Joints[j].m_Indices.end(),
						std::back_inserter(m_Joints[m_Joints[j].m_ParentIndex].m_Indices));
		}
	}

	DBG_TRACE("Num worth while joints: " << worthwhile.size());
	if (worthwhile.empty()) return;

	// Now throw the indices into the joined fragment, 
	// duplicating the material in order to trick mayFragmentCreate
	// into splitting up the large fragment for us
	shared_ptr<mdlMatInfo> shared_material(new mdlMatInfo());
	shared_material->m_pMaterial = i_pLowResMat;

	for (int w=0; w<worthwhile.size(); w++)
	{
		sJointGather &gather = m_Joints[worthwhile[w]];

		// Add in shared material (again)
		m_JoinedFrag.m_Materials.push_back( shared_material );

		// Add material switch 
		if (w > 0)
			m_JoinedFrag.m_MaterialChanges.push_back(m_JoinedFrag.m_Indices.size() / 3);

		// Append the indices
		std::copy(	gather.m_Indices.begin(),
					gather.m_Indices.end(),
					std::back_inserter(m_JoinedFrag.m_Indices));

	}

	// Finish up fragment and split it up
	std::vector<mdlSplitFragInfo> split_frags;
	mdlFragUtil::SplitFragments(m_JoinedFrag, split_frags);

	// Insert static fragments directly into joint tree
	const bool morphable = false;
	DBG_ASSERT(worthwhile.size() == split_frags.size(), "Not enough fragments " << split_frags.size() << " expected " << worthwhile.size());
	if (worthwhile.size() != split_frags.size())
		return;
	for (int w=0; w<worthwhile.size(); w++)
	{
		sJointGather &gather = m_Joints[worthwhile[w]];
		mdlSplitFragInfo &split_frag = split_frags[w];

		// Optimize the split fragment
		mdlFragCreate::OptimizeFragment(split_frag);

		// Transform the vertices by the bind pose matrix in order
		// to allow them to be altered by the joint matrices
		maPointTransformer4x4InPlace xformer( gather.m_pJoint->GetInvBindPose() );
		envSTLHelpers::ForAll(split_frag.m_Vertices, xformer);

		// Transform normals
		maMatrix3x3 sub_mtx = gather.m_pJoint->GetInvBindPose().GetSubMatrix(3,3);
		maPointTransformer3x3InPlace nformer( sub_mtx );
		envSTLHelpers::ForAll(split_frag.m_Normals, nformer);

		// Create actual fragment from split_frag now
		g3dFragment *pFragment = mdlFragCreate::CreateFragment(split_frag, morphable);
		o_Fragments.push_back( pFragment );
		
		// Add in a low-res node to contain the merged fragment
		g3dSceneNode *pNode = new g3dSceneNode(pFragment);
		pNode->SetContentResolution( g3dSceneNode::e_LowRes );
		gather.m_pJoint->AddChild( pNode );
		o_ParentNodes.push_back( pNode );
	}
}

//--------------------------------------------------------------------
// internal routine for setting up structures for skeleton
//--------------------------------------------------------------------
void smdlInfluenceSorter::gather_joints(g3dSceneNode* i_pJoint,
										std::vector<sJointGather> &o_Joints,
										int i_ParentIndex)
{
	int cur_index = o_Joints.size();

	if (i_pJoint->GetIsJoint() && !i_pJoint->GetSkipAnim())
	{
		sJointGather gather = { i_pJoint, i_ParentIndex };
		o_Joints.push_back( gather );
	}

	int nChildren = i_pJoint->GetNumChildren();
	for( int i = 0 ; i < nChildren ; ++i )
	{
		gather_joints( i_pJoint->GetChild(i), o_Joints, cur_index );
	}
}
