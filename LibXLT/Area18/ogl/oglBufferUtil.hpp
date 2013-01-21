/*****************************************************************************
**  g3dDX11BufferUtil.hpp
**
**      g3dDX11BufferUtil has functions for dealing with vertex and index
**		buffers
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#pragma once

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif
#include "Area18/ogl/oglTypes.hpp"
#include "Area18/ogl/oglBufferHandle.h"

class oglDevice;
class oglBuffer;

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

namespace oglBufferUtil
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
	void CreateVertexBuffer(oglDevice* i_pDevice,
		int i_nBufferSize,
		DWORD i_VertexShader,
		oglBufferHandle& io_pVertexBuffer,
		bool i_bMorphable,
		void* i_VertexData = NULL,
		int i_VertexDataSize = 0);

	//--------------------------------------------------------------------
	//	ReleaseVertexBuffer
	//--------------------------------------------------------------------
	void ReleaseVertexBuffer(oglDevice* i_pDevice,
		oglBufferHandle i_pVertexBuffer);

	//--------------------------------------------------------------------
	//	LockVertexBuffer
	//--------------------------------------------------------------------
	BYTE* LockVertexBuffer( oglBufferHandle io_pVertexBuffer,
							bool i_bMorphable  );

	//--------------------------------------------------------------------
	//	UnlockVertexBuffer
	//--------------------------------------------------------------------
	void UnlockVertexBuffer( oglBufferHandle i_pVertexBuffer );

	//--------------------------------------------------------------------
	//	GetVertexBufferSize
	//--------------------------------------------------------------------
	UINT GetVertexBufferSize( oglBufferHandle i_pVertexBuffer );

	//--------------------------------------------------------------------
	//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are 
	//	needed, creates the appropriate format of index buffer
	//	and returns the total size of the buffer in bytes.
	//--------------------------------------------------------------------
	int CreateAndFillIndexBuffer(	oglDevice* i_pDevice,
		const g3dIndexPtr& i_Indices,
		int i_NumIndices,
		int i_NumVertices,
		oglBufferHandle& io_pIndexBuffer,
		bool i_bMorphable = false );

	//--------------------------------------------------------------------
	//	ReleaseIndexBuffer
	//--------------------------------------------------------------------
	void ReleaseIndexBuffer(oglDevice* i_pDevice, oglBufferHandle i_pIndexBuffer);

	//--------------------------------------------------------------------
	//	LockIndexBuffer
	//--------------------------------------------------------------------
	BYTE* LockIndexBuffer(	oglBufferHandle io_pIndexBuffer,
												bool i_bMorphable  );

	//--------------------------------------------------------------------
	//	UnlockIndexBuffer
	//--------------------------------------------------------------------
	void UnlockIndexBuffer( oglBufferHandle i_pIndexBuffer );

	//--------------------------------------------------------------------
	//	GetIndexBufferSize
	//--------------------------------------------------------------------
	UINT GetIndexBufferSize( oglBufferHandle i_pIndexBuffer );

	//--------------------------------------------------------------------
	//	CreateBufferCopy
	//--------------------------------------------------------------------
	BYTE* CreateBufferCopy( int i_nBufferSize, const BYTE* i_pBuffer );
	BYTE* CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices, 
		int i_indexSize, int i_nIndices );
}
