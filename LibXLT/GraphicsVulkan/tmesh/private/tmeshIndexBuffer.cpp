/****************************************************************************\
**	tmeshIndexBuffer.cpp
**
**	See tmeshIndexBuffer.hpp.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_TMESH_INDEXBUFFER_HPP
#include "GraphicsVulkan/tmesh/tmeshIndexBuffer.hpp"
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
tmeshIndexBuffer::tmeshIndexBuffer( bool i_bLockable )
:	m_nIndices( 0 ),
	m_bLockable( i_bLockable ),
	m_nNonShadowIndices( 0 ),
	m_SizeOfIndex(2),
	m_pIndexBuffer( NULL ),
	m_pIndexBufferCopy(NULL)
{
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
tmeshIndexBuffer::~tmeshIndexBuffer()
{
	if ( m_pIndexBuffer != NULL )
	{
		// DX11 releases the buffer itself, not through
		// g3dDX11BufferUtil::ReleaseIndexBuffer, so the resource counter's
		// index buffer totals don't change (nor do they when it's made)
		delete m_pIndexBuffer;
	}
	delete[] m_pIndexBufferCopy;
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void tmeshIndexBuffer::Deallocate()
{
	// as DX11, not through ReleaseIndexBuffer (see the destructor)
	delete m_pIndexBuffer;
	// DX11 keeps the released pointer (and releases it again later); a
	// deleted buffer must not be used again here
	m_pIndexBuffer = NULL;
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void tmeshIndexBuffer::Reallocate()
{
	g2dVkIndexBufferPtr index_buffer;

	// Create the index buffer. DX11 passes a bool where the vertex count
	// goes, which gives 16-bit indices whatever the size; the vertex count
	// here keeps the size the buffer had.
	if (GetSizeOfIndex() == sizeof(uint16_t))
	{
		g3dVkBufferUtil::CreateAndFillIndexBuffer( reinterpret_cast<uint16_t*>( m_pIndexBufferCopy ),
													GetNumIndices(),
													0,
													index_buffer,
													this->GetLockable() );
	}
	else
	{
		DBG_ASSERT(GetSizeOfIndex() == sizeof(uint32_t), "Unrecognized size of index");
		g3dVkBufferUtil::CreateAndFillIndexBuffer( reinterpret_cast<uint32_t*>( m_pIndexBufferCopy ),
													GetNumIndices(),
													0x10000 + 1,
													index_buffer,
													this->GetLockable() );
	}

	SetIndexBuffer( index_buffer, m_pIndexBufferCopy, GetNumIndices(), GetSizeOfIndex() );

}


//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
unsigned char* tmeshIndexBuffer::LockIndices()
{
	DBG_ASSERT( m_pIndexBufferCopy, "IndexBufferCopy is NULL in LockIndices()" );

	return m_pIndexBufferCopy;
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
void tmeshIndexBuffer::UnlockIndices()
{
	// Lock the index buffer
	uint8_t* ibuffer_mem = g3dVkBufferUtil::LockIndexBuffer( m_pIndexBuffer, this->GetLockable() );

	// Fill the index buffer
	const int index_stride = this->GetSizeOfIndex();
	if (ibuffer_mem)
		memcpy( ibuffer_mem, m_pIndexBufferCopy, m_nIndices * index_stride );

	// Unlock the index buffer
	g3dVkBufferUtil::UnlockIndexBuffer( m_pIndexBuffer );
}

//--------------------------------------------------------------------
//  ReadOnlyLock - get read-only access to index buffer data.
//	Can be called on non-lockable buffers.
//--------------------------------------------------------------------
unsigned char* tmeshIndexBuffer::ReadOnlyLockIndices() const
{
	// if there's a sys mem copy of the vtx or index buffer, we should use it!
	uint8_t* pIndexBuffer = NULL;
	if (m_pIndexBufferCopy == NULL)
	{
		g2dVkIndexBufferPtr pIndexBuffer2 = m_pIndexBuffer;
		// We are just reading some data out.
		// Do not pass morphable flag here because we do not want to discard!
		pIndexBuffer = g3dVkBufferUtil::LockIndexBuffer(pIndexBuffer2, false);
	}
	else
	{
		pIndexBuffer = m_pIndexBufferCopy;
	}
	return pIndexBuffer;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - closes read-only access from ReadOnlyLock().
//	Can be called on non-lockable buffers.
//--------------------------------------------------------------------
void tmeshIndexBuffer::ReadOnlyUnlockIndices() const
{
	if (m_pIndexBufferCopy == NULL)
	{
		g3dVkBufferUtil::UnlockIndexBuffer(m_pIndexBuffer);
	}
}

//--------------------------------------------------------------------
//  SetIndexBuffer
//--------------------------------------------------------------------
void tmeshIndexBuffer::SetIndexBuffer( g2dVkIndexBufferPtr i_pIndexBuffer,
									 uint8_t* i_pIndexBufferCopy,
									 int i_nIndices,
									 int i_SizeOfIndex,
									 int i_nNonShadowIndices )
{
	m_pIndexBuffer = i_pIndexBuffer;
	m_pIndexBufferCopy = i_pIndexBufferCopy;
	m_nIndices = i_nIndices;
	m_SizeOfIndex = i_SizeOfIndex;

	// number of indices that are not used for shadow welding.  Default of
	// -1 means all indices are valid.
	m_nNonShadowIndices = (i_nNonShadowIndices < 0) ? i_nIndices : i_nNonShadowIndices;
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this fragment
//----------------------------------------------------------------------------
unsigned int tmeshIndexBuffer::GetSize() const
{
	return g3dVkBufferUtil::GetIndexBufferSize(m_pIndexBuffer);
}

}
