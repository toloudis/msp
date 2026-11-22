/****************************************************************************\
**	bumpTriMeshBumpFrag.cpp
**
**	See bumpTriMeshBumpFrag.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_BUMP_TRIMESHBUMPFRAG_HPP
#include "GraphicsVulkan/bump/bumpTriMeshBumpFrag.hpp"
#endif

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif
#ifndef G3D_OCCLUSIONTREE_HPP
#include "Graphics/g3d/g3dOcclusionTree.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_BUMP_BUFFERUTIL_HPP
#include "GraphicsVulkan/bump/bumpBufferUtil.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif

#include <algorithm>
#include <vector>
#include <limits>

namespace gvk
{

//============================================================================
//============================================================================


int bumpTriMeshBumpFrag::sm_RendererId = 0;
bool bumpTriMeshBumpFrag::sm_bComputeBasisVectors = true;

//--------------------------------------------------------------------
// Set id for fragments in order to choose renderer
//--------------------------------------------------------------------
//static
void bumpTriMeshBumpFrag::SetRendererId(int i_RenderMode)
{
	bumpTriMeshBumpFrag::sm_RendererId = i_RenderMode;
}

//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
//static 
void bumpTriMeshBumpFrag::SetComputeBasisVectors(bool i_bCompute)
{
	bumpTriMeshBumpFrag::sm_bComputeBasisVectors = i_bCompute;
}

//--------------------------------------------------------------------
//	Constructor - One set of texture coordinates, plus prelit color
//		per vertex for ambient occlusion
//--------------------------------------------------------------------
bumpTriMeshBumpFrag::bumpTriMeshBumpFrag(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										const maPoint2d* i_UVs,
										//const maFloatRGBA* i_Colors,
										const maPoint3d* i_Ss,
										const maPoint3d* i_Ts,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										int i_nNonShadowIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable,
										bool i_bComponentSort,
										PrimitiveType i_PrimitiveType)
:	tmeshFrag( i_pMaterial,
			  g3dType::e_BumpTex1Vertex,
			  0, //fvf is dead in dx10
			  sizeof( g3dType::BumpTex1Vertex ),
			  i_bMorphable,
			  i_bComponentSort ),
	m_pAOTree(NULL)
{
	this->SetRenderMode(bumpTriMeshBumpFrag::sm_RendererId);

	shared_ptr<tmeshVertexBuffer> shared_vertex_buffer =
		bumpBufferUtil::CreateVertexBuffer(i_Vertices,
											i_Normals,
											i_UVs,
											i_Ss,
											i_Ts,
											i_nVertices,
											i_Indices,
											i_nNonShadowIndices,
											i_bMorphable,
											i_bComponentSort,
											sm_bComputeBasisVectors );


	if (shared_vertex_buffer)
	{
		maAxisBox bounding_box = shared_vertex_buffer->GetBoundingBox();
		SetBoundingBox( bounding_box );
	}

	shared_ptr<tmeshIndexBuffer> shared_index_buffer =
		bumpBufferUtil::CreateIndexBuffer(i_Indices,
											i_nIndices,
											i_nNonShadowIndices,
											i_nVertices,
											i_bComponentSort );



	// Set shared_ptr to buffers directly
	if(shared_vertex_buffer)
		this->SetVertexBuffer( shared_vertex_buffer );
	//SetVertexBuffer( vertex_buffer, vertex_buffer_copy, i_nVertices );
	if (shared_index_buffer)
		this->SetIndexBuffer( shared_index_buffer );
	//SetIndexBuffer( index_buffer, index_buffer_copy, i_nIndices, nIndexBufferSize / i_nIndices, i_nNonShadowIndices );

	CreateVelocityBuffer(i_bMorphable);

	// Prepare buffer for sorting triangles internally
	if (i_bComponentSort)
	{
		m_SortList.resize(i_nIndices / 3);
	}
}


//--------------------------------------------------------------------
//	Constructor taking shared buffers
//--------------------------------------------------------------------
bumpTriMeshBumpFrag::bumpTriMeshBumpFrag(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
										 shared_ptr<tmeshIndexBuffer> & i_IndexBuffer,
										matMaterial* i_pMaterial)
:	tmeshFrag( i_pMaterial,
			  g3dType::e_BumpTex1Vertex,
			  0, // fvf is dead
			  sizeof( g3dType::BumpTex1Vertex ),
			  i_VertexBuffer->GetLockable(),
			  i_IndexBuffer->GetLockable() ),
	m_pAOTree(NULL)
{
	this->SetRenderMode(bumpTriMeshBumpFrag::sm_RendererId);

	maAxisBox bounding_box = i_VertexBuffer->GetBoundingBox();
	SetBoundingBox( bounding_box );
	
	// Set shared_ptr to buffers directly
	this->SetVertexBuffer( i_VertexBuffer );
	this->SetIndexBuffer( i_IndexBuffer );

	CreateVelocityBuffer(i_VertexBuffer->GetLockable());

	// Prepare buffer for sorting triangles internally
	if (i_IndexBuffer->GetLockable()) //i_bComponentSort
	{
		m_SortList.resize(i_IndexBuffer->GetNumIndices() / 3);
	}

}

//--------------------------------------------------------------------
//	Constructor taking shared buffers
//--------------------------------------------------------------------
bumpTriMeshBumpFrag::bumpTriMeshBumpFrag(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
										 shared_ptr<tmeshVertexBuffer> &i_VertexBufferOld,
										 shared_ptr<tmeshIndexBuffer> & i_IndexBuffer,
										 matMaterial* i_pMaterial)
:	tmeshFrag( i_pMaterial,
			  g3dType::e_BumpTex1Vertex,
			  0, // fvf is dead
			  sizeof( g3dType::BumpTex1Vertex ),
			  i_VertexBuffer->GetLockable(),
			  i_IndexBuffer->GetLockable() ),
	m_pAOTree(NULL)
{
	this->SetRenderMode(bumpTriMeshBumpFrag::sm_RendererId);

	maAxisBox bounding_box = i_VertexBuffer->GetBoundingBox();
	SetBoundingBox( bounding_box );
	
	// Set shared_ptr to buffers directly
	this->SetVertexBuffer( i_VertexBuffer );
	this->SetIndexBuffer( i_IndexBuffer );

	// Don't allocate a new velocity buffer this time, just use the one given to us
	// which is shared with other fragments.
	GetVertexBuffer_Old() = i_VertexBufferOld;
	this->SetHasVelocityBuffer(i_VertexBuffer->GetLockable());

	// Prepare buffer for sorting triangles internally
	if (i_IndexBuffer->GetLockable()) //i_bComponentSort
	{
		m_SortList.resize(i_IndexBuffer->GetNumIndices() / 3);
	}
}

//--------------------------------------------------------------------
// Copy constructor - shallow copy of just base information
//--------------------------------------------------------------------
bumpTriMeshBumpFrag::bumpTriMeshBumpFrag(const bumpTriMeshBumpFrag &i_Frag)
:  	tmeshFrag(i_Frag),
	m_pAOTree(NULL)
{
	// tmeshFrag base class uses shared_ptr, so the default copy constructor will work.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const g3dOcclusionTree* bumpTriMeshBumpFrag::GetAOTree() const
{
	if ((m_pAOTree == NULL) && (GetNumIndices() % 3 == 0))
	{

	    maPoint3d m,M;
		maAxisBox b = this->GetBoundingBox();
		m = maPoint3d(b.GetMinX(), b.GetMinY(), b.GetMinZ());
		M = maPoint3d(b.GetMaxX(), b.GetMaxY(), b.GetMaxZ());


		int nVerts = GetNumVertices();
		int nInds = GetNumIndices();

		maPoint3d* verts = new maPoint3d[nVerts];
		// Lock the vertex buffer
		uint8_t* rawVerts = GetVertexBuffer()->ReadOnlyLock();
		g3dType::BumpTex1Vertex* vbuffer_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(rawVerts);
		// Get the positions
		for( int i = 0; i < nVerts; ++i )
		{
			verts[i] = vbuffer_vertices[i].m_Vertex;
		}
		// Unlock the vertex buffer
		GetVertexBuffer()->ReadOnlyUnlock();

		uint32_t* inds = new uint32_t[nInds];
//		std::vector<uint32_t> inds;
//		inds.resize(nInds);
		// Lock the index buffer
		uint8_t* rawInds = GetIndexBuffer()->ReadOnlyLockIndices();
		// Get the indices
		if (g3dIndexPtr::Needs32Bit(nVerts))
		{
			for (int i = 0; i < nInds; i++)
				inds[i] = (reinterpret_cast<const uint32_t*>(rawInds))[i];
		}
		else
		{
			for (int i = 0; i < nInds; i++)
				inds[i] = (reinterpret_cast<const uint16_t*>(rawInds))[i];
		}
		// Unlock the index buffer
		GetIndexBuffer()->ReadOnlyUnlockIndices();

		g3dIndexPtr indexPtr(inds);

		m_pAOTree = new g3dOcclusionTree;
		m_pAOTree->build(verts, nVerts, indexPtr, nInds, m, M);

		delete [] inds;
		delete [] verts;
	}
	return m_pAOTree;
}


//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
bumpTriMeshBumpFrag::~bumpTriMeshBumpFrag()
{
	if (m_pAOTree)
		delete m_pAOTree;
}

//---------------------------------------------------------------------------
// Update buffer functions provide a way to alter the topology of the
//	geometry without deleting and creating a new fragment. The assumption
//	is that the number of vertices or indices is changing. Otherwise,
//	use the UpdateVertices() or Lock/Unlock() functions.
//---------------------------------------------------------------------------
void bumpTriMeshBumpFrag::UpdateVertexBuffer(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
											 shared_ptr<tmeshVertexBuffer> &i_VertexBufferOld)
{
	maAxisBox bounding_box = i_VertexBuffer->GetBoundingBox();
	SetBoundingBox( bounding_box );
	
	// Set shared_ptr to buffer directly
	this->SetVertexBuffer( i_VertexBuffer );

	// Don't allocate a new velocity buffer this time, just use the one given to us
	// which is shared with other fragments.
	GetVertexBuffer_Old() = i_VertexBufferOld;
	this->SetHasVelocityBuffer(i_VertexBuffer->GetLockable());
}
void bumpTriMeshBumpFrag::UpdateIndexBuffer(shared_ptr<tmeshIndexBuffer> &i_IndexBuffer)
{
	// Set shared_ptr to buffer directly
	this->SetIndexBuffer( i_IndexBuffer );

	// Prepare buffer for sorting triangles internally
	if (i_IndexBuffer->GetLockable()) //i_bComponentSort
	{
		m_SortList.resize(i_IndexBuffer->GetNumIndices() / 3);
	}
}

//---------------------------------------------------------------------------
// UpdateVertices - alter the position of the vertices in the
// given fragment. i_pNormals may be NULL, in which case the
// normals should remain as before. i_NumVertices should
// represent the number of positions given and should match the
// number of vertices in the fragment.
// This method can only be called on a fragment that was created
// with the "morphable" flag set to true.
//---------------------------------------------------------------------------
//virtual 
void bumpTriMeshBumpFrag::UpdateVertices( int i_NumVertices, 
						const maPoint3d* i_pVertices, 
						const maVector3d* i_pNormals )
{
	maAxisBox bounding_box;

	if (this->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex))
	{
		// Lock the vertex buffer
		g3dType::BumpTex1Vertex* vbuffer_vertices = 
			reinterpret_cast<g3dType::BumpTex1Vertex*>( this->Lock() );

		// Fill in the vertex buffer
		for( int i = 0; i < i_NumVertices; ++i )
		{
			vbuffer_vertices[i].m_Vertex = i_pVertices[i];
			if (i_pNormals) vbuffer_vertices[i].m_Normal = i_pNormals[i];
			bounding_box.Union( i_pVertices[i] );
		}
		this->Unlock();
	}
	else
	{
		DBG_ASSERT(false, "Unexpected vertex stride in bumpTriMeshBumpFrag::UpdateVertices");
	}

	SetBoundingBox( bounding_box );
}

//---------------------------------------------------------------------------
// ComponentSort - let the fragment sort its internal components before
// rendering. The current model to world transformation and the camera 
// position are passed as arguments in order to do the sorting.
//---------------------------------------------------------------------------
//virtual 
void bumpTriMeshBumpFrag::ComponentSort(const maMatrix4x4& i_Transorm,
										const maPoint3d& i_CameraPos)
{
	// Note: the internal sorting is a non-const way of handling the 
	// fragments which wouldn't really work if the fragments were shared.
	// A better way may be to return a list of the order the components
	// should be rendered without really altering the actual indices.
	// Then the renderer could handle the index ordering at render time.
	// Also, this type of code needs to get into the regular tmesh handling.
	//  But at the time of writing, only the bump fragment needed it.
	//

	DBG_ASSERT(this->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex), "Unexpected vertex stride in bumpTriMeshBumpFrag::ComponentSort");
	
	// Access the vertex buffer without locking or modifying
	g3dType::BumpTex1Vertex* vbuffer_vertices = 
		reinterpret_cast<g3dType::BumpTex1Vertex*>( GetVertexBuffer()->ReadOnlyLock() );

	// Lock the index buffer
	unsigned char *index_buffer =  this->LockIndices();
	uint16_t* pIndices16 = reinterpret_cast<uint16_t*>( index_buffer );
	uint32_t* pIndices32 = reinterpret_cast<uint32_t*>( index_buffer );
	bool b32Bit = (this->GetSizeOfIndex() == 4);

	const int nIndices = this->GetNumNonShadowIndices();
	const int nTris = nIndices / 3;		// Always tris in bump-map frag

	int tind = 0;
	maPoint3d center;
	for (int i = 0; i < nIndices; i+=3)
	{
		sTri& tri = m_SortList[tind++];
		tri.m_I1 = b32Bit? pIndices32[i] : pIndices16[i];
		tri.m_I2 = b32Bit? pIndices32[i+1] : pIndices16[i+1];
		tri.m_I3 = b32Bit? pIndices32[i+2] : pIndices16[i+2];

		center =  ( vbuffer_vertices[tri.m_I1].m_Vertex + 
					vbuffer_vertices[tri.m_I2].m_Vertex + 
					vbuffer_vertices[tri.m_I3].m_Vertex ) / 3.0f;

		i_Transorm.Transform( center );
		tri.m_Dist = ( center - i_CameraPos ).LengthSqr();
	}
	std::sort(m_SortList.begin(), m_SortList.end());
	int ind = 0;
	for (int t = 0; t < nTris; ++t)
	{
		sTri& tri = m_SortList[t];
		if (b32Bit)
		{
			pIndices32[ind++] = tri.m_I1;
			pIndices32[ind++] = tri.m_I2;
			pIndices32[ind++] = tri.m_I3;
		}
		else
		{
			pIndices16[ind++] = tri.m_I1;
			pIndices16[ind++] = tri.m_I2;
			pIndices16[ind++] = tri.m_I3;
		}
	}

	this->UnlockIndices();
	GetVertexBuffer()->ReadOnlyUnlock();
}

void bumpTriMeshBumpFrag::Split(const maAxisBox& i_camSpaceBox)
{
	DBG_ASSERT(this->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex), "Unexpected vertex stride in bumpTriMeshBumpFrag::ComponentSort");
	
	// Access the vertex buffer without locking or modifying
	g3dType::BumpTex1Vertex* vbuffer_vertices = 
		reinterpret_cast<g3dType::BumpTex1Vertex*>( GetVertexBuffer()->ReadOnlyLock() );

	// Lock the index buffer
	unsigned char *index_buffer =  GetIndexBuffer()->ReadOnlyLockIndices();
	uint16_t* pIndices16 = reinterpret_cast<uint16_t*>( index_buffer );
	uint32_t* pIndices32 = reinterpret_cast<uint32_t*>( index_buffer );
	bool b32Bit = (this->GetSizeOfIndex() == 4);

	const int nIndices = this->GetNumNonShadowIndices();
	const int nTris = nIndices / 3;		// Always tris in bump-map frag

	// simple test: just split the indices into fourths, as a test
	sSubFrag subFrag[4];
	int nSubFrags = 4;
	int tind = 0;
	maPoint3d center;

	// ASSUME sort list is in same order as index buffer at this time 
	for (int j = 0; j < nSubFrags; j++)
	{
		subFrag[j].m_startIndex = (j * (nTris/nSubFrags)) * 3;
		int start = subFrag[j].m_startIndex;
		int end = subFrag[j].m_startIndex + (nTris/nSubFrags) * 3;
		for (int i = start; i < end; i++)
		{
			subFrag[j].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[i] : pIndices16[i]].m_Vertex);
		}
		subFrag[j].m_numTris = (end-start)/3;
	}
	m_subFragments.clear();
	m_subFragments.push_back(subFrag[0]);
	m_subFragments.push_back(subFrag[1]);
	m_subFragments.push_back(subFrag[2]);
	m_subFragments.push_back(subFrag[3]);

#if 0
	// split the indices into 4ths, by distance
	sSubFrag subFrag[4];
	int nSubFrags = 4;
	int tind = 0;
	maPoint3d center;

	int ind;
	float zfar = i_camSpaceBox.GetMaxZ() * i_camSpaceBox.GetMaxZ();
	float znear = i_camSpaceBox.GetMinZ() * i_camSpaceBox.GetMinZ();
	subFrag[0].m_startIndex = nIndices;
	subFrag[1].m_startIndex = nIndices;
	subFrag[2].m_startIndex = nIndices;
	subFrag[3].m_startIndex = nIndices;
	// ASSUME sort list is in same order as index buffer at this time 
	for (int i = 0; i < nTris; i++)
	{
		sTri& tri = m_SortList[i];
		if (tri.m_Dist < znear)
		{
			subFrag[0].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I1] : pIndices16[tri.m_I1]].m_Vertex);
			subFrag[0].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I2] : pIndices16[tri.m_I2]].m_Vertex);
			subFrag[0].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I3] : pIndices16[tri.m_I3]].m_Vertex);
			subFrag[0].m_numTris++;
			if (subFrag[0].m_startIndex > i*3)
				subFrag[0].m_startIndex = i*3;
		}
		else if ( tri.m_Dist < (zfar - znear)*0.5 ) 
		{
			subFrag[1].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I1] : pIndices16[tri.m_I1]].m_Vertex);
			subFrag[1].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I2] : pIndices16[tri.m_I2]].m_Vertex);
			subFrag[1].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I3] : pIndices16[tri.m_I3]].m_Vertex);
			subFrag[1].m_numTris++;
			if (subFrag[1].m_startIndex > i*3)
				subFrag[1].m_startIndex = i*3;
		}
		else if (tri.m_Dist < zfar)
		{
			subFrag[2].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I1] : pIndices16[tri.m_I1]].m_Vertex);
			subFrag[2].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I2] : pIndices16[tri.m_I2]].m_Vertex);
			subFrag[2].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I3] : pIndices16[tri.m_I3]].m_Vertex);
			subFrag[2].m_numTris++;
			if (subFrag[2].m_startIndex > i*3)
				subFrag[2].m_startIndex = i*3;
		}
		else
		{
			subFrag[3].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I1] : pIndices16[tri.m_I1]].m_Vertex);
			subFrag[3].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I2] : pIndices16[tri.m_I2]].m_Vertex);
			subFrag[3].m_bounds.Union(vbuffer_vertices[b32Bit? pIndices32[tri.m_I3] : pIndices16[tri.m_I3]].m_Vertex);
			subFrag[3].m_numTris++;
			if (subFrag[3].m_startIndex > i*3)
				subFrag[3].m_startIndex = i*3;
		}
	}
	m_subFragments.clear();
	m_subFragments.push_back(subFrag[0]);
	m_subFragments.push_back(subFrag[1]);
	m_subFragments.push_back(subFrag[2]);
	m_subFragments.push_back(subFrag[3]);
#endif

	GetIndexBuffer()->ReadOnlyUnlockIndices();
	GetVertexBuffer()->ReadOnlyUnlock();
}

//====================================================================
//	trisort_pred - sort triangle structure defined above
//====================================================================
bool bumpTriMeshBumpFrag::sTri::operator < ( const bumpTriMeshBumpFrag::sTri& i_Tri2 )
{
	return m_Dist > i_Tri2.m_Dist;
}

//---------------------------------------------------------------------------
// GetFaceInfo - return a triangle from the underlying mesh, if one exists.
//	If no such triangle can be found, return false. This call can be expensive
//	as it may involve locking a vertex buffer.
//---------------------------------------------------------------------------
bool bumpTriMeshBumpFrag::GetFaceInfo(int i_Face, maPoint3d o_Points[3], maVector3d o_Normals[3])
{
	if (i_Face >= GetNumIndices()/3)
		return false;

	// if there's a sys mem copy of the vtx or index buffer, we should use it!
	uint8_t* pIndexBuffer = GetIndexBuffer()->ReadOnlyLockIndices();

	// need to know whether 16 or 32 bit indices.
	bool indexSize32 = g3dIndexPtr::Needs32Bit(GetNumVertices());
	uint32_t ind[3];
	if (indexSize32)
	{
		uint32_t *buffer = reinterpret_cast<uint32_t*>(pIndexBuffer);
		ind[0] = buffer[i_Face*3+0];
		ind[1] = buffer[i_Face*3+1];
		ind[2] = buffer[i_Face*3+2];
	}
	else
	{
		uint16_t *buffer = reinterpret_cast<uint16_t*>(pIndexBuffer);
		ind[0] = buffer[i_Face*3+0];
		ind[1] = buffer[i_Face*3+1];
		ind[2] = buffer[i_Face*3+2];
	}
	// done with buffer.
	GetIndexBuffer()->ReadOnlyUnlockIndices();

	// now get the vertices.
	uint8_t* pVertexBuffer = GetVertexBuffer()->ReadOnlyLock();
	g3dType::BumpTex1Vertex* vbuffer = reinterpret_cast<g3dType::BumpTex1Vertex*>(pVertexBuffer);
	o_Points[0] = vbuffer[ind[0]].m_Vertex;
	o_Points[1] = vbuffer[ind[1]].m_Vertex;
	o_Points[2] = vbuffer[ind[2]].m_Vertex;
	o_Normals[0] = vbuffer[ind[0]].m_Normal;
	o_Normals[1] = vbuffer[ind[1]].m_Normal;
	o_Normals[2] = vbuffer[ind[2]].m_Normal;

	// done with buffer.
	 GetVertexBuffer()->ReadOnlyUnlock();

	return true;
}

//----------------------------------------------------------------------------
//	Return uv scaling factors to keep uvs within 0..1
//----------------------------------------------------------------------------
void bumpTriMeshBumpFrag::GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const
{
	// Vertex buffer stores the bake factors now
	GetVertexBuffer()->GetUVBakeFactors(o_Scale, o_Translate);
	// default impl
	/*o_Scale = m_BakeUVScale;
	o_Translate = m_BakeUVTranslate;*/
}

//----------------------------------------------------------------------------
//	Return uv overlap factor
//----------------------------------------------------------------------------
float bumpTriMeshBumpFrag::GetUVOverlapFactor() const
{
	return GetVertexBuffer()->GetUVOverlapFactor();
}

void bumpTriMeshBumpFrag::SetHasSkinning(bool i_bSkinning)
{
	if (i_bSkinning)
	{
		// if skinning data already created, then don't do anything.
		// assumes num vertices won't change for lifetime of fragment.
		if (GetSkinningBuffer().get() == NULL)
		{
			// create skinning buffer
			GetSkinningBuffer().reset(new tmeshVertexBuffer(g3dType::e_SkinVertex, sizeof(g3dType::SkinVertex), GetMorphable()));

			int nSkinningBufferSize = GetNumVertices() * sizeof(g3dType::SkinVertex);
			g2dVkVertexBufferPtr vertex_buffer = NULL;
			g3dVkBufferUtil::CreateVertexBuffer( nSkinningBufferSize, 0/*GetVertexShader()*/, vertex_buffer, GetMorphable() );
			// Create temporary buffer for sorting the info into the correct format
			uint8_t* vertex_buffer_copy = new uint8_t[ nSkinningBufferSize ];
			g3dType::SkinVertex *vbuffer_vertices = reinterpret_cast<g3dType::SkinVertex*>(vertex_buffer_copy);

			// Fill in the temporary vertex buffer
			maAxisBox bounding_box;
			int nBadNormals = 0;
			for( int i = 0; i < GetNumVertices(); ++i )
			{
				vbuffer_vertices[i].m_Bones = maVector4d();
				vbuffer_vertices[i].m_Weights = maVector4d();
			}

			// Lock, fill and unlock vertex buffer. Using the memcpy here is faster than
			// setting each vertex info into the video memory buffer above.
			if (vertex_buffer)
			{
				uint8_t* vbuffer_mem = g3dVkBufferUtil::LockVertexBuffer( vertex_buffer, GetMorphable() );
				if (vbuffer_mem)
				{
					memcpy( vbuffer_mem, vertex_buffer_copy, nSkinningBufferSize );
					g3dVkBufferUtil::UnlockVertexBuffer( vertex_buffer );
				}
			}

			if (vertex_buffer && vertex_buffer_copy)
				SetSkinningBuffer( vertex_buffer, vertex_buffer_copy, GetNumVertices() );
		}
	}
	else
	{
		// destroy skinning buffer.
		GetSkinningBuffer().reset();
	}
	g3dFragment::SetHasSkinning(i_bSkinning);
}


void bumpTriMeshBumpFrag::CreateVelocityBuffer(bool i_bMorphable)
{
	if (i_bMorphable)
	{
		// if velocity map data already created, then don't do anything.
		// assumes num vertices won't change for lifetime of fragment.

		if (GetVertexBuffer_Old().get() == NULL){

			// create velocity buffer			
			bumpBufferUtil::CreateVelocityBuffer(GetVertexBuffer(), GetVertexBuffer_Old(), i_bMorphable);

			this->SetHasVelocityBuffer(true);
		}
	}
	else
	{
		// destroy velocity buffer.
		GetVertexBuffer_Old().reset();
	}
}

}
