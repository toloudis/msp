/*****************************************************************************
**  g3dVkBufferUtil.cpp
**
**      See g3dVkBufferUtil.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif

#include <cstring>

namespace gvk
{

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

//====================================================================
// g3dVkBuffer
//====================================================================
g3dVkBuffer::g3dVkBuffer(Kind i_Kind, uint32_t i_Size, bool i_bDynamic, const void* i_pData, const char* i_pName)
:	m_Kind( i_Kind ),
	m_Size( i_Size ),
	m_bDynamic( i_bDynamic )
{
	if (m_bDynamic)
	{
		m_Cpu.resize(i_Size);
		if (i_pData)
		{
			memcpy(m_Cpu.data(), i_pData, i_Size);
		}
	}
	else
	{
		const VkBufferUsageFlags usage = (i_Kind == e_Vertex) ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		//	Vulkan has no zero-sized buffers; D3D11 refuses them too (DX11 asserts)
		m_pBuffer = vkdBuffer::Create(vkdGlobal::Device(), i_Size ? i_Size : 4, usage, i_pName);
		if (i_pData)
		{
			m_pBuffer->Upload(vkdGlobal::Context(), 0, i_pData, i_Size);
		}
	}
}

g3dVkBuffer::~g3dVkBuffer()
{
}

uint8_t* g3dVkBuffer::Map()
{
	if (!m_bDynamic)
	{
		DBG_ASSERT( false, "Error locking a default-usage buffer (D3D11 can't map one)" );
		return NULL;
	}
	return m_Cpu.data();
}

void g3dVkBuffer::Unmap()
{
	if (m_bDynamic)
	{
		m_bDirty = true;
	}
}

void g3dVkBuffer::GetBinding(vkdContext& i_Context, VkBuffer& o_Buffer, VkDeviceSize& o_Offset)
{
	if (!m_bDynamic)
	{
		o_Buffer = m_pBuffer->Buffer();
		o_Offset = 0;
		return;
	}
	//	Written to the ring after a change, and again for a later submission
	//	(the memory of the last upload may have been reused since)
	const uint64_t pending = vkdGlobal::Device().PendingValue();
	if (m_bDirty || m_UploadedFor != pending || m_RingBuffer == VK_NULL_HANDLE)
	{
		const vkdRingBuffer::Allocation allocation = i_Context.Upload(m_Cpu.data(), m_Size ? m_Size : 4, 16);
		m_RingBuffer = allocation.m_Buffer;
		m_RingOffset = allocation.m_Offset;
		m_UploadedFor = pending;
		m_bDirty = false;
	}
	o_Buffer = m_RingBuffer;
	o_Offset = m_RingOffset;
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
void g3dVkBufferUtil::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	l_bGeometryInVideoMemory = i_bVideoMem;
}

//--------------------------------------------------------------------
// Get current video mem usage totals
//--------------------------------------------------------------------
float g3dVkBufferUtil::GetTotalVertexBufferMemory()
{
	return (float)(l_TotalVertexBufferMemory)/1024.0f;
}
float g3dVkBufferUtil::GetTotalIndexBufferMemory()
{
	return (float)(l_TotalIndexBufferMemory)/1024.0f;
}
unsigned int g3dVkBufferUtil::GetNumVertexBuffers()
{
	return l_NumVertexBuffers;
}
unsigned int g3dVkBufferUtil::GetNumIndexBuffers()
{
	return l_NumIndexBuffers;
}

//--------------------------------------------------------------------
//	CreateVertexBuffer
//--------------------------------------------------------------------
void g3dVkBufferUtil::CreateVertexBuffer(	int i_nBufferSize,
											uint32_t /*i_VertexShader*/,
											g2dVkVertexBufferPtr& io_pVertexBuffer,
											bool i_bMorphable,
											void* i_VertexData /*= NULL*/,
											int /*i_VertexDataSize = 0*/)
{
	// dynamic (written with WRITE_DISCARD) when morphable, or when geometry
	// is not kept in video memory
	bool dynamic = i_bMorphable;
	if (!l_bGeometryInVideoMemory)
	{
		dynamic = true;
	}

	io_pVertexBuffer = new g3dVkBuffer(g3dVkBuffer::e_Vertex, (uint32_t)i_nBufferSize, dynamic, i_VertexData, "tmesh vertex buffer");

	l_TotalVertexBufferMemory += i_nBufferSize;
	l_NumVertexBuffers++;
}

//--------------------------------------------------------------------
//	ReleaseVertexBuffer
//--------------------------------------------------------------------
void g3dVkBufferUtil::ReleaseVertexBuffer(g2dVkVertexBufferPtr i_pVertexBuffer)
{
	if (i_pVertexBuffer != NULL)
	{
		l_TotalVertexBufferMemory -= i_pVertexBuffer->GetSize();
		l_NumVertexBuffers--;
		delete i_pVertexBuffer;
	}
}

//--------------------------------------------------------------------
//	LockVertexBuffer
//--------------------------------------------------------------------
uint8_t* g3dVkBufferUtil::LockVertexBuffer(	g2dVkVertexBufferPtr& io_pVertexBuffer,
											bool /*i_bMorphable*/  )
{
	// DX11: WRITE_DISCARD for a morphable buffer, READ_WRITE otherwise
	// (which D3D11 refuses for a default-usage buffer)
	return io_pVertexBuffer ? io_pVertexBuffer->Map() : NULL;
}

//--------------------------------------------------------------------
//	UnlockVertexBuffer
//--------------------------------------------------------------------
void g3dVkBufferUtil::UnlockVertexBuffer( g2dVkVertexBufferPtr i_pVertexBuffer )
{
	if (i_pVertexBuffer)
		i_pVertexBuffer->Unmap();
}

//--------------------------------------------------------------------
//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are
//	needed, creates the appropriate format of index buffer
//	and returns the total size of the buffer in bytes.
//--------------------------------------------------------------------
int g3dVkBufferUtil::CreateAndFillIndexBuffer(	const g3dIndexPtr& i_Indices,
								int i_NumIndices,
								int i_NumVertices,
								g2dVkIndexBufferPtr& io_pIndexBuffer,
								bool i_bMorphable )
{
	bool dynamic = i_bMorphable;
	if (!l_bGeometryInVideoMemory)
	{
		dynamic = true;
	}

	bool bNeed32Bit = g3dIndexPtr::Needs32Bit(i_NumVertices);
	int nSizeOfIndex = bNeed32Bit? 4 : 2;
	int nBufferSize = i_NumIndices * nSizeOfIndex;

	// get the system mem data ptr squared away.
	const void* data = NULL;
	uint16_t* buffer_16 = NULL;
	if (i_Indices.Is32BitIndices())
	{
		if (bNeed32Bit)
		{
			// Need 32-bit and have 32-bit
			data = i_Indices.GetIndices32();
		}
		else
		{
			// Have 32-bit, but only need 16-bit
			buffer_16 = new uint16_t[i_NumIndices];
			for (int i=0; i<i_NumIndices; i++)
			{
				buffer_16[i] = i_Indices.GetIndices32()[i];
			}
			data = buffer_16;
		}
	}
	else
	{
		// If we only have 16-bit vertices, there shouldn't be a case where
		// we needed 32-bit. That would mean that the given indices can't
		// handle the vertices.
		DBG_ASSERT(!bNeed32Bit, "Need 32-bit indices, but are only given 16-bit");
		data = i_Indices.GetIndices16();
	}

	io_pIndexBuffer = new g3dVkBuffer(g3dVkBuffer::e_Index, (uint32_t)nBufferSize, dynamic, data, "tmesh index buffer");

	if (buffer_16)
		delete [] buffer_16;
	return nBufferSize;
}

//--------------------------------------------------------------------
//	ReleaseIndexBuffer
//--------------------------------------------------------------------
void g3dVkBufferUtil::ReleaseIndexBuffer(g2dVkIndexBufferPtr i_pIndexBuffer)
{
	if (i_pIndexBuffer != NULL)
	{
		l_TotalIndexBufferMemory -= i_pIndexBuffer->GetSize();
		l_NumIndexBuffers--;
		delete i_pIndexBuffer;
	}
}

//--------------------------------------------------------------------
//	LockIndexBuffer
//--------------------------------------------------------------------
uint8_t* g3dVkBufferUtil::LockIndexBuffer(	g2dVkIndexBufferPtr& io_pIndexBuffer,
											bool /*i_bMorphable*/  )
{
	// DX11: WRITE_DISCARD for a morphable buffer, READ otherwise
	return io_pIndexBuffer ? io_pIndexBuffer->Map() : NULL;
}

//--------------------------------------------------------------------
//	UnlockIndexBuffer
//--------------------------------------------------------------------
void g3dVkBufferUtil::UnlockIndexBuffer( g2dVkIndexBufferPtr i_pIndexBuffer )
{
	if (i_pIndexBuffer)
		i_pIndexBuffer->Unmap();
}

//--------------------------------------------------------------------
//	CreateBufferCopy
//--------------------------------------------------------------------
uint8_t* g3dVkBufferUtil::CreateBufferCopy( int i_nBufferSize, const uint8_t* i_pBuffer )
{
	uint8_t* buffer_copy = new uint8_t[ i_nBufferSize ];
	memcpy( buffer_copy, i_pBuffer, i_nBufferSize );
	return buffer_copy;
}

uint8_t* g3dVkBufferUtil::CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices, int i_indexSize, int i_nIndices )
{
	uint8_t* buffer_copy = new uint8_t[ i_nBufferSize ];
	uint16_t* indices16 = reinterpret_cast<uint16_t*>(buffer_copy);
	uint32_t* indices32 = reinterpret_cast<uint32_t*>(buffer_copy);
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
uint32_t g3dVkBufferUtil::GetIndexBufferSize( g2dVkIndexBufferPtr i_pIndexBuffer )
{
	return i_pIndexBuffer ? i_pIndexBuffer->GetSize() : 0;
}

//--------------------------------------------------------------------
//	GetVertexBufferSize
//--------------------------------------------------------------------
uint32_t g3dVkBufferUtil::GetVertexBufferSize( g2dVkVertexBufferPtr i_pVertexBuffer )
{
	return i_pVertexBuffer ? i_pVertexBuffer->GetSize() : 0;
}

}
