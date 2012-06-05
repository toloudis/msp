/*****************************************************************************
**	smdlSubdivSurface.hpp
**
**		smdlSubdivSurface handles a subdivision surface that animates. 
**	It handles skinning, morph target animation and direct vertex animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dVertexBuffer.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"
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
}

//--------------------------------------------------------------------
//	smdlSubdivSurface requires a subdivision network and the 
//		character skin info with information about how it animates.
//--------------------------------------------------------------------
smdlSubdivSurface::smdlSubdivSurface(	shared_ptr<mdlSubdivInfo> i_pSubdivInfo,
					shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork,
					const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
					const smdlCharacterSkin& i_CharacterSkin )
:	m_CharacterSkin( i_CharacterSkin ),
	m_pSubdivInfo( i_pSubdivInfo ),
	m_pSubdivNetwork( i_pSubdivNetwork ),
	m_BasePositions( i_pSubdivInfo->GetVertices() ),
	m_SubdivName(i_pSubdivInfo->GetName() ),
	m_NumOriginalVertices(i_pSubdivInfo->GetNumOrigVertices()),
	m_pSharedVertexBuffer(NULL)
{
	// In order to do vertex animation, need to have remapping
	mdlFragUtil::ConvertRemapping(i_pSubdivInfo->GetVertexRemap(), m_VertexRemapping, i_pSubdivInfo->GetVertices().size());

	// Create multiple fragments that share a single vertex buffer,
	// one fragment per material
	const bool morphable = true;
	m_pSharedVertexBuffer = mdlFragCreate::CreateFragmentGroup(m_pSubdivNetwork->GetSubdivFragInfo(),
									 m_Fragments,
									 morphable);

	// Create a root node for our surface so that we can control the visibility
	m_pNode = new g3dSceneNode();
	m_pNode->SetName( m_SubdivName.c_str() );

	const int num_fragments = m_Fragments.size();
	for (int fi=0; fi<num_fragments; fi++)
	{
		g3dFragment *pFragment = m_Fragments[fi];
		pFragment->SetHardwareTesselate( false );

		// Look for material override for this instance
		matMaterial *pMaterial = i_pSubdivInfo->GetMaterials()[fi]->m_pMaterial;
		std::map<const matMaterial*,matMaterial*>::const_iterator it = i_MaterialRemapping.find(pMaterial);
		if (it != i_MaterialRemapping.end())
		{
			// Allow custom materials using shared networks
			if (it->second)
				pFragment->SetMaterial(it->second);
		}

		// If a single fragment, put it in the root node.
		// Otherwise, make a little subtree under the root node.
		if (num_fragments == 1)
			m_pNode->SetFragment( pFragment );
		else
		{
			g3dSceneNode *pChildNode = new g3dSceneNode(pFragment);
			m_pNode->AddChild(pChildNode);
		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
smdlSubdivSurface::~smdlSubdivSurface()
{
	// Fragments and shared vertex buffer are owned by this object
	envSTLHelpers::DeleteContainer(m_Fragments);
	delete m_pSharedVertexBuffer;
}

//----------------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//----------------------------------------------------------------------------
g3dSceneNode* smdlSubdivSurface::RootNode()
{
	return m_pNode;
}

//----------------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//----------------------------------------------------------------------------
//virtual 
void smdlSubdivSurface::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlSubdivSurface::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//----------------------------------------------------------------------------
// Set the current subdivision level for the surface.
//----------------------------------------------------------------------------
void smdlSubdivSurface::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	if (i_SubdivLevel != m_pSubdivNetwork->GetCurrentSubdivLevel())
	{
		// Set the subdiv level in the subdiv network
		m_pSubdivNetwork->SetCurrentSubdivLevel(i_SubdivLevel);
	}

	// Since subdiv networks are shared, it is possible that another character
	// changed the subdiv level in the shared network. So the first case
	// might be false (because the shared network has been subdivided), but our
	// fragment is the wrong size. So, we need to resize the fragment now.
	if (m_pSharedVertexBuffer->GetNumVertices() != m_pSubdivNetwork->GetNumSubdivVertices())
	{
		// Update the fragment buffers without needing to delete and 
		//	recreate the fragments
		const bool morphable = true;
		mdlFragCreate::UpdateFragmentGroup(m_pSubdivNetwork->GetSubdivFragInfo(), 
										   *m_pSharedVertexBuffer, 
										   m_Fragments,
										   morphable);

		//// Delete the old fragment
		//delete m_pFragment;

		//// Create a new fragment at the new level
		//const bool morphable = true;
		//m_pFragment = mdlFragCreate::CreateFragment(m_pSubdivNetwork->GetSubdivFragInfo(), morphable);
		//// Allow custom materials using shared networks
		//if (m_pMaterial)
		//	m_pFragment->SetMaterial(m_pMaterial);

		//// Replace the fragment in the scene node
		//m_pNode->SetFragment( m_pFragment );

		//// Set hardware tesselation boolean (could be set to more detail later)
		//// true only if we are at the base mesh level.
		//// Global render settings should turn on/off hardware tesselation
		//// and set the global tesellation level and technique.
		//m_pFragment->SetHardwareTesselate( false );
	}
}

//----------------------------------------------------------------------------
//	TransformSubdiv - given the joint matrices and weights for the
//		influence of the morph targets, transform the
//		base mesh based on the vertex influences. Then
//		propagate the base mesh changes through to the current
//		subdivision level.
//----------------------------------------------------------------------------
void smdlSubdivSurface::TransformSubdiv( const maMatrix4x4* i_BoneMatrices,
									     const std::vector<float>& i_Weights )
{	
	// Confirm that we have skinning information before doing jointed animation.
	// If we don't have skinning information, then we are probably expecting
	// a vertex animation later.
	if (m_CharacterSkin.m_BoneVertices.empty() && m_CharacterSkin.m_MorphTargets.empty())
		return;

	int numVerts = m_BasePositions.size();
	if (m_VertCache.size() < numVerts)
	{
		m_VertCache.resize( numVerts );
	}

	if (m_CharacterSkin.m_BoneVertices.empty())
	{
		// No skinning info, just do morph targets
		for (int i = 0; i < numVerts; ++i )
		{
			// transform vertex based on i_Weights gathered above
			m_VertCache[i] = morph_vertex(m_BasePositions[i], 
											   m_CharacterSkin.m_MorphTargets, 
											   i_Weights, 
											   i);
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

		// Tranform the vertices into our local CachedVerts array
		int i, j;
		for ( i = 0; i < numVerts; ++i )
		{
			const smdlBoneVertex& bone_vertex = m_CharacterSkin.m_BoneVertices[i];

			maPoint3d &vert = m_VertCache[i];
			//maPoint3d &normal = (bump_frag) ? bump_vertices[i].m_Normal : 
			//	((hasTextureCoords) ? tex1_vertices[i].m_Normal : tex0_vertices[i].m_Normal);

			// transform vertex based on i_Weights gathered above
			maVector3d base_pos = morph_vertex(m_BasePositions[i], m_CharacterSkin.m_MorphTargets, i_Weights, i);
			// Calculate each influence on the vertex
			int nInfluences = bone_vertex.m_Influences.size();
			if (nInfluences == 0)
			{
				vert = base_pos;
			}
			else
			{
				vert.Set( 0.0f, 0.0f, 0.0f );
				//DBG_ASSERT(nInfluences > 0, "Need influences for vertices for mesh to be skinned.");
				for ( j = 0; j < nInfluences; ++j )
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
				}
			}

			// Update the normal with the transform of the greatest influence
			//const maMatrix4x4& transform = i_BoneMatrices[
			//										bone_vertex.m_Influences[
			//										nLargestInfluenceIndex ].m_BoneIndex ];
			//
			//normal = transform_normal( transform, bone_vertex.m_Normal );
		}
	}

	// Subdiv networks can be shared now, so we need to make sure
	// that we aren't using the same network for two characters
	// at the same time
	{
		envScopedLock subdiv_network_lock(m_pSubdivNetwork->GetMutex());

		// Now, give these new vertices to the subdivision network in order
		// to update the subdivided model.
		m_pSubdivNetwork->AlterBaseMesh(&(m_VertCache[0]), numVerts);

		// Update our fragment with the new subdiv position
		this->update_fragment();
	}
}

//----------------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//----------------------------------------------------------------------------
bool smdlSubdivSurface::CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const
{
	//const int num_remap = this->m_VertexRemapping.size();
	//int max_remap = 0;
	//for (int i=0; i<num_remap; i++)
	//{
	//	if (m_VertexRemapping[i] > max_remap)
	//		max_remap = m_VertexRemapping[i];
	//}
	//DBG_LOG3("Subdiv %s Comparing org %d versus max_remap %d", m_SubdivName.c_str(), m_NumOriginalVertices, max_remap);

	int num_verts = 0, num_normals = 0;
	i_VertKeys->GetNumVertices(num_verts, num_normals);
	if (num_verts != m_NumOriginalVertices)
	{
		DBG_WARNING("Subdiv " << this->m_SubdivName.c_str() << ", number of vertices " << m_NumOriginalVertices << " does not match animation " << num_verts );
		return false;
	}
	// Subdivs don't use the normal info. No need to check that.

	return true;
}

//----------------------------------------------------------------------------
//	VertexTransformSubdiv - use baked vertex animation to transform
//		the base mesh of the subdivision surface.
//----------------------------------------------------------------------------
void smdlSubdivSurface::VertexTransformSubdiv(float i_CurrentFrame,
											  const smdlVertexAnimKeys &i_VertKeys)
{
	// Confirm that we have vertex remapping before doing baked vertex animation.
	if (m_VertexRemapping.empty())
		return;

	// Get frames to interpolate between. Should we clamp to one frame or the other?
	vtxVertexFrame *pFrame0 = NULL, *pFrame1 = NULL;
	float alpha = 0.0f;
	i_VertKeys->GetBracketingFrames(i_CurrentFrame, pFrame0, pFrame1, alpha);
	if (!pFrame0)
		return;
	float inv_alpha = (1 - alpha);

	int numVerts = m_pSubdivNetwork->GetNumBaseMeshVertices();
	if (m_VertCache.size() < numVerts)
	{
		m_VertCache.resize( numVerts );
	}

	if (pFrame1)
	{
		for (int j=0; j<numVerts; j++)
		{
			m_VertCache[j] = alpha * pFrame1->m_Positions[ m_VertexRemapping[j] ] + inv_alpha * pFrame0->m_Positions[ m_VertexRemapping[j] ];
		}
	}
	else
	{
		for (int j=0; j<numVerts; j++)
		{
			m_VertCache[j] = pFrame0->m_Positions[ m_VertexRemapping[j] ];
		}
	}

	// Subdiv networks can be shared now, so we need to make sure
	// that we aren't using the same network for two characters
	// at the same time
	{
		envScopedLock subdiv_network_lock(m_pSubdivNetwork->GetMutex());

		// Now, give these new vertices to the subdivision network in order
		// to update the subdivided model.
		m_pSubdivNetwork->AlterBaseMesh(&(m_VertCache[0]), numVerts);

		// Update our fragment with the new subdiv position
		this->update_fragment();
	}
}

//----------------------------------------------------------------------------
// Animation data is giving us the bounding box for the surface 
//	before the actual animation is done. Set this bounding box 
//	into the fragment.
//----------------------------------------------------------------------------
void smdlSubdivSurface::BBoxTransform(const maAxisBox &i_BBox)
{
	for (int fi=0; fi<m_Fragments.size(); fi++)
		m_Fragments[fi]->SetBoundingBox( i_BBox );
}

//----------------------------------------------------------------------------
// Update our fragment from the current state of the base mesh
//	in the subdivision network. Used in animation.
//----------------------------------------------------------------------------
void smdlSubdivSurface::update_fragment()
{
	// Get results from subdiv network
	int num_subdiv_verts = m_pSubdivNetwork->GetNumSubdivVertices();
	const smdlSubdivUtil::sVert* pSubdivVerts = m_pSubdivNetwork->GetSubdivVertices();

	// Now alter vertices within fragment itself
	//
	DBG_ASSERT(m_pSharedVertexBuffer->GetNumVertices() == num_subdiv_verts, "Fragment and m_pSubdivNetwork don't share same number of vertices?");
	if (m_pSharedVertexBuffer->GetNumVertices() != num_subdiv_verts)
		return;

	bool hasTextureCoords = true;
	g3dType::VertexFormat vformat = m_pSharedVertexBuffer->GetVertexFormat();
	bool bump_frag = (vformat == g3dType::e_BumpTex1Vertex);
	if (!bump_frag)
	{	
		if (vformat == g3dType::e_NonTexVertex)
			hasTextureCoords = false;
		else
		{
			DBG_ASSERT( vformat == g3dType::e_Tex1Vertex, "Only g3dType::BumpTex1Vertex, g3dType::Tex1Vertex, or g3dType::NonTexVertex format implemented" ); 
			if (vformat != g3dType::e_Tex1Vertex)
				return;
		}
	}

	// Lock the vertex buffer
	// Use the smdlSurface function to call Lock in order
	// to unify the thread locking behavior
	unsigned char *pVertexBuffer = this->LockVertexBuffer(m_pSharedVertexBuffer);

	// Transform the vertices
	maAxisBox bounding_box;
	const smdlSubdivUtil::sVert* pCurSubdivVert = pSubdivVerts;
	if (bump_frag)
	{
		g3dType::BumpTex1Vertex* pCurBumpVert = reinterpret_cast<g3dType::BumpTex1Vertex*>( pVertexBuffer );
		for (int i = 0; i < num_subdiv_verts; ++i, ++pCurSubdivVert, ++pCurBumpVert )
		{
			pCurBumpVert->m_Vertex = pCurSubdivVert->m_Loc;
/*			if ( m_pSubdivNetwork->GetCurrentSubdivLevel() == 1 && (i == 991	 || i == 5976 || i == 3061 || i == 5975)) //flip normal to debug bad polygin in krawk model
			{
				pCurBumpVert->m_Normal = -pCurSubdivVert->m_Norm;
			}
			else*/ pCurBumpVert->m_Normal = pCurSubdivVert->m_Norm;
			pCurBumpVert->m_S = pCurSubdivVert->m_S;
			pCurBumpVert->m_T = pCurSubdivVert->m_T;
			bounding_box.Union( pCurBumpVert->m_Vertex );
		}
	}
	else if (hasTextureCoords)
	{
		g3dType::Tex1Vertex* pCurTex1Vert = reinterpret_cast<g3dType::Tex1Vertex*>( pVertexBuffer );
		for (int i = 0; i < num_subdiv_verts; ++i, ++pCurSubdivVert, ++pCurTex1Vert )
		{
			pCurTex1Vert->m_Vertex = pCurSubdivVert->m_Loc;
			pCurTex1Vert->m_Normal = pCurSubdivVert->m_Norm;
			bounding_box.Union( pCurTex1Vert->m_Vertex );
		}

	}
	else 
	{
		g3dType::NonTexVertex* pCurTex0Vert = reinterpret_cast<g3dType::NonTexVertex*>( pVertexBuffer );
		for (int i = 0; i < num_subdiv_verts; ++i, ++pCurSubdivVert, ++pCurTex0Vert )
		{
			pCurTex0Vert->m_Vertex = pCurSubdivVert->m_Loc;
			pCurTex0Vert->m_Normal = pCurSubdivVert->m_Norm;
			bounding_box.Union( pCurTex0Vert->m_Vertex );
		}
	}

	m_pSharedVertexBuffer->SetBoundingBox( bounding_box );

	// Use the smdlSurface function to call Unlock in order
	// to unify the thread locking behavior
	this->UnlockVertexBuffer(m_pSharedVertexBuffer);

	// Now update the bounding box on all fragments.
	// It might be better to figure out exact bounding box for each fragment,
	// using its index list, but there is also the sense that this is a "single surface"
	// in the original modelling package, so it can still be treated as a single
	// renderable entitiy with the same extents.
	for (int fi=0; fi<m_Fragments.size(); fi++)
		m_Fragments[fi]->SetBoundingBox( bounding_box );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlSubdivSurface::GetNumFacesAtLevel( int i_SubdivLevel ) const
{
	return GetSubdivNetwork()->GetNumFacesAtLevel( i_SubdivLevel );
}


//--------------------------------------------------------------------
// Access to vertices
//--------------------------------------------------------------------
std::vector<maPoint3d>* smdlSubdivSurface::GetVertices()
{	
	if ( !m_VertCache.empty() )
	{	
		return &m_VertCache;
	}
	else
	{
		return &m_BasePositions;
	}
}

//--------------------------------------------------------------------
// Access to fragment
//--------------------------------------------------------------------
std::vector<g3dFragment*>& smdlSubdivSurface::GetFragments()
{
	return m_Fragments;
}