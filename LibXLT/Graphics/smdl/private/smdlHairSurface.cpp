/*****************************************************************************
**	smdlHairSurface.hpp
**
**		smdlHairSurface handles a single skinned polygon mesh
**	with no remapping.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlHairSurface.hpp"

#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"


//--------------------------------------------------------------------
//	smdlHairSurface requires a hair info structure. It creates a
//	hair fragment for it and assumes ownership of the fragment. 
//--------------------------------------------------------------------
smdlHairSurface::smdlHairSurface( const mdlHairInfo& i_HairInfo,
								  matMaterial *i_pMaterial )
:	m_HairName(i_HairInfo.m_HairName),
	m_pFragment(NULL),
	m_pNode(NULL),
	m_pInternalHairShader(NULL)
{
	// Store number of vertices expected in animation buffer
	m_NumOriginalVertices  = i_HairInfo.m_nVerticesPerStrand * i_HairInfo.m_Strands.size();

	// Create fragment
	matMaterial *pMaterial = i_pMaterial;
	if (!pMaterial)
	{
		// If we weren't given a material for the hair surface,
		// then fall back on the internal hair shader.
		m_pInternalHairShader = new matMaterial("Hair_SH.fx");
		pMaterial = m_pInternalHairShader;
	}
	m_pFragment = mdlFragCreate::CreateHairFragment( i_HairInfo, pMaterial );

	// Create a node for our fragment so that we can control the visibility
	m_pNode = new g3dSceneNode( m_pFragment );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlHairSurface::~smdlHairSurface()
{
	// Since fragments are not shared for hair, we should delete ours
	delete m_pFragment;

	// If we had to use the no-UI internal hair shader, then delete it now
	if (m_pInternalHairShader)
		delete m_pInternalHairShader;
}

//--------------------------------------------------------------------
// Return node that contains the fragments in this model in a small
//	sub-scene graph
//--------------------------------------------------------------------
g3dSceneNode* smdlHairSurface::RootNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlHairSurface::SetVisible(bool i_bVisible)
{
	m_pNode->SetRenderable( i_bVisible );
}
//virtual 
bool smdlHairSurface::GetVisible() const
{
	return m_pNode->GetRenderable();
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
bool smdlHairSurface::CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const
{
	int num_verts = 0, num_normals = 0;
	i_VertKeys->GetNumVertices(num_verts, num_normals);
	if (num_verts != m_NumOriginalVertices)
	{
		DBG_WARNING("Hair " << m_HairName << ", number of vertices " << m_NumOriginalVertices << " does not match animation " << num_verts);
		return false;
	}
	return true;
}

//--------------------------------------------------------------------
//	VertexTransformHair - use baked vertex animation to transform
//		the strands in this group.
//--------------------------------------------------------------------
void smdlHairSurface::VertexTransformHair(float i_CurrentFrame,
											const smdlVertexAnimKeys &i_VertKeys)
{
	// Get frames to interpolate between. Should we clamp to one frame or the other?
	vtxVertexFrame *pFrame0 = NULL, *pFrame1 = NULL;
	float alpha = 0.0f;
	i_VertKeys->GetBracketingFrames(i_CurrentFrame, pFrame0, pFrame1, alpha);
	if (!pFrame0)
		return;
	float inv_alpha = (1 - alpha);

	int numVerts = pFrame0->m_Positions.size();
	if (numVerts == m_NumOriginalVertices)
	{
		if (m_VertCache.size() < numVerts)
		{
			m_VertCache.resize( numVerts );
		}

		if (pFrame1)
		{
			for (int j=0; j<numVerts; j++)
			{
				m_VertCache[j] = alpha * pFrame1->m_Positions[ j ] + inv_alpha * pFrame0->m_Positions[ j ];
			}
		}
		else
		{
			for (int j=0; j<numVerts; j++)
			{
				m_VertCache[j] = pFrame0->m_Positions[ j ];
			}
		}

		m_pFragment->UpdateVertices(m_NumOriginalVertices, &m_VertCache[0]);

		maAxisBox bbox;
		bbox.Union(&m_VertCache[0], m_NumOriginalVertices);
		m_pFragment->SetBoundingBox( bbox );
	}
}

//--------------------------------------------------------------------
// Animation data is giving us the bounding box for the surface 
//	before the actual animation is done. Set this bounding box 
//	into the fragment.
//--------------------------------------------------------------------
void smdlHairSurface::BBoxTransform(const maAxisBox &i_BBox)
{
	m_pFragment->SetBoundingBox( i_BBox );
}
