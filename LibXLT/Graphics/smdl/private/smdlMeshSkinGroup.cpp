/*****************************************************************************
**	smdlMeshSkinGroup.hpp
**
**		smdlMeshSkinGroup handles a multi-material mesh that was a single
**	mesh in Maya, but is now multiple meshes. It handles both skinning and
**	vertex animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/g3d/g3dIndexPtr.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//  transform_normal
	//--------------------------------------------------------------------
	inline maVector3d transform_normal( const maMatrix4x4& i_pTransform, const maVector3d& i_Normal )
	{
		return maVector3d(	( i_pTransform.m_Mat[0] * i_Normal.m_X + i_pTransform.m_Mat[4] * i_Normal.m_Y + i_pTransform.m_Mat[8] * i_Normal.m_Z ),
							( i_pTransform.m_Mat[1] * i_Normal.m_X + i_pTransform.m_Mat[5] * i_Normal.m_Y + i_pTransform.m_Mat[9] * i_Normal.m_Z ),
							( i_pTransform.m_Mat[2] * i_Normal.m_X + i_pTransform.m_Mat[6] * i_Normal.m_Y + i_pTransform.m_Mat[10] * i_Normal.m_Z ) );
	}

	//--------------------------------------------------------------------
	// deforms a single vertex according to morph target weights
	//--------------------------------------------------------------------
	inline maPoint3d morph_vertex( const maPoint3d &i_BasePosition, 
							const std::vector< shared_ptr<smdlMorphTarget> > &i_MorphTargets, 
							const std::vector<float> &i_Weights, 
							int i_VertexIndex)
	{
		if (i_MorphTargets.empty()) 
		{
			return i_BasePosition;
		}
		else
		{
			int nWeights = i_Weights.size();
			DBG_ASSERT(nWeights == i_MorphTargets.size(), "Weights array doesn't match number of morph targets");
			if (nWeights != i_MorphTargets.size())
				return i_BasePosition;
			maPoint3d pos = i_BasePosition;
			for (int w=0; w<nWeights; w++)
			{
				if (i_MorphTargets[w]->m_bOffsetsAreDeltas)
					pos += i_MorphTargets[w]->m_Offsets[i_VertexIndex] * i_Weights[w];
				else
					pos += (i_MorphTargets[w]->m_Offsets[i_VertexIndex] - i_BasePosition) * i_Weights[w];
			}
			return pos;
		}
	}

	//====================================================================
	// Creates basis vectors, based on a vertex and index list.
	// Copied from bumpBufferUtil.cpp, needs to be in shared
	// utility class.
	//====================================================================
	void create_basis_vectors(g3dType::BumpTex1Vertex *io_Vertices,
		int	i_nVertices,
		const envType::UInt32* i_pIndices,
		int	i_nIndices,
		bool i_bDoFullComputation = true)
	{
		// Clear the basis vectors
		int i;
		for (i = 0; i < i_nVertices; i++)
		{
			io_Vertices[i].m_S = maPoint3d(0.0f, 0.0f, 0.0f);
			io_Vertices[i].m_T = maPoint3d(0.0f, 0.0f, 0.0f);
		}

		// Flag controls whether to skip computation:
		if (!i_bDoFullComputation)
			return;

		if (i_nIndices %3 != 0)
			return;

		// Walk through the triangle list and calculate gradiants for each triangle.
		// Sum the results into the S and T components.
		maVector3d S, T, edge01, edge02, cp;
		for( i = 0; i < i_nIndices; i += 3 )
		{
			g3dType::BumpTex1Vertex& v0 = io_Vertices[i_pIndices[i]];
			g3dType::BumpTex1Vertex& v1 = io_Vertices[i_pIndices[i+1]];
			g3dType::BumpTex1Vertex& v2 = io_Vertices[i_pIndices[i+2]];

			S.Set(0,0,0);
			T.Set(0,0,0);

			// x, s, t
			edge01.Set( v1.m_Vertex.m_X - v0.m_Vertex.m_X, v1.m_TexCoord.m_X - v0.m_TexCoord.m_X, v1.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );
			edge02.Set( v2.m_Vertex.m_X - v0.m_Vertex.m_X, v2.m_TexCoord.m_X - v0.m_TexCoord.m_X, v2.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_X = -cp.m_Y / cp.m_X;
				T.m_X = -cp.m_Z / cp.m_X;
			}

			// y, s, t
			edge01.Set( v1.m_Vertex.m_Y - v0.m_Vertex.m_Y, v1.m_TexCoord.m_X - v0.m_TexCoord.m_X, v1.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );
			edge02.Set( v2.m_Vertex.m_Y - v0.m_Vertex.m_Y, v2.m_TexCoord.m_X - v0.m_TexCoord.m_X, v2.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Y = -cp.m_Y / cp.m_X;
				T.m_Y = -cp.m_Z / cp.m_X;
			}


			// z, s, t
			edge01.Set( v1.m_Vertex.m_Z - v0.m_Vertex.m_Z, v1.m_TexCoord.m_X - v0.m_TexCoord.m_X, v1.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );
			edge02.Set( v2.m_Vertex.m_Z - v0.m_Vertex.m_Z, v2.m_TexCoord.m_X - v0.m_TexCoord.m_X, v2.m_TexCoord.m_Y - v0.m_TexCoord.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Z = -cp.m_Y / cp.m_X;
				T.m_Z = -cp.m_Z / cp.m_X;
			}

			S.Normalize();
			T.Normalize();

			// Now add normalized vector to actual vertex
			v0.m_S += S; v0.m_T += T;
			v1.m_S += S; v1.m_T += T;
			v2.m_S += S; v2.m_T += T;
		}

		// Calculate the SxT vector
		maVector3d vecSxT;
		for(i = 0; i < i_nVertices; i++)
		{
			// Normalize the S, T vectors
			io_Vertices[i].m_S.Normalize();
			io_Vertices[i].m_T.Normalize();
		} 
	}

}


//--------------------------------------------------------------------
//	smdlMeshSkinGroup represents a skinned mesh with multiple
//	materials. It has to split the surface into multiple fragments
//	and keep track of remapping arrays to animate them.
//--------------------------------------------------------------------
smdlMeshSkinGroup::smdlMeshSkinGroup(const mdlFragInfo& i_FragInfo,
									 const smdlCharacterSkin& i_CharacterSkin,
									 const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
									 bool i_bVertexAnimation)
:	m_CharacterSkin( i_CharacterSkin ),
	m_MeshName(i_FragInfo.m_Name),
	m_nNumVertices( i_FragInfo.m_Vertices.size() ),
	m_NumOriginalVertices( i_FragInfo.m_NumOrigVertices ),
	m_NumOriginalNormals( i_FragInfo.m_NumOrigNormals ),
	m_BasePositions( i_FragInfo.m_Vertices ),
	m_BaseNormals( i_FragInfo.m_Normals )
{
	std::vector<mdlSplitFragInfo> split_frags;
	mdlFragUtil::SplitFragments(i_FragInfo, split_frags, i_MaterialRemapping);

	// Create a mesh group
	for (int si=0; si<split_frags.size(); ++si)
	{
		g3dFragment *pFragment =  mdlFragCreate::CreateFragment(split_frags[si], true);
		m_Fragments.push_back(pFragment);
		m_SplitRemapping.push_back(split_frags[si].m_RemapArray);
		m_FragmentIndexVecs.push_back( split_frags[si].m_Indices );
	};
	DBG_ASSERT( m_Fragments.size() == m_FragmentIndexVecs.size(), "There should be as many fragments as there are fragmentIndexVecs" );
	DBG_ASSERT( m_Fragments.size() == m_SplitRemapping.size(), "There should be as many fragments as there are RemapArrays"  );

	// Full remapping for vertex animation
	if (i_bVertexAnimation || i_FragInfo.m_Flags.m_bVertexAnimation)
	{
		mdlFragUtil::GenerateFullRemappings(i_FragInfo, 
											  split_frags, 
											  m_FullVertexRemapping, 
											  m_FullNormalRemapping);
	}

	// Create root node for our fragments so that we can control the visibility
	m_pNode = new g3dSceneNode();
	m_pNode->SetName( m_MeshName.c_str() );
	
	// Add fragments into small hierarchy under the root node
	//DBG_LOG("MeshSkinGroup: Num fragments: " << m_Fragments.size());
	for (int i=0; i<m_Fragments.size(); i++)
	{
		g3dSceneNode *node = new g3dSceneNode;
		node->SetFragment( m_Fragments[i] );
		
		// Mark scene node to signify that the contents are low-res, if needed.
		// It is possible to have vertex animated low resolution meshes.
		g3dSceneNode::Resolution resolution;
		switch (i_FragInfo.m_ResolutionLevel)
		{
		default:
		case 0:
			resolution = g3dSceneNode::e_Mixed;
			break;
		case 1:
			resolution = g3dSceneNode::e_LowRes;
			break;
		case 2:
			resolution = g3dSceneNode::e_HighRes;
			break;
		}
		node->SetContentResolution( resolution );

		m_pNode->AddChild(node);
	}

	//DBG_LOG("MeshSkinGroup: Num vertices in skin: " << m_nNumVertices);
	for (int m=0; m<m_CharacterSkin.m_MorphTargets.size(); m++)
	{
		//DBG_LOG2(" MorphTarget (%d): Num verts: %d", m, m_CharacterSkin.m_MorphTargets[m].m_Offsets.size());
	}
}

//--------------------------------------------------------------------
//	smdlMeshSkinGroup requires the fragments and bone vertex data.
//	Assumes ownership of the fragments. Bone vertices can be shared
//
//	The i_SplitRemapping array matches from the vertices in the 
//	fragments to the single array of bone vertices.  There should be
//	one RemapArray per fragment and each RemapArray should have
//	an index into the BoneVertices array per vertex in the
//	fragment.
//
//	The i_OrgVertexRemapping and i_OrgNormalRemapping arrays are 
//	only needed if the mesh will be	vertex animated. These should
//	come from the original mdlFragInfo's remapping from Maya data.
//--------------------------------------------------------------------
//smdlMeshSkinGroup::smdlMeshSkinGroup(const std::vector<g3dFragment*>& i_Fragments,
//									const std::vector<RemapArray> &i_SplitRemapping,
//									const std::vector<RemapArray> &i_FullVertexRemapping,
//									const std::vector<RemapArray> &i_FullNormalRemapping,
//									const smdlCharacterSkin& i_CharacterSkin,
//									const std::string& i_MeshName,
//									int i_NumOriginalVertices,
//									int i_NumOriginalNormals )
//:	m_CharacterSkin( i_CharacterSkin ),
//	m_MeshName(i_MeshName),
//	m_nNumVertices( i_CharacterSkin.m_BoneVertices.size() ),
//	m_Fragments( i_Fragments ),
//	m_SplitRemapping( i_SplitRemapping ),
//	m_FullVertexRemapping( i_FullVertexRemapping ),
//	m_FullNormalRemapping( i_FullNormalRemapping ),
//	m_NumOriginalVertices( i_NumOriginalVertices ),
//	m_NumOriginalNormals( i_NumOriginalNormals )
//{
//	//DBG_ASSERT( i_Fragments.size() == m_SplitRemapping.size(), "A Remapping is needed for multiple fragments." );
//
//	// Create root node for our fragments so that we can control the visibility
//	m_pNode = new g3dSceneNode();
//	m_pNode->SetName( i_MeshName.c_str() );
//	
//	// Add fragments into small hierarchy under the root node
//	//DBG_LOG("MeshSkinGroup: Num fragments: " << m_Fragments.size());
//	for (int i=0; i<m_Fragments.size(); i++)
//	{
//		g3dSceneNode *node = new g3dSceneNode;
//		node->SetFragment( m_Fragments[i] );
//		m_pNode->AddChild(node);
//	}
//
//	//DBG_LOG("MeshSkinGroup: Num vertices in skin: " << m_nNumVertices);
//	for (int m=0; m<m_CharacterSkin.m_MorphTargets.size(); m++)
//	{
//		//DBG_LOG2(" MorphTarget (%d): Num verts: %d", m, m_CharacterSkin.m_MorphTargets[m].m_Offsets.size());
//	}
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMeshSkinGroup::~smdlMeshSkinGroup()
{
	// Fragments are owned by this object
	envSTLHelpers::DeleteContainer(m_Fragments);
}

//--------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//--------------------------------------------------------------------
g3dSceneNode* smdlMeshSkinGroup::RootNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlMeshSkinGroup::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlMeshSkinGroup::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//--------------------------------------------------------------------
//	TransformMesh - given the joint matrices and weights for the
//		influence of the morph targets, transform the
//		vertices based on the vertex influences.
//--------------------------------------------------------------------
void smdlMeshSkinGroup::TransformMesh( const maMatrix4x4* i_BoneMatrices,
									   std::vector<float> i_Weights )
{	
	// Confirm that we have skinning information before doing jointed animation.
	// If we don't have skinning information, then we are probably expecting
	// a vertex animation later.
	if (m_SplitRemapping.empty())
		return;
	// We need either skinning info or morph targets. 
	if (m_CharacterSkin.m_BoneVertices.empty() && m_CharacterSkin.m_MorphTargets.empty())
		return;

	// Resize vertices if needed
	if ( m_XformedVerts.size() < m_nNumVertices )
	{
		m_XformedVerts.resize( m_nNumVertices );
		m_XformedNorms.resize( m_nNumVertices );
	}

	// Tranform the vertices
	if (m_CharacterSkin.m_BoneVertices.empty())
	{
		// No skinning info, just do morph targets
		for(int i = 0; i < m_nNumVertices; ++i )
		{
			// transform vertex based on i_Weights gathered above
			m_XformedVerts[i] = morph_vertex(m_BasePositions[i], 
											   m_CharacterSkin.m_MorphTargets, 
											   i_Weights, 
											   i);
			m_XformedNorms[i] = m_BaseNormals[i];
		}
	}
	else
	{
		// The bind pose code is inefficient. We could be multiplying matrices
		// before applying them to the vertices for instance, or pre-transforming
		// the m_BoneVertices. But, none of that really matters if the artists just
		// freeze the transforms before skinning, resulting in a bind pose of
		// identity, and skipping all of that code.
		// We could also check in the constructor for IsIdentity() and do the 
		// Invert() once in the constructor also.
		maMatrix4x4 inv_bind_pose(m_CharacterSkin.m_BindPose);
		bool bHasBindPose = (!m_CharacterSkin.m_BindPose.IsIdentity());
		if (bHasBindPose)
		{
			inv_bind_pose.Invert();
		}

		// Have skinning info, with possible morph targets also
		for (int i = 0; i < m_nNumVertices; ++i )
		{
			const smdlBoneVertex& bone_vertex = m_CharacterSkin.m_BoneVertices[i];

			// transform vertex based on i_Weights gathered above
			maVector3d base_pos = morph_vertex(m_BasePositions[i], 
											   m_CharacterSkin.m_MorphTargets, 
											   i_Weights, 
											   i);

			maPoint3d &vert = m_XformedVerts[i];
			maPoint3d &normal = m_XformedNorms[i];
			vert.Set( 0.0f, 0.0f, 0.0f );

			int nLargestInfluenceIndex = 0;

			// Calculate each influence on the vertex
			int nInfluences = bone_vertex.m_Influences.size();
			if (nInfluences == 0)
			{
				vert = base_pos;
				normal = m_BaseNormals[i];
			}
			else
			{
				for(int j = 0; j < nInfluences; ++j )
				{
					const scBoneInfluence& influence = bone_vertex.m_Influences[j];
					const maMatrix4x4& bone_matrix = i_BoneMatrices[ influence.m_BoneIndex ];

					maVector3d influenced_pos = base_pos;
					if (bHasBindPose)
						m_CharacterSkin.m_BindPose.Transform( influenced_pos );
					bone_matrix.Transform( influenced_pos );
					if (bHasBindPose)
						inv_bind_pose.Transform( influenced_pos );
					influenced_pos *= influence.m_fWeight;

					vert += influenced_pos;

					// Find the largest influence on the vertex to update the normal
					if( influence.m_fWeight > bone_vertex.m_Influences[ nLargestInfluenceIndex ].m_fWeight )
					{
						nLargestInfluenceIndex = j;
					}
				}

				if (nLargestInfluenceIndex < nInfluences)
				{
					// Update the normal with the transform of the greatest influence
					const maMatrix4x4& transform = i_BoneMatrices[
															bone_vertex.m_Influences[
															nLargestInfluenceIndex ].m_BoneIndex ];

					//bga - Do we need to consider the morph targets here where
					// we are using the bone_vertex.m_Normal ?	
					if (bHasBindPose)
					{
						normal = transform_normal( m_CharacterSkin.m_BindPose, m_BaseNormals[i] );
						normal = transform_normal( transform, normal );
						normal = transform_normal( inv_bind_pose, normal );
					}
					else
						normal = transform_normal( transform, m_BaseNormals[i] );
				}

				// Note: we really could be transforming the S,T and SxT parts of the
				// BumpTex1 vertex when animating. They are being recomputed down below.
			}
		}
	}

	// Now map the transformed vertices and normals into the multiple
	// split fragments
	int num_fragments = m_Fragments.size();
	for (int i=0; i<num_fragments; i++)
	{
		g3dFragment *fragment = m_Fragments[i];
		// We just use the vertex remapping with skinning, not the normal remapping
		const RemapArray &remap = m_SplitRemapping[i];

		int num_frag_verts = fragment->GetNumVertices();
		DBG_ASSERT( remap.size() == num_frag_verts, "Number of remapping vertices is incorrect" );
		if (remap.size() != num_frag_verts)
			continue;

		g3dType::VertexFormat vformat = fragment->GetVertexFormat();
		bool bump_frag = (vformat == g3dType::e_BumpTex1Vertex);
		if (!bump_frag)
		{	
			DBG_ASSERT( vformat == g3dType::e_Tex1Vertex, "Only g3dType::BumpTex1Vertex or g3dType::Tex1Vertex format implemented" ); 
			if (vformat != g3dType::e_BumpLitTex1Vertex)
				continue;
		}
		// Lock the vertex buffer
		unsigned char *pVertexBuffer = this->LockFragment(fragment);
		maAxisBox bounding_box;
		const int *remap_ptr = &remap[0];
		if (bump_frag)
		{
			g3dType::BumpTex1Vertex* pBumpVertices = reinterpret_cast<g3dType::BumpTex1Vertex*>( pVertexBuffer );
			g3dType::BumpTex1Vertex* pCurBumpVert = pBumpVertices;
			for (int j=0; j<num_frag_verts; ++j, ++remap_ptr, ++pCurBumpVert)
			{
				pCurBumpVert->m_Vertex = m_XformedVerts[ *remap_ptr ];
				pCurBumpVert->m_Normal = m_XformedNorms[ *remap_ptr ];
				bounding_box.Union( pCurBumpVert->m_Vertex );
			}
			
			// Update basis vectors
			const std::vector<envType::UInt32> &indicesOfThisFragment =  m_FragmentIndexVecs[i];
			create_basis_vectors(pBumpVertices, num_frag_verts, &indicesOfThisFragment[0], indicesOfThisFragment.size() );
		}
		else
		{
			g3dType::Tex1Vertex* pCurTex1Vert = reinterpret_cast<g3dType::Tex1Vertex*>( pVertexBuffer );
			for (int j=0; j<num_frag_verts; ++j, ++remap_ptr, ++pCurTex1Vert)
			{
				pCurTex1Vert->m_Vertex = m_XformedVerts[ *remap_ptr ];
				pCurTex1Vert->m_Normal = m_XformedNorms[ *remap_ptr ];
				bounding_box.Union( pCurTex1Vert->m_Vertex );
			}
		}

		fragment->SetBoundingBox( bounding_box );

		// Use the smdlSurface function to call Unlock in order
		// to unify the thread locking behavior
		this->UnlockFragment(fragment);
	}

}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
bool smdlMeshSkinGroup::CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const
{
	int num_verts = 0, num_normals = 0;
	i_VertKeys->GetNumVertices(num_verts, num_normals);
	if (num_verts != m_NumOriginalVertices)
	{
		DBG_WARNING("Mesh " << this->m_MeshName.c_str() << ", number of vertices " << m_NumOriginalVertices << " does not match animation " << num_verts);
		return false;
	}
	if (num_normals != m_NumOriginalNormals)
	{
		DBG_WARNING("Mesh " << this->m_MeshName.c_str() << ", number of normals " << m_NumOriginalNormals << " does not match animation %d" << num_normals );
		return false;
	}
	return true;
}

//--------------------------------------------------------------------
//	VertexTransformMesh - use baked vertex animation to transform
//		the meshes in this group.
//--------------------------------------------------------------------
void smdlMeshSkinGroup::VertexTransformMesh(float i_CurrentFrame,
											const smdlVertexAnimKeys &i_VertKeys)
{
	// Confirm that we have normal remapping before doing vertex animation.
	// If we don't have normal remapping, then the mesh was probably not
	// exported with the flag that signified that it would be vertex animated.
	//DBG_ASSERT(m_NormalRemapping.size() == m_Fragments.size(), "Don't have normal remapping, %d out of %d", m_Fragments.size(), m_NormalRemapping.size());
	if (m_FullVertexRemapping.empty() || m_FullNormalRemapping.empty())
		return;

	// Get frames to interpolate between. Should we clamp to one frame or the other?
	vtxVertexFrame *pFrame0 = NULL, *pFrame1 = NULL;
	float alpha = 0.0f;
	i_VertKeys->GetBracketingFrames(i_CurrentFrame, pFrame0, pFrame1, alpha);
	if (!pFrame0)
		return;
	float inv_alpha = (1 - alpha);

	// Now map the transformed vertices and normals into the multiple
	// split fragments
	const int num_fragments = m_Fragments.size();
	for (int i=0; i<num_fragments; i++)
	{
		g3dFragment *fragment = m_Fragments[i];
		if (!fragment) continue;

		const RemapArray &vremap = m_FullVertexRemapping[i];
		const RemapArray &nremap = m_FullNormalRemapping[i];

		int num_frag_verts = fragment->GetNumVertices();
		DBG_ASSERT( vremap.size() == fragment->GetNumVertices(), "Number of remapping vertices is incorrect" );
		if (vremap.size() != fragment->GetNumVertices())
			continue;
		DBG_ASSERT( vremap.size() == nremap.size(), "Number of remapping normals is incorrect" );
		if (vremap.size() != nremap.size())
			continue;

		g3dType::VertexFormat vformat = fragment->GetVertexFormat();
		bool bump_frag = (vformat == g3dType::e_BumpTex1Vertex);
		if (!bump_frag)
		{	
			DBG_ASSERT( vformat == g3dType::e_Tex1Vertex, "Only g3dType::BumpTex1Vertex or g3dType::Tex1Vertex format implemented" ); 
			if (vformat != g3dType::e_BumpLitTex1Vertex)
				continue;
		}
		// Lock the vertex buffer
		unsigned char *pVertexBuffer = this->LockFragment(fragment);

		maAxisBox bounding_box;
		const int *vremap_ptr = &vremap[0];
		const int *nremap_ptr = &nremap[0];
		if (bump_frag)
		{
			g3dType::BumpTex1Vertex* pBumpVertices = reinterpret_cast<g3dType::BumpTex1Vertex*>( pVertexBuffer );
			g3dType::BumpTex1Vertex* pCurBumpVert = pBumpVertices;			
			if (pFrame1)
			{
				for (int j=0; j<num_frag_verts; ++j, ++pCurBumpVert, ++vremap_ptr, ++nremap_ptr)
				{
					pCurBumpVert->m_Vertex = alpha * pFrame1->m_Positions[ *vremap_ptr ] + inv_alpha * pFrame0->m_Positions[ *vremap_ptr ];
					pCurBumpVert->m_Normal = alpha * pFrame1->m_Normals[ *nremap_ptr ] + inv_alpha * pFrame0->m_Normals[ *nremap_ptr ];
					pCurBumpVert->m_Normal.Normalize();
					bounding_box.Union( pCurBumpVert->m_Vertex );
				}
			}
			else
			{
				for (int j=0; j<num_frag_verts; ++j, ++pCurBumpVert, ++vremap_ptr, ++nremap_ptr)
				{
					pCurBumpVert->m_Vertex = pFrame0->m_Positions[ *vremap_ptr ];
					pCurBumpVert->m_Normal = pFrame0->m_Normals[ *nremap_ptr ];
					bounding_box.Union( pCurBumpVert->m_Vertex );
				}
			}
			// Update basis vectors
			const std::vector<envType::UInt32> &indicesOfThisFragment =  m_FragmentIndexVecs[i];
			create_basis_vectors(pBumpVertices, num_frag_verts, &indicesOfThisFragment[0], indicesOfThisFragment.size() );
		}
		else
		{
			g3dType::Tex1Vertex* pCurTex1Vert = reinterpret_cast<g3dType::Tex1Vertex*>( pVertexBuffer );
			if (pFrame1)
			{
				for (int j=0; j<num_frag_verts; ++j, ++pCurTex1Vert, ++vremap_ptr, ++nremap_ptr)
				{
					pCurTex1Vert->m_Vertex = alpha * pFrame1->m_Positions[ *vremap_ptr ] + inv_alpha * pFrame0->m_Positions[ *vremap_ptr ];
					pCurTex1Vert->m_Normal = alpha * pFrame1->m_Normals[ *nremap_ptr ] + inv_alpha * pFrame0->m_Normals[ *nremap_ptr ];
					pCurTex1Vert->m_Normal.Normalize();
					bounding_box.Union( pCurTex1Vert->m_Vertex );
				}
			}
			else
			{
				for (int j=0; j<num_frag_verts; ++j, ++pCurTex1Vert, ++vremap_ptr, ++nremap_ptr)
				{
					pCurTex1Vert->m_Vertex = pFrame0->m_Positions[ *vremap_ptr ];
					pCurTex1Vert->m_Normal = pFrame0->m_Normals[ *nremap_ptr ];
					bounding_box.Union( pCurTex1Vert->m_Vertex );
				}
			}
		}

		fragment->SetBoundingBox( bounding_box );

		// Use the smdlSurface function to call Unlock in order
		// to unify the thread locking behavior
		this->UnlockFragment(fragment);
	}
}

//--------------------------------------------------------------------
// Animation data is giving us the bounding box for the surface 
//	before the actual animation is done. Set this bounding box 
//	into the fragment.
//--------------------------------------------------------------------
void smdlMeshSkinGroup::BBoxTransform(const maAxisBox &i_BBox)
{
	// Don't have separate bounding boxes for each portion of the
	// surface, so set the same bbox for all fragments.
	const int num_fragments = m_Fragments.size();
	for (int i=0; i<num_fragments; i++)
	{
		m_Fragments[i]->SetBoundingBox( i_BBox );
	}
}


//--------------------------------------------------------------------
// Access to mesh group name
//--------------------------------------------------------------------
std::string smdlMeshSkinGroup::GetMeshGroupName()
{
	return m_MeshName;
}

//--------------------------------------------------------------------
// Access to vertices
//--------------------------------------------------------------------
std::vector<maPoint3d>* smdlMeshSkinGroup::GetVertices()
{	
	return &m_BasePositions;	
}

//--------------------------------------------------------------------
// Access to normals
//--------------------------------------------------------------------
std::vector<maVector3d>* smdlMeshSkinGroup::GetNormals()
{
	return &m_BaseNormals;
}

//--------------------------------------------------------------------
// Access to indices
//--------------------------------------------------------------------
std::vector<envType::UInt32>* smdlMeshSkinGroup::GetIndices()
{	
	return &(m_FragmentIndexVecs[0]);	
}

//--------------------------------------------------------------------
// Access to fragments
//--------------------------------------------------------------------
std::vector<g3dFragment*>& smdlMeshSkinGroup::GetFragments()
{	
	return m_Fragments;	
}

//--------------------------------------------------------------------
// Access to node
//--------------------------------------------------------------------
g3dSceneNode* smdlMeshSkinGroup::GetNode()
{	
	return m_pNode;	
}