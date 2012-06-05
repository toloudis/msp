/*****************************************************************************
**	smdlMeshGPUSkin.hpp
**
**		smdlMeshGPUSkin handles a single skinned polygon mesh
**	with no remapping that is animated on the GPU.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlMeshGPUSkin.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/smdlBoneVertex.hpp"


//--------------------------------------------------------------------
//	smdlMeshGPUSkin requires the skinning and bone vertex data. 
//	Assumes ownership of the fragment. Bone vertices can be shared
//--------------------------------------------------------------------
smdlMeshGPUSkin::smdlMeshGPUSkin(	const mdlSplitFragInfo& i_SplitFragInfo,
									const std::vector<smdlBoneVertex>& i_BoneVertices,
									const maMatrix4x4& i_BindPose )
:	m_nNumVertices( i_BoneVertices.size() ),
	m_BindPose( i_BindPose )
{
	// NOTE: we need error checks on the usage of this class. There are limits on the
	// size of the skinning palette, the number of influences per vertex and
	// the bind pose passed in needs to identity, or the bind pose needs to be
	// combined with the skinning palette below.

	// Create fragment
	m_pFragment =  mdlFragCreate::CreateFragment(i_SplitFragInfo, true);

	// Process the bone vertices and figure out which bone indices we really need
	std::map<int,int> bone_index_map;
	for (int i=0; i<m_nNumVertices; ++i)
	{
		const smdlBoneVertex &bone_vertex = i_BoneVertices[i];
		// could check bone_vertex.m_Influences.size() for too many influences here
		std::vector<scBoneInfluence>::const_iterator it, end = bone_vertex.m_Influences.end();
		for (it = bone_vertex.m_Influences.begin(); it != end; ++it)
		{
			if (bone_index_map.find(it->m_BoneIndex) == bone_index_map.end())
			{
				// Map from original bone indices based on whole character
				// to shorter list of just the matrices used by this mesh
				bone_index_map[it->m_BoneIndex] = m_BoneIndices.size();
				m_BoneIndices.push_back(it->m_BoneIndex);
			}
		}
	}

	// Create and fill the skinning buffer
	m_pFragment->SetHasSkinning(true);
	g3dType::SkinVertex *skin_vertices = reinterpret_cast<g3dType::SkinVertex*>(m_pFragment->LockSkinning());
	const int c_MaxWeightsPerVertex = 4; // because we are using maVector4d in SkinVertex
	int w, nInfs;
	for (int i=0; i<m_nNumVertices; ++i)
	{
		const smdlBoneVertex &bone_vertex = i_BoneVertices[i];
		nInfs = maFunctions::Lowest((int)bone_vertex.m_Influences.size(), c_MaxWeightsPerVertex);
		for (w=0; w<nInfs; w++)
		{
			// Use local bone index by doing a lookup into the map
			skin_vertices[i].m_Bones[w] = bone_index_map[bone_vertex.m_Influences[w].m_BoneIndex];
			skin_vertices[i].m_Weights[w] = bone_vertex.m_Influences[w].m_fWeight;
		}
	}
	m_pFragment->UnlockSkinning();

	// Resize the skinning palette of matrices to the number of matrices that
	// this mesh uses.
	m_pFragment->GetSkinningPalette().resize(m_BoneIndices.size());

	// Create a node for our fragment so that we can control the visibility
	m_pNode = new g3dSceneNode( m_pFragment );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMeshGPUSkin::~smdlMeshGPUSkin()
{
	// Since fragments are not shared for single-skin, we should delete ours
	delete m_pFragment;
}

//--------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//--------------------------------------------------------------------
g3dSceneNode* smdlMeshGPUSkin::RootNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlMeshGPUSkin::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlMeshGPUSkin::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//--------------------------------------------------------------------
//	TransformMesh - given the joint matrices, transform the
//		vertices based on the vertex influences.
//--------------------------------------------------------------------
void smdlMeshGPUSkin::TransformMesh( const maMatrix4x4* i_BoneMatrices )
{
	int num_local_matxs = m_BoneIndices.size();
	for (int m=0; m<num_local_matxs; ++m)
	{
		// Use only the matrices that this mesh uses.
		// NOTE: need to include m_BindPose here eventually.
		int remap_index = m_BoneIndices[m];
		m_pFragment->GetSkinningPalette()[m] = i_BoneMatrices[ remap_index ];
	}
}
