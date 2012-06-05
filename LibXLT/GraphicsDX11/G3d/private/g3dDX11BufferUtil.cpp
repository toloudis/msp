/*****************************************************************************
**  g3dDX11BufferUtil.cpp
**
**      g3dDX11BufferUtil has functions for dealing with vertex and index
**		buffers
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envThread.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

//--------------------------------------------------------------------
// Anonymous Namespace for local variables and functions
//--------------------------------------------------------------------
namespace
{
	bool l_bGeometryInVideoMemory = true;

	unsigned int l_TotalVertexBufferMemory = 0;
	unsigned int l_TotalIndexBufferMemory = 0;
	unsigned int l_NumVertexBuffers = 0;
	unsigned int l_NumIndexBuffers = 0;
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
void g3dDX11BufferUtil::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	l_bGeometryInVideoMemory = i_bVideoMem;
}

//--------------------------------------------------------------------
// Get current video mem usage totals
//--------------------------------------------------------------------
float g3dDX11BufferUtil::GetTotalVertexBufferMemory()
{
	return (float)(l_TotalVertexBufferMemory)/1024.0f;
}
float g3dDX11BufferUtil::GetTotalIndexBufferMemory()
{
	return (float)(l_TotalIndexBufferMemory)/1024.0f;
}
unsigned int g3dDX11BufferUtil::GetNumVertexBuffers()
{
	return l_NumVertexBuffers;
}
unsigned int g3dDX11BufferUtil::GetNumIndexBuffers()
{
	return l_NumIndexBuffers;
}

//--------------------------------------------------------------------
//	CreateVertexBuffer
//--------------------------------------------------------------------
void g3dDX11BufferUtil::CreateVertexBuffer(	int i_nBufferSize,
											DWORD i_VertexShader,
											g2dD3D11VertexBufferPtr& io_pVertexBuffer,
											bool i_bMorphable,
											void* i_VertexData /*= NULL*/,
											int i_VertexDataSize /*= 0*/)
{
	D3D11_USAGE usage = ( i_bMorphable ) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT /*D3D11_USAGE_IMMUTABLE*/;
	// no cpu read access for now?
	UINT cpuAccess = ( i_bMorphable ) ? D3D11_CPU_ACCESS_WRITE : 0; 
	if (!l_bGeometryInVideoMemory)
	{
		// are these correct?

		cpuAccess = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = i_nBufferSize;
    bufferDesc.Usage = usage;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.CPUAccessFlags = cpuAccess;
    bufferDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = i_VertexDataSize;

	// Create the index buffer
	D3D11_SUBRESOURCE_DATA data;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	data.pSysMem = i_VertexData;

	// Create the vertex buffer
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateBuffer(&bufferDesc, 
		i_VertexData ? (&data) : NULL, 
		&io_pVertexBuffer);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating vertex buffer" );
	}

	l_TotalVertexBufferMemory += i_nBufferSize;
//	DBG_LOG("[++]Adding vertex memory, " << l_TotalVertexBufferMemory << " total.");
	l_NumVertexBuffers++;
}

//--------------------------------------------------------------------
//	ReleaseVertexBuffer
//--------------------------------------------------------------------
void g3dDX11BufferUtil::ReleaseVertexBuffer(g2dD3D11VertexBufferPtr i_pVertexBuffer)
{
	if (i_pVertexBuffer != NULL)
	{
		// get the size.
		// alternative, could store the size as member var
		D3D11_BUFFER_DESC desc;
		i_pVertexBuffer->GetDesc(&desc);

		ULONG ref_count = i_pVertexBuffer->Release();
		if( ref_count > 0 )
		{
			DBG_WARNING("Error releasing vertex buffer, ref_count " << ref_count);
		}

		l_TotalVertexBufferMemory -= desc.ByteWidth;
//		DBG_LOG("[--]Remove vertex memory, " << l_TotalVertexBufferMemory << " remains.");
		l_NumVertexBuffers--;
	}
}
//--------------------------------------------------------------------
//	LockVertexBuffer
//--------------------------------------------------------------------
BYTE* g3dDX11BufferUtil::LockVertexBuffer(	g2dD3D11VertexBufferPtr& io_pVertexBuffer,
											bool i_bMorphable  )
{
	D3D11_MAP flags = ( i_bMorphable ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_READ_WRITE;

	// Lock the vertex buffer
	D3D11_MAPPED_SUBRESOURCE vbuffer_mem;

	HRESULT op_result = S_OK;
	{
		//envScopedLock lockDeviceContext(g2dDX11Global::g_D3DDeviceContextMutex);
		op_result = g2dDX11Global::g_pDeviceContext->Map(io_pVertexBuffer,0,
			flags, 0, 
			&vbuffer_mem);
	}
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error locking vertex buffer" );
	}

	return (BYTE*)vbuffer_mem.pData;
}

//--------------------------------------------------------------------
//	UnlockVertexBuffer
//--------------------------------------------------------------------
void g3dDX11BufferUtil::UnlockVertexBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer )
{
	//envScopedLock lockDeviceContext(g2dDX11Global::g_D3DDeviceContextMutex);
	// Unlock the vertex buffer
	g2dDX11Global::g_pDeviceContext->Unmap(i_pVertexBuffer, 0);
}

//--------------------------------------------------------------------
//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are 
//	needed, creates the appropriate format of index buffer
//	and returns the total size of the buffer in bytes.
//--------------------------------------------------------------------
int g3dDX11BufferUtil::CreateAndFillIndexBuffer(	const g3dIndexPtr& i_Indices,
								int i_NumIndices,
								int i_NumVertices,
								g2dD3D11IndexBufferPtr& io_pIndexBuffer,
								bool i_bMorphable )
{
	D3D11_USAGE usage = ( i_bMorphable ) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT /*D3D11_USAGE_IMMUTABLE*/;
	// no cpu read access for now?
	UINT cpuAccess = ( i_bMorphable ) ? D3D11_CPU_ACCESS_WRITE : 0; 
	if (!l_bGeometryInVideoMemory)
	{
		// are these correct?

		cpuAccess = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		usage = D3D11_USAGE_DYNAMIC;
	}

	// force 32 bit for shaderresrouceview (ray trace) reasons
	//bool bNeed32Bit = true;
	bool bNeed32Bit = g3dIndexPtr::Needs32Bit(i_NumVertices);
	int nSizeOfIndex = bNeed32Bit? 4 : 2;
	int nBufferSize = i_NumIndices * nSizeOfIndex;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = nBufferSize;
    bufferDesc.Usage = usage;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;// | D3D11_BIND_SHADER_RESOURCE; // Bind Shader only for ray tracing
    bufferDesc.CPUAccessFlags = cpuAccess;
    bufferDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = nSizeOfIndex;

	// Create the index buffer
	D3D11_SUBRESOURCE_DATA data;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	// get the system mem data ptr squared away.
	envType::UInt16* buffer_16 = NULL;
	envType::UInt32* buffer_32 = NULL;
	if (i_Indices.Is32BitIndices())
	{
		if (bNeed32Bit)
		{
			// Need 32-bit and have 32-bit
			data.pSysMem = i_Indices.GetIndices32();
		}
		else
		{
			// Have 32-bit, but only need 16-bit

			buffer_16 = new envType::UInt16[i_NumIndices];
			for (int i=0; i<i_NumIndices; i++)
			{
				buffer_16[i] = i_Indices.GetIndices32()[i];
			}
			data.pSysMem = buffer_16;
		}
	}
	else
	{
		// Have 16-bit, but need 32-bit

		/*buffer_32 = new envType::UInt32[i_NumIndices];
		for (int i=0; i<i_NumIndices; i++)
		{
			buffer_32[i] = i_Indices.GetIndices16()[i];
		}
		data.pSysMem = buffer_32;*/

		// If we only have 16-bit vertices, there shouldn't be a case where
		// we needed 32-bit. That would mean that the given indices can't
		// handle the vertices.
		DBG_ASSERT(!bNeed32Bit, "Need 32-bit indices, but are only given 16-bit");
		data.pSysMem = i_Indices.GetIndices16();
	}


	HRESULT op_result = g2dDX11Global::g_pDevice->CreateBuffer(&bufferDesc,
		&data,
		&io_pIndexBuffer);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating index buffer" );
	}

	if (buffer_16)
		delete [] buffer_16;
	if (buffer_32)
		delete [] buffer_32;
	return nBufferSize;
}

//--------------------------------------------------------------------
//	ReleaseIndexBuffer
//--------------------------------------------------------------------
void g3dDX11BufferUtil::ReleaseIndexBuffer(g2dD3D11IndexBufferPtr i_pIndexBuffer)
{
	if (i_pIndexBuffer != NULL)
	{
		// get the size.
		// alternative, could store the size as member var
		D3D11_BUFFER_DESC desc;
		i_pIndexBuffer->GetDesc(&desc);

		ULONG ref_count = i_pIndexBuffer->Release();
		if( ref_count > 0 )
		{
			DBG_WARNING("Error releasing vertex buffer, ref_count " << ref_count);
		}

		l_TotalIndexBufferMemory -= desc.ByteWidth;
		l_NumIndexBuffers--;
	}
}

//--------------------------------------------------------------------
//	LockIndexBuffer
//--------------------------------------------------------------------
BYTE* g3dDX11BufferUtil::LockIndexBuffer(	g2dD3D11IndexBufferPtr& io_pIndexBuffer,
											bool i_bMorphable  )
{
	D3D11_MAP flags = ( i_bMorphable ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_READ;

	// Lock the index buffer
	D3D11_MAPPED_SUBRESOURCE ibuffer_mem;
	HRESULT op_result = g2dDX11Global::g_pDeviceContext->Map(io_pIndexBuffer,0,
		flags, 0, 
		&ibuffer_mem);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error locking index buffer" );
	}

	return (BYTE*)ibuffer_mem.pData;
}

//--------------------------------------------------------------------
//	UnlockIndexBuffer
//--------------------------------------------------------------------
void g3dDX11BufferUtil::UnlockIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer )
{
	// Unlock the vertex buffer
	g2dDX11Global::g_pDeviceContext->Unmap(i_pIndexBuffer,0);
}

//--------------------------------------------------------------------
//	CreateBufferCopy
//--------------------------------------------------------------------
BYTE* g3dDX11BufferUtil::CreateBufferCopy( int i_nBufferSize, const BYTE* i_pBuffer )
{
	BYTE* buffer_copy = new BYTE[ i_nBufferSize ];
	memcpy( buffer_copy, i_pBuffer, i_nBufferSize );
	return buffer_copy;
}

BYTE* g3dDX11BufferUtil::CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices, int i_indexSize, int i_nIndices )
{
	BYTE* buffer_copy = new BYTE[ i_nBufferSize ];
	envType::UInt16* indices16 = reinterpret_cast<envType::UInt16*>(buffer_copy);
	envType::UInt32* indices32 = reinterpret_cast<envType::UInt32*>(buffer_copy);
	DBG_ASSERT(i_nBufferSize == i_indexSize * i_nIndices, "Bad inputs to CreateBufferCopy");
	if (i_Indices.Is32BitIndices())
	{
		// 32 bit
		if (i_indexSize == 4)
		{
			memcpy( buffer_copy, i_Indices.GetIndices32(), i_nBufferSize );
		}
		else
		{
			// type conversion - truncation may occur here if not careful
			//DBG_WARNING("Trying to copy 32 bit indices into 16 bit index buffer");
			for (int i = 0; i < i_nIndices; i++)
			{
				indices16[i] = i_Indices.GetIndex(i);
			}
		}
	}
	else 
	{
		// 16 bit
		if (i_indexSize == 2)
		{
			memcpy( buffer_copy, i_Indices.GetIndices16(), i_nBufferSize );
		}
		else
		{
			// type conversion
			for (int i = 0; i < i_nIndices; i++)
			{
				indices32[i] = i_Indices.GetIndex(i);
			}
		}
	}
	return buffer_copy;
}

//--------------------------------------------------------------------
//	GetIndexBufferSize
//--------------------------------------------------------------------
UINT g3dDX11BufferUtil::GetIndexBufferSize( g2dD3D11IndexBufferPtr i_pIndexBuffer )
{
	if (i_pIndexBuffer != NULL)
	{
		D3D11_BUFFER_DESC desc;
		i_pIndexBuffer->GetDesc(&desc);
		return desc.ByteWidth;
	}
	return 0;
}

//--------------------------------------------------------------------
//	GetVertexBufferSize
//--------------------------------------------------------------------
UINT g3dDX11BufferUtil::GetVertexBufferSize( g2dD3D11VertexBufferPtr i_pVertexBuffer )
{
	if (i_pVertexBuffer != NULL)
	{
		D3D11_BUFFER_DESC desc;
		i_pVertexBuffer->GetDesc(&desc);
		return desc.ByteWidth;
	}
	return 0;
}
