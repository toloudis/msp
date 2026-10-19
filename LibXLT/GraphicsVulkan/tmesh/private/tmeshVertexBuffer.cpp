/****************************************************************************\
**	tmeshVertexBuffer.cpp
**
**	See tmeshVertexBuffer.hpp.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_TMESH_VERTEXBUFFER_HPP
#include "GraphicsVulkan/tmesh/tmeshVertexBuffer.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <cstring>

namespace gvk
{

//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
tmeshVertexBuffer::tmeshVertexBuffer(g3dType::VertexFormat i_VertexFormat,
									 int i_nVertexStride,
									 bool i_bLockable )
:	m_nVertices( 0 ),
	m_bLockable( i_bLockable ),
	m_VertexFormat( i_VertexFormat ),
	m_nVertexStride( i_nVertexStride ),
	m_pVertexBuffer( NULL ),
	m_pVertexBufferCopy( NULL ),
	m_UVOverlapping( 0.0f)
{
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
tmeshVertexBuffer::~tmeshVertexBuffer()
{
	if (m_pVertexBuffer != NULL)
	{
		g3dVkBufferUtil::ReleaseVertexBuffer(m_pVertexBuffer);
	}
	delete[] m_pVertexBufferCopy;
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void tmeshVertexBuffer::Deallocate()
{
	g3dVkBufferUtil::ReleaseVertexBuffer(m_pVertexBuffer);
	// DX11 keeps the released pointer (and releases it again later); a
	// deleted buffer must not be used again here
	m_pVertexBuffer = NULL;
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void tmeshVertexBuffer::Reallocate()
{
	int nVertexBufferSize = GetNumVertices() * GetVertexStride();

	g2dVkVertexBufferPtr vertex_buffer;

	// Create the vertex buffer
	g3dVkBufferUtil::CreateVertexBuffer(	nVertexBufferSize,
											0/*GetVertexShader()*/,
											vertex_buffer,
											GetLockable() );

	SetVertexBuffer( vertex_buffer, m_pVertexBufferCopy, GetNumVertices() );

	// Update the vertex buffer
	Unlock();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat tmeshVertexBuffer::GetVertexFormat() const
{
	return m_VertexFormat;
}

//--------------------------------------------------------------------
//  Lock
//--------------------------------------------------------------------
unsigned char* tmeshVertexBuffer::Lock()
{
	DBG_ASSERT( this->GetLockable(), "Only lockable buffers can be locked" );
	DBG_ASSERT( m_pVertexBufferCopy, "VertexBufferCopy is NULL in Lock()" );

	return m_pVertexBufferCopy;
}

//--------------------------------------------------------------------
//  Unlock
//--------------------------------------------------------------------
void tmeshVertexBuffer::Unlock()
{
	// Lock the vertex buffer
	uint8_t* vbuffer_mem = g3dVkBufferUtil::LockVertexBuffer( m_pVertexBuffer, this->GetLockable() );

	// Fill the vertex buffer
	if (m_pVertexBufferCopy && vbuffer_mem)
		memcpy( vbuffer_mem, m_pVertexBufferCopy, m_nVertices * m_nVertexStride );

	// Unlock the vertex buffer
	if (vbuffer_mem)
		g3dVkBufferUtil::UnlockVertexBuffer( m_pVertexBuffer );

}

//--------------------------------------------------------------------
//  ReadOnlyLock - get read-only access to vertex buffer data
//--------------------------------------------------------------------
unsigned char* tmeshVertexBuffer::ReadOnlyLock() const
{
	uint8_t* pVertexBuffer = NULL;
	if (m_pVertexBufferCopy == NULL)
	{
		g2dVkVertexBufferPtr pVertexBuffer2 = m_pVertexBuffer;
		// We are just reading some data out.
		// Do not pass morphable flag here because we do not want to discard!
		pVertexBuffer = g3dVkBufferUtil::LockVertexBuffer(pVertexBuffer2, false);
	}
	else
	{
		pVertexBuffer = m_pVertexBufferCopy;
	}
	return pVertexBuffer;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - closes read-only access from ReadOnlyLock()
//--------------------------------------------------------------------
void tmeshVertexBuffer::ReadOnlyUnlock() const
{
	if (m_pVertexBufferCopy == NULL)
	{
		g3dVkBufferUtil::UnlockVertexBuffer(m_pVertexBuffer);
	}
}

//--------------------------------------------------------------------
//  SetVertexBuffer
//--------------------------------------------------------------------
void tmeshVertexBuffer::SetVertexBuffer( g2dVkVertexBufferPtr i_pVertexBuffer,
									  uint8_t* i_pVertexBufferCopy,
									  int i_nVertices )
{
	m_pVertexBuffer = i_pVertexBuffer;
	m_pVertexBufferCopy = i_pVertexBufferCopy;
	m_nVertices = i_nVertices;
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this buffer
//----------------------------------------------------------------------------
unsigned int tmeshVertexBuffer::GetSize() const
{
	return g3dVkBufferUtil::GetVertexBufferSize(m_pVertexBuffer);
}

//----------------------------------------------------------------------------
//	BoundingBox
//----------------------------------------------------------------------------
const maAxisBox& tmeshVertexBuffer::GetBoundingBox() const
{
	return m_BoundingBox;
}
void tmeshVertexBuffer::SetBoundingBox( const maAxisBox& i_BoundingBox )
{
	m_BoundingBox = i_BoundingBox;
}


//----------------------------------------------------------------------------
//	Return uv scaling factors to keep uvs within 0..1
//----------------------------------------------------------------------------
void tmeshVertexBuffer::GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const
{
	o_Scale = m_BakeUVScale;
	o_Translate = m_BakeUVTranslate;
}
void tmeshVertexBuffer::SetUVBakeFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate)
{
	m_BakeUVScale = i_Scale;
	m_BakeUVTranslate = i_Translate;
}

//----------------------------------------------------------------------------
//	Return uv overlapping factor
//----------------------------------------------------------------------------
void tmeshVertexBuffer::SetUVOverlapFactor(float i_overlap)
{
	m_UVOverlapping = i_overlap;
}

float tmeshVertexBuffer::GetUVOverlapFactor()
{
	return m_UVOverlapping;
}

}
