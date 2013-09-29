/*****************************************************************************
**  meshRenderer.hpp
**
**      see .hpp
**
** Area17
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Area18/mesh/meshRenderer.hpp"

#include "Area18/Area18Layer.hpp"
#include "Area18/ogl/oglContext.h"
#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglBuffer.hpp"
#include "Area18/ogl/oglSystem2D.h"
//#include "Area18/g3d/g3dSceneGlobal.hpp"
//#include "Area18/g3d/g3dSceneRenderUtil.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
//#include "Area18/shdr/shdrPipeline.hpp"
//#include "Area18/shdr/shdrUtil.hpp"

#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	UINT l_DrawCallLimit = GL_MAX_ELEMENTS_INDICES;
};

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
meshRenderer::meshRenderer()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
meshRenderer::~meshRenderer()
{
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void meshRenderer::Deallocate()
{
//	bumpVertexDecl::DeInitialize();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void meshRenderer::Reallocate()
{
//	bumpVertexDecl::Initialize();
}


//--------------------------------------------------------------------
// draw a limited number of triangles per draw call.
// this is to mitigate the windows TDR (timeout detection response)
// for expensive calls (e.g. high tessellation + GS amplification)
// -1 means use the D3D limit.
//--------------------------------------------------------------------
void meshRenderer::SetDrawLimit(int i_NumIndices)
{
	if (i_NumIndices == -1)
		l_DrawCallLimit = GL_MAX_ELEMENTS_INDICES;
	else
		l_DrawCallLimit = i_NumIndices;
}

int meshRenderer::Render( const meshTriMeshFrag* i_pFrag, const matMaterial* i_pMaterial, shdrPipeline* i_pEffect,
						 oglDevice* i_pDevice)
{
//	const meshTriMeshFrag* i_pFrag = dynamic_cast<const meshTriMeshFrag*>( i_pNode->GetFragment() );
//	DBG_ASSERT( i_pFrag, "A triangle mesh fragment expected" );

	bool bDoSkinning = false;//i_pFrag->GetHasSkinning() && i_pEffect->GetHasSkinning();
	bool bTessellate = false;//only tessellate if we have a displacement map

	bool bVelocityMaps = i_pFrag->GetHasVelocityBuffer();

	const matMaterial* pMtl = i_pFrag->GetMaterial();	//acquire material from fragment
	if( pMtl )
	{
		bTessellate = pMtl->GetHasDisplacement();	//use original material 
	}

	if (i_pEffect)
	{
		// does pipeline provide hull and domain shdr?
		bTessellate = false;
		//bTessellate &= i_pEffect->HasHardwareTessellation();
	}
	bTessellate &= g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;
	
//	ID3D11Buffer* buf[2] = {
//		i_pFrag->GetVertexBuffer()->GetVertexBuffer()->GetBuffer(), 
//		//bDoSkinning ? i_pFrag->GetSkinningBuffer()->GetVertexBuffer() : NULL
//		bVelocityMaps ? i_pFrag->GetVertexBuffer_Old()->GetVertexBuffer()->GetBuffer() : NULL
//	};
//	UINT strides[2] = {
//		i_pFrag->GetVertexStride(), 
//		//bDoSkinning ? sizeof(g3dType::SkinVertex) : 0
//		bVelocityMaps ? sizeof(g3dType::NonTexVertex) : 0
//	};
//	UINT offsets[2] = {0,0};
	
	glBindVertexArray(oglContext::currentContext()->getVAO());

	glBindBuffer(GL_ARRAY_BUFFER, i_pFrag->GetVertexBuffer()->GetVertexBuffer()->GetBuffer());
	
	// vertex layout: g3dType::BumpTex1Vertex
	// last value is a byte offset.
	DBG_ASSERT(i_pFrag->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex), "Bad vertex format/layout.");
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), 0); 
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*3)); 
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*6)); 
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*8)); 
//	glEnableVertexAttribArray(4);
//	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*11)); 

//	D3D11_INPUT_ELEMENT_DESC decl[] =
//	{
//		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_pFrag->GetIndexBuffer()->GetIndexBuffer()->GetBuffer());

	bool bDoubleSided = i_pFrag->GetDoubleSided();
	
	int numInds = 0;
	if( i_pFrag->GetPrimitiveType() == meshTriMeshFrag::e_TriangleList )
	{
		numInds = i_pFrag->GetNumNonShadowIndices();
	}
	else
	{
		numInds = i_pFrag->GetNumIndices();
	}
	int startIndex = 0;

	// Draw the triangles
	//

//	if( bTessellate && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation )
//	{
//		i_pDevice->m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
//	}
//	else
//	{
//		i_pDevice->m_pDeviceContext->IASetPrimitiveTopology( (GLenum)i_pFrag->GetPrimitiveType());
//	}

	GLenum indType = (i_pFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

//	i_pEffect->Bind(i_pDevice);
		CHECKGLERROR();

	// ability to limit to a certain number of primitives per draw call:

	int drawCallSize = l_DrawCallLimit;// multiple of 3 and of 2 for tris and lines!!
	if (drawCallSize > numInds) 
		drawCallSize = numInds;
	int i = 0;
	// draw in increments of drawCallSize indices.
	GLenum primType = (GLenum)i_pFrag->GetPrimitiveType();
	while (i <= numInds-drawCallSize)
	{
		glDrawRangeElements(primType, 
			i+startIndex, 
			i+startIndex+drawCallSize,
			drawCallSize,
			indType, 
			0);
		//i_pDevice->m_pDeviceContext->DrawIndexed(drawCallSize, i+startIndex, 0);
		CHECKGLERROR();
		i += drawCallSize;
	}
	// draw any remaining.
	if (i < numInds)
	{
		glDrawRangeElements((GLenum)i_pFrag->GetPrimitiveType(), 
			i+startIndex, 
			i+startIndex+numInds-i,
			numInds-i,
			indType, 
			0);
		//i_pDevice->m_pDeviceContext->DrawIndexed(numInds-i, i+startIndex, 0);
	}

	int nPrimitives = 0;
	if( i_pFrag->GetPrimitiveType() == meshTriMeshFrag::e_TriangleList )
	{
		// tri list
		nPrimitives = numInds / 3;
	}
	else
	{
		// line list
		nPrimitives = numInds / 2;
	}
	return nPrimitives;
}
