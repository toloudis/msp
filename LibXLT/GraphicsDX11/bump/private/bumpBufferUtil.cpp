/****************************************************************************\
**	bumpBufferUtil.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/bump/bumpBufferUtil.hpp"
#include "GraphicsDX11/tmesh/tmeshIndexBuffer.hpp"
#include "GraphicsDX11/tmesh/tmeshVertexBuffer.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

//#include "GraphicsDX11/g3d/g3dFVFWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"

#include <algorithm>
#include <vector>
#include <limits>

namespace
{
	//====================================================================
	//====================================================================
	//unsigned int convert_color(	const maFloatRGBA &i_Color )
	//{
	//	float alpha = 255 * i_Color.m_Alpha;
	//	maFunctions::Clamp(alpha, 0.0f, 255.0f);
	//	float red = 255 * i_Color.m_Red;
	//	maFunctions::Clamp(red, 0.0f, 255.0f);
	//	float green = 255 * i_Color.m_Green;
	//	maFunctions::Clamp(green, 0.0f, 255.0f);
	//	float blue = 255 * i_Color.m_Blue;
	//	maFunctions::Clamp(blue, 0.0f, 255.0f);

	//	unsigned int color =      ( (unsigned int) alpha << 24 ) 
	//							| ( (unsigned int) red << 16 ) 
	//							| ( (unsigned int) green << 8 )
	//							| ( (unsigned int) blue );
	//				
	//	return color;
	//}

	//====================================================================
	//====================================================================
 //   void getBoundingBox(const maPoint3d* i_Vertices,
	//	int	i_nVertices, maPoint3d& o_Min, maPoint3d& o_Max)
	//{
	//	o_Min = maPoint3d(FLT_MAX, FLT_MAX, FLT_MAX);
	//	o_Max = maPoint3d(FLT_MIN, FLT_MIN, FLT_MIN);
	//	for( int i = 0; i < i_nVertices; ++i )
	//	{
	//		o_Min.SetX(min(o_Min.GetX(), i_Vertices[i].GetX()));
	//		o_Min.SetY(min(o_Min.GetY(), i_Vertices[i].GetY()));
	//		o_Min.SetZ(min(o_Min.GetZ(), i_Vertices[i].GetZ()));
	//		o_Max.SetX(max(o_Max.GetX(), i_Vertices[i].GetX()));
	//		o_Max.SetY(max(o_Max.GetY(), i_Vertices[i].GetY()));
	//		o_Max.SetZ(max(o_Max.GetZ(), i_Vertices[i].GetZ()));
	//	}
	//}

	//====================================================================
	// Detect uv overlapping
	//====================================================================
	float detect_uv_overlapping(g3dType::BumpTex1Vertex *io_Vertices,
							int	i_nVertices,
							const g3dIndexPtr i_Indices,
							int	i_nIndices)
	{
		if (i_nIndices %3 != 0)
			return 0.0f;

		bool b32bit = i_Indices.Is32BitIndices();
		const envType::UInt16* pInd16 = i_Indices.GetIndices16();
		const envType::UInt32* pInd32 = i_Indices.GetIndices32();

		float overlapping = 0.0f;
		for(int i = 0; i < i_nIndices; i += 3 )
		{
			g3dType::BumpTex1Vertex& v0 = io_Vertices[b32bit ? pInd32[i] : pInd16[i]];
			g3dType::BumpTex1Vertex& v1 = io_Vertices[b32bit ? pInd32[i+1] : pInd16[i+1]];
			g3dType::BumpTex1Vertex& v2 = io_Vertices[b32bit ? pInd32[i+2] : pInd16[i+2]];

			maVector2d e0 = v1.m_TexCoord - v0.m_TexCoord;
			maVector2d e1 = v2.m_TexCoord - v0.m_TexCoord;

			overlapping += 0.5f * fabs(e0.m_X * e1.m_Y - e0.m_Y * e1.m_X);
			
		}

		return overlapping;
	}

	//====================================================================
	// Creates basis vectors, based on a vertex and index list.
	//====================================================================
	void create_basis_vectors(g3dType::BumpTex1Vertex *io_Vertices,
							int	i_nVertices,
							const g3dIndexPtr i_Indices,
							int	i_nIndices,
							bool i_bDoFullComputation)
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

		bool b32bit = i_Indices.Is32BitIndices();
		const envType::UInt16* pInd16 = i_Indices.GetIndices16();
		const envType::UInt32* pInd32 = i_Indices.GetIndices32();

		// Walk through the triangle list and calculate gradiants for each triangle.
		// Sum the results into the S and T components.
		maVector3d S, T, edge01, edge02, cp;
		for( i = 0; i < i_nIndices; i += 3 )
		{
			g3dType::BumpTex1Vertex& v0 = io_Vertices[b32bit ? pInd32[i] : pInd16[i]];
			g3dType::BumpTex1Vertex& v1 = io_Vertices[b32bit ? pInd32[i+1] : pInd16[i+1]];
			g3dType::BumpTex1Vertex& v2 = io_Vertices[b32bit ? pInd32[i+2] : pInd16[i+2]];

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

			//if (i_Indices[i] == 175 || i_Indices[i+1] == 175 || i_Indices[i+2] == 175)
			//{
			//	DBG_LOG("i_Indices[?] == 175, #" << i);
			//	DBG_LOG3("  m_S: %f %f %f", S.m_X, S.m_Y, S.m_Z);
			//	DBG_LOG3("  m_T: %f %f %f", T.m_X, T.m_Y, T.m_Z);
			//
			//	S.Normalize();
			//	T.Normalize();
			//
			//	DBG_LOG3("  N-S: %f %f %f", S.m_X, S.m_Y, S.m_Z);
			//	DBG_LOG3("  N-T: %f %f %f", T.m_X, T.m_Y, T.m_Z);
			//
			//	DBG_LOG5("  v0: %f %f %f (%f,%f)", v0.m_Vertex.m_X, v0.m_Vertex.m_Y, v0.m_Vertex.m_Z, v0.m_TexCoord.m_X, v0.m_TexCoord.m_Y);
			//	DBG_LOG5("  v1: %f %f %f (%f,%f)", v1.m_Vertex.m_X, v1.m_Vertex.m_Y, v1.m_Vertex.m_Z, v1.m_TexCoord.m_X, v1.m_TexCoord.m_Y);
			//	DBG_LOG5("  v2: %f %f %f (%f,%f)", v2.m_Vertex.m_X, v2.m_Vertex.m_Y, v2.m_Vertex.m_Z, v2.m_TexCoord.m_X, v2.m_TexCoord.m_Y);
			//	
			//}

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

  			// Get the cross of the S and T vectors
  			//vecSxT = io_Vertices[i].m_S / io_Vertices[i].m_T;

  			// Get the direction of the SxT vector
  			//if (vecSxT * io_Vertices[i].m_Normal < 0.0f)
  			//{
  			//	vecSxT *= -1.0f;
  			//}

			// It seems like the SxT is the same as the normal now
			//io_Vertices[i].m_SxT = vecSxT;
  			// Need a normalized normal
			//io_Vertices[i].m_SxT.Normalize();

			//DBG_LOG("Vertex #" << i);
			//DBG_LOG3("  Normal: %f %f %f", io_Vertices[i].m_Normal.m_X, io_Vertices[i].m_Normal.m_Y, io_Vertices[i].m_Normal.m_Z);
			//DBG_LOG3("  m_S: %f %f %f", io_Vertices[i].m_S.m_X, io_Vertices[i].m_S.m_Y, io_Vertices[i].m_S.m_Z);
			//DBG_LOG3("  m_T: %f %f %f", io_Vertices[i].m_T.m_X, io_Vertices[i].m_T.m_Y, io_Vertices[i].m_T.m_Z);
			//DBG_LOG3("  m_SxT: %f %f %f", io_Vertices[i].m_SxT.m_X, io_Vertices[i].m_SxT.m_Y, io_Vertices[i].m_SxT.m_Z);
  		} 
	}

	// find a scale and translate to guarantee all uvs in [0..1]
	void ComputeBakeFactors(int i_nUVs, const maPoint2d* i_UVs, maPoint2d& o_UVScale, maPoint2d& o_UVTranslate)
	{
		o_UVScale = maPoint2d(1,1);
		o_UVTranslate = maPoint2d(0,0);

		if ((i_nUVs == 0) || (i_UVs == NULL))
			return;
		// use the maAxisBox union call to accumulate a min and max.
		maAxisBox uvBounds;
		for (int i = 0; i < i_nUVs; ++i)
		{
			uvBounds.Union(maPoint3d(i_UVs[i].m_X, i_UVs[i].m_Y, 0.5f));
		}
		// is texture space contained in [0,0]..[1,1]?
		maAxisBox unitBox(0,1,0,1,0,1);
		if (unitBox.ContainsPoint(maPoint3d(uvBounds.GetMinX(), uvBounds.GetMinY(), uvBounds.GetMinZ())) && 
			unitBox.ContainsPoint(maPoint3d(uvBounds.GetMaxX(), uvBounds.GetMaxY(), uvBounds.GetMaxZ())))
			return;
		// if the uv bounds are smaller than epsilon, then they are degenerate, and we should treat
		// the mesh as if it has no UVs.
		if ((fabs(uvBounds.GetDiffX()) < maConstants::c_fEpsilon) || 
			(fabs(uvBounds.GetDiffY()) < maConstants::c_fEpsilon))
			return;

		// now i know i need to create new uv's by scaling and shifting to fit into [0,0]..[1,1].
		// factors to be applied as a "mad": uv1 = uv0 * s + t
		// R = 1/range
		// by translate first, then scale. (uv + t) * R = uv*R + R*t
		o_UVScale = maPoint2d(1.0f / uvBounds.GetDiffX(), 1.0f / uvBounds.GetDiffY());
		o_UVTranslate = maPoint2d(-uvBounds.GetMinX()*o_UVScale.m_X, -uvBounds.GetMinY()*o_UVScale.m_Y);
	}


}

//--------------------------------------------------------------------
//	CreateVertexBuffer - create vertex buffer from 
//		arrays of vertex data
//--------------------------------------------------------------------
shared_ptr<tmeshVertexBuffer> bumpBufferUtil::CreateVertexBuffer(const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												const maPoint2d* i_UVs,
												const maPoint3d* i_Ss,
												const maPoint3d* i_Ts,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int i_nNonShadowIndices,
												bool i_bMorphable,
												bool i_bComponentSort,
												bool i_bComputeBasisVectors )
{
	const int vertex_stride = sizeof( g3dType::BumpTex1Vertex );
	int nVertexBufferSize = i_nVertices * vertex_stride;

	g2dD3D11VertexBufferPtr vertex_buffer;

	// Create the vertex buffer later, after the verts are accumulated
//	g3dDX11BufferUtil::CreateVertexBuffer( nVertexBufferSize, 0/*GetVertexShader()*/, vertex_buffer, i_bMorphable );

	// Create temporary buffer for sorting the info into the correct format
	BYTE* vertex_buffer_copy = new BYTE[ nVertexBufferSize ];
	g3dType::BumpTex1Vertex *vbuffer_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(vertex_buffer_copy);

	// Fill in the temporary vertex buffer
	maAxisBox bounding_box;
	int nBadNormals = 0;
	for( int i = 0; i < i_nVertices; ++i )
	{
		vbuffer_vertices[i].m_Vertex = i_Vertices[i];

		vbuffer_vertices[i].m_Normal = i_Normals[i];
		if (!vbuffer_vertices[i].m_Normal.Normalize())
		{
			vbuffer_vertices[i].m_Normal = maVector3d(0,0,1);
			nBadNormals++;
		}

		if (i_UVs)
			vbuffer_vertices[i].m_TexCoord = i_UVs[i];
		else
			vbuffer_vertices[i].m_TexCoord = maPoint2d(0,0);


		if (i_Ss)
			vbuffer_vertices[i].m_S = i_Ss[i];
		else
			vbuffer_vertices[i].m_S = maPoint3d(0,0,0);

		if (i_Ts)
			vbuffer_vertices[i].m_T = i_Ts[i];
		else
			vbuffer_vertices[i].m_T = maPoint3d(0,0,0);

		bounding_box.Union( i_Vertices[i] );
	}
	if (nBadNormals > 0)
	{
		DBG_WARNING("Mesh has " << nBadNormals << " Bad Normals");
	}

	//m_bUseVertexColors = (i_Colors != NULL);

	// compute basis vectors if we don't already have them
	if (!i_Ss || ! i_Ts)
	{
		// only compute basis vectors for non-welding indices
		create_basis_vectors(vbuffer_vertices, i_nVertices, i_Indices, i_nNonShadowIndices, i_bComputeBasisVectors);
	}

	// Create the vertex buffer
	g3dDX11BufferUtil::CreateVertexBuffer( nVertexBufferSize, 0/*GetVertexShader()*/, vertex_buffer, i_bMorphable,
		vertex_buffer_copy, sizeof(g3dType::BumpTex1Vertex));

	// Lock, fill and unlock vertex buffer. Using the memcpy here is faster than
	// setting each vertex info into the video memory buffer above.
//	BYTE* vbuffer_mem = g3dDX11BufferUtil::LockVertexBuffer( vertex_buffer, i_bMorphable );
//	memcpy( vbuffer_mem, vertex_buffer_copy, nVertexBufferSize );
//	g3dDX11BufferUtil::UnlockVertexBuffer( vertex_buffer );

	// new behavior: maintain a vertex buffer copy in memory always.

//	// If the fragment is morphable, keep the vertex buffer copy for updating the vertices.
//	// Otherwise, delete it now.
//	if (!i_bMorphable && !i_bComponentSort)
//	{
//		delete [] vertex_buffer_copy;
//		vertex_buffer_copy = NULL;
//	}

	// Create shared_ptr to vertex buffer to return
	shared_ptr<tmeshVertexBuffer> shared_buffer(new tmeshVertexBuffer(g3dType::e_BumpTex1Vertex,
																	  vertex_stride,
																	  i_bMorphable));
	shared_buffer->SetVertexBuffer( vertex_buffer, vertex_buffer_copy, i_nVertices );

	// Vertex buffer holds a bounding box
	shared_buffer->SetBoundingBox( bounding_box );

	// Vertex buffer holds bake info to allow sharing of that info also
	maPoint2d bakeUVScale, bakeUVTranslate;
	float overlapping = 0.0f;
	ComputeBakeFactors(i_nVertices, i_UVs, bakeUVScale, bakeUVTranslate);
	overlapping = detect_uv_overlapping(vbuffer_vertices, i_nVertices, i_Indices, i_nNonShadowIndices);
	shared_buffer->SetUVBakeFactors(bakeUVScale, bakeUVTranslate);
	shared_buffer->SetUVOverlapFactor(overlapping);

	return shared_buffer;
}

//--------------------------------------------------------------------
//	CreateIndexBuffer - create index buffer from 
//		array of index data
//--------------------------------------------------------------------
shared_ptr<tmeshIndexBuffer> bumpBufferUtil::CreateIndexBuffer(const g3dIndexPtr i_Indices,
												int	i_nIndices,
												int i_nNonShadowIndices,
												int i_NumVertices,
												bool i_bComponentSort )
{
	g2dD3D11IndexBufferPtr index_buffer;

	// Create the index buffer
	//DBG_LOG("Creating index buffer, number of verts: " << i_nVertices);
	int nIndexBufferSize = g3dDX11BufferUtil::CreateAndFillIndexBuffer( i_Indices, 
		i_nIndices, i_NumVertices, index_buffer, i_bComponentSort );

	BYTE* index_buffer_copy = NULL;

	// Create index buffer copy
	//if (i_bComponentSort)
	{
		index_buffer_copy = g3dDX11BufferUtil::CreateBufferCopy(nIndexBufferSize, i_Indices, 
			g3dIndexPtr::Needs32Bit(i_NumVertices)?4:2,
			i_nIndices);
	}

	// Create shared_ptr to index buffer to return
	shared_ptr<tmeshIndexBuffer> shared_buffer(new tmeshIndexBuffer(i_bComponentSort));
	shared_buffer->SetIndexBuffer( index_buffer, index_buffer_copy, i_nIndices, nIndexBufferSize / i_nIndices, i_nNonShadowIndices );
	return shared_buffer;
}

//--------------------------------------------------------------------
//	CreateVelocityBuffer - create velocity buffer from 
//		vertex positions in i_pVertexBuffer
//--------------------------------------------------------------------
void bumpBufferUtil::CreateVelocityBuffer(const shared_ptr<tmeshVertexBuffer> &i_pVertexBuffer,
						  shared_ptr<tmeshVertexBuffer> &o_pVertexBuffer_Old,
						  bool i_bMorphable)
{
	// create velocity buffer
	o_pVertexBuffer_Old.reset(new tmeshVertexBuffer(g3dType::e_VelocityVertex, sizeof(g3dType::NonTexVertex), i_bMorphable ));

	int size = i_pVertexBuffer->GetNumVertices() * sizeof(g3dType::NonTexVertex);
	g2dD3D11VertexBufferPtr vertex_buffer = NULL;
	g3dDX11BufferUtil::CreateVertexBuffer( size, 0/*GetVertexShader()*/, vertex_buffer, i_bMorphable );

	// Lock the vertex buffers
	BYTE* dstVerts = g3dDX11BufferUtil::LockVertexBuffer( vertex_buffer , i_bMorphable );
	g3dType::NonTexVertex* dst_vertices = reinterpret_cast<g3dType::NonTexVertex*>(dstVerts);

	BYTE* srcVerts = i_pVertexBuffer->ReadOnlyLock();
	g3dType::BumpTex1Vertex* src_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(srcVerts);

	// Init
	if (dstVerts && srcVerts)
	{
		for( int i = 0; i < i_pVertexBuffer->GetNumVertices() ; ++i )
		{
			dst_vertices[i].m_Vertex = src_vertices[i].m_Vertex;//.XYZ();
		}
	}

	// Unlock the vertex buffers
	if (srcVerts)
		i_pVertexBuffer->ReadOnlyUnlock();

	if (dstVerts)
		g3dDX11BufferUtil::UnlockVertexBuffer( vertex_buffer );
	
	// Create temporary buffer for sorting the info into the correct format
	BYTE* vertex_buffer_copy = new BYTE[ size ];
	g3dType::NonTexVertex *vbuffer_vertices = reinterpret_cast<g3dType::NonTexVertex*>(vertex_buffer_copy);

	// Fill in the temporary vertex buffer
	for( int i = 0; i < i_pVertexBuffer->GetNumVertices(); ++i )
	{
		vbuffer_vertices[i].m_Vertex = maPoint3d();
	}

	o_pVertexBuffer_Old->SetVertexBuffer( vertex_buffer, vertex_buffer_copy, i_pVertexBuffer->GetNumVertices() );
}

//--------------------------------------------------------------------
//  BackupVertexBuffer - make copy of positions from i_pCurBuffer
//		into i_pOldBuffer.
//--------------------------------------------------------------------
void bumpBufferUtil::BackupVertexBuffer( tmeshVertexBuffer * i_pOldBuffer, 
										 tmeshVertexBuffer * i_pCurBuffer )
{
	DBG_ASSERT(i_pOldBuffer->GetVertexStride() == sizeof(g3dType::NonTexVertex), "Unexpected dest vertex format in BackupVertexBuffer");
	DBG_ASSERT(i_pCurBuffer->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex), "Unexpected src vertex format in BackupVertexBuffer");
	if (i_pCurBuffer->GetVertexBufferCopy()) 
	{
		// Lock the vertex buffer
		BYTE* srcVerts = i_pCurBuffer->GetVertexBufferCopy();
		g3dType::BumpTex1Vertex* src_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(srcVerts);

		g2dD3D11VertexBufferPtr dst_buffer = i_pOldBuffer->GetVertexBuffer();
		BYTE* dstVerts = g3dDX11BufferUtil::LockVertexBuffer( dst_buffer , i_pOldBuffer->GetLockable() );
		g3dType::NonTexVertex* dst_vertices = reinterpret_cast<g3dType::NonTexVertex*>(dstVerts);
	
		// Inspect i_pCurBuffer
		//BYTE* currVerts = i_pCurBuffer->ReadOnlyLock();
		//g3dType::BumpTex1Vertex* curr_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(currVerts);

		//maPoint3d previous_Vtx = dst_vertices[0].m_Vertex;
		//maPoint3d current_Vtx = curr_vertices[0].m_Vertex;	
		//
		//GetVertexBuffer()->ReadOnlyUnlock();

		// Get the positions
		if (src_vertices && dst_vertices)
		{
			for( int i = 0; i < i_pCurBuffer->GetNumVertices() ; ++i )
			{
				dst_vertices[i].m_Vertex = src_vertices[i].m_Vertex;
				dst_vertices[i].m_Normal = src_vertices[i].m_Normal;
			}
		}

		// Unlock the vertex buffer		
		if (dstVerts)
			g3dDX11BufferUtil::UnlockVertexBuffer( i_pOldBuffer->GetVertexBuffer() );
	}
}
