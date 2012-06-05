/*****************************************************************************
**	smdlCCPNSurface.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlCCPNSurface.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/Dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"


//============================================================================
//============================================================================
namespace
{
	bool l_bHardwareTesselate = false;

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
			DBG_ASSERT0(nWeights == i_MorphTargets.size(), "Weights array doesn't match number of morph targets");
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
// Get or set whether this surface type should use 
//	hardware tesselation
//--------------------------------------------------------------------
//static 
void smdlCCPNSurface::SetHardwareTesselate(bool i_bHardwareTesselate)
{
	l_bHardwareTesselate = i_bHardwareTesselate;
}
//static 
bool smdlCCPNSurface::GetHardwareTesselate()
{
	return l_bHardwareTesselate;
}

//--------------------------------------------------------------------
//	smdlCCPNSurface requires a subdivision network and the 
//		character skin info with information about how it animates.
//--------------------------------------------------------------------
smdlCCPNSurface::smdlCCPNSurface(	const mdlSubdivInfo& i_SubdivInfo,
								shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork,
								matMaterial* i_pMaterial,
								const smdlCharacterSkin& i_CharacterSkin )
:	m_CharacterSkin( i_CharacterSkin ),
	m_PNTessellator( i_pSubdivNetwork ),
//	m_BasePositions( i_SubdivInfo.m_Vertices ),
	m_SubdivName(i_SubdivInfo.m_Name),
	m_pMaterial(i_pMaterial),
	m_NumOriginalVertices(i_SubdivInfo.m_NumOrigVertices),
	m_nCurrentSubdivLevel( 0 )
{
	i_pSubdivNetwork->SetCurrentSubdivLevel(1);
//	m_nCurrentSubdivLevel = i_pSubdivNetwork->GetCurrentSubdivLevel();

	//collect base normals
	const smdlSubdivUtil::sVert* V = i_pSubdivNetwork->GetSubdivVertices();
//	int N = i_pSubdivNetwork->GetNumSubdivVertices();
	int N = i_pSubdivNetwork->GetNumBaseMeshVertices();
	m_BasePositions.resize( N );
	m_BaseNormals.resize( N );
	for( int i = 0; i < N; i++, V++ )
	{
		m_BasePositions[ i ] = V->m_Loc;
		m_BaseNormals[ i ] = V->m_Norm;
	}
	i_pSubdivNetwork->SetCurrentSubdivLevel(0);

	// In order to do vertex animation, need to have remapping
	mdlFragUtil::ConvertRemapping(i_SubdivInfo.m_VertexRemap, m_VertexRemapping, i_SubdivInfo.m_Vertices.size());

	const bool morphable = true;
	m_pFragment = mdlFragCreate::CreateFragment(m_PNTessellator.GetSubdivFragInfo(), morphable);
	// Allow custom materials using shared networks
	if (m_pMaterial)
		m_pFragment->SetMaterial(m_pMaterial);

	// Create a node for our fragment so that we can control the visibility
	m_pNode = new g3dSceneNode( m_pFragment );
	m_pNode->SetName( m_SubdivName.c_str() );

	// Set hardware tesselation boolean (could be set to more detail later)
	// true only if we are at the base mesh level.
	// Global render settings should turn on/off hardware tesselation
	// and set the global tesellation level and technique.
	m_pFragment->SetHardwareTesselate( l_bHardwareTesselate );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlCCPNSurface::~smdlCCPNSurface()
{
	// Fragment and subdiv network are owned by this object
	delete m_pFragment;
}

//--------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//--------------------------------------------------------------------
g3dSceneNode* smdlCCPNSurface::RootNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlCCPNSurface::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlCCPNSurface::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//--------------------------------------------------------------------
// Set the current subdivision level for the surface.
//--------------------------------------------------------------------
void smdlCCPNSurface::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	m_nCurrentSubdivLevel = i_SubdivLevel;
	if ( l_bHardwareTesselate )
	{
		m_PNTessellator.SetCurrentSubdivLevel( 0 );	//force to base mesh
	}
	else if (i_SubdivLevel != m_PNTessellator.GetCurrentSubdivLevel())
	{
		// Set the subdiv level in the subdiv network
		m_PNTessellator.SetCurrentSubdivLevel(i_SubdivLevel);
	}

	// Since subdiv networks are shared, it is possible that another character
	// changed the subdiv level in the shared network. So the first case
	// might be false (because the shared network has been subdivided), but our
	// fragment is the wrong size. So, we need to resize the fragment now.
//	if (m_pFragment->GetNumVertices() != m_PNTessellator.GetNumTessellatedVertices())
	{
		// Delete the old fragment
		delete m_pFragment;

		// Create a new fragment at the new level
		const bool morphable = true;
		m_pFragment = mdlFragCreate::CreateFragment(m_PNTessellator.GetSubdivFragInfo(), morphable);
		// Allow custom materials using shared networks
		if (m_pMaterial)
			m_pFragment->SetMaterial(m_pMaterial);

		// Replace the fragment in the scene node
		m_pNode->SetFragment( m_pFragment );

		// Set hardware tesselation boolean (could be set to more detail later)
		// true only if we are at the base mesh level.
		// Global render settings should turn on/off hardware tesselation
		// and set the global tesellation level and technique.
//		bool bHardwareSubdivide = (i_SubdivLevel == 0);
		m_pFragment->SetHardwareTesselate( l_bHardwareTesselate );
	}
}

//--------------------------------------------------------------------
//	TransformSubdiv - given the joint matrices and weights for the
//		influence of the morph targets, transform the
//		base mesh based on the vertex influences. Then
//		propagate the base mesh changes through to the current
//		subdivision level.
//--------------------------------------------------------------------
void smdlCCPNSurface::TransformSubdiv( const maMatrix4x4* i_BoneMatrices,
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
		m_NormCache.clear();
		// No skinning info, just do morph targets
		for(int i = 0; i < numVerts; ++i )
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
		DBG_ASSERT( numVerts == m_BaseNormals.size(), "Normals don't match vertices.");
		m_NormCache.resize( numVerts );

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
		for( i = 0; i < numVerts; ++i )
		{
			const smdlBoneVertex& bone_vertex = m_CharacterSkin.m_BoneVertices[i];

			maPoint3d &vert = m_VertCache[i];

			int nLargestInfluenceIndex = 0;
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
				float maxWeight = -1;

				vert.Set( 0.0f, 0.0f, 0.0f );
				//DBG_ASSERT0(nInfluences > 0, "Need influences for vertices for mesh to be skinned.");
				for( j = 0; j < nInfluences; ++j )
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

					if ( influence.m_fWeight > maxWeight )
					{
						nLargestInfluenceIndex = j;
						maxWeight = influence.m_fWeight;
					}

					vert += influenced_pos;
				}
			}


			// Update the normal with the transform of the greatest influence
			const maMatrix4x4& transform = i_BoneMatrices[
													bone_vertex.m_Influences[
													nLargestInfluenceIndex ].m_BoneIndex ];
			
			m_NormCache[ i ] = transform_normal( transform, m_BaseNormals[ i ] );
		}
	}

	// Now, give these new vertices to the subdivision network in order
	// to update the subdivided model.
	m_PNTessellator.AlterBaseMesh(m_VertCache, m_NormCache);

	// Update our fragment with the new subdiv position
	update_fragment();
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
bool smdlCCPNSurface::CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const
{
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

//--------------------------------------------------------------------
//	VertexTransformSubdiv - use baked vertex animation to transform
//		the base mesh of the subdivision surface.
//--------------------------------------------------------------------
void smdlCCPNSurface::VertexTransformSubdiv(float i_CurrentFrame,
						   const smdlVertexAnimKeys &i_VertKeys)
{
	// Confirm that we have vertex remapping before doing baked vertex animation.
	if (m_VertexRemapping.empty())
		return;

	// Get frames to interpolate between. Should we clamp to one frame or the other?
	smdlVertexFrame *pFrame0 = NULL, *pFrame1 = NULL;
	float alpha = 0.0f;
	i_VertKeys->GetBracketingFrames(i_CurrentFrame, pFrame0, pFrame1, alpha);
	if (!pFrame0)
		return;
	float inv_alpha = (1 - alpha);

	int numVerts = m_PNTessellator.GetNumBaseMeshVertices();
	if (m_VertCache.size() < numVerts)
	{
		m_VertCache.resize( numVerts );
	}
//	DBG_ASSERT( numVerts == m_BaseNormals.size(), "Normals don't match vertices.");
//	m_NormCache.resize( numVerts );
	m_NormCache.clear();

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
//		envScopedLock subdiv_network_lock(m_PNTessellator.GetMutex());

		// Now, give these new vertices to the subdivision network in order
		// to update the subdivided model.
		m_PNTessellator.AlterBaseMesh( m_VertCache, m_NormCache );

		// Update our fragment with the new subdiv position
		update_fragment();
	}
}

//--------------------------------------------------------------------
// Animation data is giving us the bounding box for the surface 
//	before the actual animation is done. Set this bounding box 
//	into the fragment.
//--------------------------------------------------------------------
void smdlCCPNSurface::BBoxTransform(const maAxisBox &i_BBox)
{
	m_pFragment->SetBoundingBox( i_BBox );
}

//--------------------------------------------------------------------
// Update our fragment from the current state of the base mesh
//	in the subdivision network. Used in animation.
//--------------------------------------------------------------------
void smdlCCPNSurface::update_fragment()
{
	// Get results from subdiv network
	int num_tess_verts = m_PNTessellator.GetNumTessellatedVertices();
	const std::vector<tVert>& TessVerts = m_PNTessellator.GetTessellatedVertices();

	// Now alter vertices within fragment itself
	//
	DBG_ASSERT0(m_pFragment->GetNumVertices() == num_tess_verts, "Fragment and Tessellator don't share same number of vertices?");

	bool hasTextureCoords = true;
	g3dType::VertexFormat vformat = m_pFragment->GetVertexFormat();
	bool bump_frag = (vformat == g3dType::e_BumpTex1Vertex);
	if (!bump_frag)
	{	
		if (vformat == g3dType::e_NonTexVertex)
			hasTextureCoords = false;
		else
		{
			DBG_ASSERT0( vformat == g3dType::e_Tex1Vertex, "Only g3dType::BumpTex1Vertex, g3dType::Tex1Vertex, or g3dType::NonTexVertex format implemented" ); 
		}
	}
	// Lock the vertex buffer
	unsigned char *pVertexBuffer = m_pFragment->Lock();

	// Transform the vertices
	maAxisBox bounding_box;
	const tVert* pCurSubdivVert = &TessVerts[0];
	if (bump_frag)
	{
		g3dType::BumpTex1Vertex* pCurBumpVert = reinterpret_cast<g3dType::BumpTex1Vertex*>( pVertexBuffer );
		for (int i = 0; i < num_tess_verts; ++i, ++pCurSubdivVert, ++pCurBumpVert )
		{
			pCurBumpVert->m_Vertex = pCurSubdivVert->Position;
			pCurBumpVert->m_Normal = pCurSubdivVert->Normal;
			pCurBumpVert->m_S = pCurSubdivVert->Tangent;
			pCurBumpVert->m_T = pCurSubdivVert->Binormal;
			bounding_box.Union( pCurBumpVert->m_Vertex );
		}
	}
	else if (hasTextureCoords)
	{
		g3dType::Tex1Vertex* pCurTex1Vert = reinterpret_cast<g3dType::Tex1Vertex*>( pVertexBuffer );
		for (int i = 0; i < num_tess_verts; ++i, ++pCurSubdivVert, ++pCurTex1Vert )
		{
			pCurTex1Vert->m_Vertex = pCurSubdivVert->Position;
			pCurTex1Vert->m_Normal = pCurSubdivVert->Normal;
			bounding_box.Union( pCurTex1Vert->m_Vertex );
		}
	}
	else
	{
		g3dType::NonTexVertex* pCurTex0Vert = reinterpret_cast<g3dType::NonTexVertex*>( pVertexBuffer );
		for (int i = 0; i < num_tess_verts; ++i, ++pCurSubdivVert, ++pCurTex0Vert )
		{
			pCurTex0Vert->m_Vertex = pCurSubdivVert->Position;
			pCurTex0Vert->m_Normal = pCurSubdivVert->Normal;
			bounding_box.Union( pCurTex0Vert->m_Vertex );
		}
	}

	m_pFragment->SetBoundingBox( bounding_box );

	// Use the smdlSurface function to call Unlock in order
	// to unify the thread locking behavior
	UnlockFragment(m_pFragment);

	//temp set to wireframe
	// Use m_pNode->SetDrawStyle(g3dSceneNode::e_LitWireframe); instead
//	m_pFragment->SetDrawWireframe( true );

	if ( l_bHardwareTesselate )
	{
/*
		if ( m_nCurrentSubdivLevel > 0 && TSCheckTessellationSupport( g2dDX9Global::g_pD3D, g2dDX9Global::g_pDevice ) )
		{
			m_pFragment->SetHardwareTesselate( true );
			float hardwareTessVal = ((m_nCurrentSubdivLevel-1)<<1) + 1.0f;
			if ( m_nCurrentSubdivLevel >= 3 ) hardwareTessVal = 15.0f;
			m_pFragment->SetHardwareTesselateVal( hardwareTessVal );
		}
		else
*/
		{
			m_pFragment->SetHardwareTesselate( false );
			m_pFragment->SetHardwareTesselateVal( 0.0f );
		}
	}
}
