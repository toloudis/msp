/*****************************************************************************
**	smdlMeshSingleSkin.hpp
**
**		smdlMeshSingleSkin handles a single skinned polygon mesh
**	with no remapping.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlMeshSingleSkin.hpp"

#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/smdlBoneVertex.hpp"


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
}


//--------------------------------------------------------------------
//	smdlMeshSingleSkin requires the skinning and bone vertex data. 
//	Assumes ownership of the fragment. Bone vertices can be shared
//--------------------------------------------------------------------
smdlMeshSingleSkin::smdlMeshSingleSkin(	const mdlSplitFragInfo& i_SplitFragInfo,
										const std::vector<smdlBoneVertex>& i_BoneVertices,
										const maMatrix4x4& i_BindPose )
:	m_BoneVertices( i_BoneVertices ),
	m_nNumVertices( i_BoneVertices.size() ),
	m_BasePositions( i_SplitFragInfo.m_Vertices ),
	m_BaseNormals( i_SplitFragInfo.m_Normals ),
	m_BindPose( i_BindPose )
{
	// Create fragment
	m_pFragment =  mdlFragCreate::CreateFragment(i_SplitFragInfo, true);

	// Create a node for our fragment so that we can control the visibility
	m_pNode = new g3dSceneNode( m_pFragment );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMeshSingleSkin::~smdlMeshSingleSkin()
{
	// Since fragments are not shared for single-skin, we should delete ours
	delete m_pFragment;
}

//--------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//--------------------------------------------------------------------
g3dSceneNode* smdlMeshSingleSkin::RootNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlMeshSingleSkin::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlMeshSingleSkin::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//--------------------------------------------------------------------
//	TransformMesh - given the joint matrices, transform the
//		vertices based on the vertex influences.
//--------------------------------------------------------------------
void smdlMeshSingleSkin::TransformMesh( const maMatrix4x4* i_BoneMatrices )
{
	if (m_nNumVertices == 0) return;

	g3dType::VertexFormat vformat = m_pFragment->GetVertexFormat();
	bool bump_frag = (vformat == g3dType::e_BumpTex1Vertex);
	if (!bump_frag)
	{	
		DBG_ASSERT( vformat == g3dType::e_Tex1Vertex, "Only g3dType::BumpTex1Vertex or g3dType::Tex1Vertex format implemented" ); 
		if (vformat != g3dType::e_Tex1Vertex)
			return;
	}
	// Lock the vertex buffer
	unsigned char *pVertexBuffer = 	this->LockFragment(m_pFragment);
	g3dType::Tex1Vertex* tex1_vertices = reinterpret_cast<g3dType::Tex1Vertex*>( pVertexBuffer );
	g3dType::BumpTex1Vertex* bump_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>( pVertexBuffer );

	maAxisBox bounding_box;

	// The bind pose code is inefficient. We could be multiplying matrices
	// before applying them to the vertices for instance, or pre-transforming
	// the m_BoneVertices. But, none of that really matters if the artists just
	// freeze the transforms before skinning, resulting in a bind pose of
	// identity, and skipping all of that code.
	// We could also check in the constructor for IsIdentity() and do the 
	// Invert() once in the constructor also.
	maMatrix4x4 inv_bind_pose(m_BindPose);
	bool bHasBindPose = (!m_BindPose.IsIdentity());
	if (bHasBindPose)
	{
		inv_bind_pose.Invert();
	}

	// Tranform the vertices
	int i, j;
	for( i = 0; i < m_nNumVertices; ++i )
	{
		const smdlBoneVertex& bone_vertex = m_BoneVertices[i];

		maPoint3d &vert = (bump_frag) ? bump_vertices[i].m_Vertex : tex1_vertices[i].m_Vertex;
		maPoint3d &normal = (bump_frag) ? bump_vertices[i].m_Normal : tex1_vertices[i].m_Normal;
		vert.Set( 0.0f, 0.0f, 0.0f );

		int nLargestInfluenceIndex = 0;

		// Calculate each influence on the vertex
		int nInfluences = bone_vertex.m_Influences.size();
		if (nInfluences == 0)
		{
			vert = m_BasePositions[i];
			normal = m_BaseNormals[i];
		}
		else
		{
			for( j = 0; j < nInfluences; ++j )
			{
				const scBoneInfluence& influence = bone_vertex.m_Influences[j];
				const maMatrix4x4& bone_matrix = i_BoneMatrices[ influence.m_BoneIndex ];

				maVector3d influenced_pos = m_BasePositions[i];
				if (bHasBindPose)
					m_BindPose.Transform( influenced_pos );
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

				if (bHasBindPose)
				{
					normal = transform_normal( m_BindPose, m_BaseNormals[i] );
					normal = transform_normal( transform, normal );
					normal = transform_normal( inv_bind_pose, normal );
				}
				else
					normal = transform_normal( transform, m_BaseNormals[i] );
			}
		}

		bounding_box.Union( vert );
	}

	m_pFragment->SetBoundingBox( bounding_box );

	// Use the smdlSurface function to call Unlock in order
	// to unify the thread locking behavior
	this->UnlockFragment(m_pFragment);

	// Note: we really should be updating the S,T and SxT parts of the
	// BumpTex1 vertex when animating, but that would require extra
	// data and it isn't so obviously wrong if we don't do it.
}
