/*****************************************************************************
**  g3dDX11BufferUtil.hpp
**
**      g3dDX11BufferUtil has functions for dealing with vertex and index
**		buffers
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DX11BUFFERUTIL_HPP
#error g3dDX11BufferUtil.hpp multiply included
#endif
#define G3D_DX11BUFFERUTIL_HPP

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif


//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

namespace g3dDX11BufferUtil
{
	//--------------------------------------------------------------------
	// Set flag for whether to store geometry in video or system memory
	//--------------------------------------------------------------------
	void StoreGeometryInVideoMemory(bool i_bVideoMem);

	//--------------------------------------------------------------------
	// Get current video mem usage totals, in KB
	//--------------------------------------------------------------------
	float GetTotalVertexBufferMemory();
	float GetTotalIndexBufferMemory();
	unsigned int GetNumVertexBuffers();
	unsigned int GetNumIndexBuffers();

	//--------------------------------------------------------------------
	//	CreateVertexBuffer
	//--------------------------------------------------------------------
	void CreateVertexBuffer(	int i_nBufferSize,
								DWORD i_VertexShader,
								g2dD3D11VertexBufferPtr& io_pVertexBuffer,
								bool i_bMorphable,
								void* i_VertexData = NULL,
								int i_VertexDataSize = 0);

	//--------------------------------------------------------------------
	//	ReleaseVertexBuffer
	//--------------------------------------------------------------------
	void ReleaseVertexBuffer(g2dD3D11VertexBufferPtr i_pVertexBuffer);

	//--------------------------------------------------------------------
	//	LockVertexBuffer
	//--------------------------------------------------------------------
	BYTE* LockVertexBuffer( g2dD3D11VertexBufferPtr& io_pVertexBuffer,
							bool i_bMorphable  );

	//--------------------------------------------------------------------
	//	UnlockVertexBuffer
	//--------------------------------------------------------------------
	void UnlockVertexBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer );

	//--------------------------------------------------------------------
	//	GetVertexBufferSize
	//--------------------------------------------------------------------
	UINT GetVertexBufferSize( g2dD3D11VertexBufferPtr i_pVertexBuffer );

	//--------------------------------------------------------------------
	//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are 
	//	needed, creates the appropriate format of index buffer
	//	and returns the total size of the buffer in bytes.
	//--------------------------------------------------------------------
	int CreateAndFillIndexBuffer(	const g3dIndexPtr& i_Indices,
									int i_NumIndices,
									int i_NumVertices,
									g2dD3D11IndexBufferPtr& io_pIndexBuffer,
									bool i_bMorphable = false );

	//--------------------------------------------------------------------
	//	ReleaseIndexBuffer
	//--------------------------------------------------------------------
	void ReleaseIndexBuffer(g2dD3D11IndexBufferPtr i_pIndexBuffer);

	//--------------------------------------------------------------------
	//	LockIndexBuffer
	//--------------------------------------------------------------------
	BYTE* LockIndexBuffer(	g2dD3D11IndexBufferPtr& io_pIndexBuffer,
												bool i_bMorphable  );

	//--------------------------------------------------------------------
	//	UnlockIndexBuffer
	//--------------------------------------------------------------------
	void UnlockIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer );

	//--------------------------------------------------------------------
	//	GetIndexBufferSize
	//--------------------------------------------------------------------
	UINT GetIndexBufferSize( g2dD3D11IndexBufferPtr i_pIndexBuffer );

	//--------------------------------------------------------------------
	//	CreateBufferCopy
	//--------------------------------------------------------------------
	BYTE* CreateBufferCopy( int i_nBufferSize, const BYTE* i_pBuffer );
	BYTE* CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices, 
		int i_indexSize, int i_nIndices );
}