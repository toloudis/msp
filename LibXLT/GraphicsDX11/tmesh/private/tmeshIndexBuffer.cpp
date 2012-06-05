/****************************************************************************\
**	tmeshIndexBuffer.hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/tmesh/tmeshIndexBuffer.hpp"

#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"
#include "Core/dbg/dbgMsg.hpp"

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
		ULONG ref_count = m_pIndexBuffer->Release();
		if( ref_count > 0 )
		{
			DBG_WARNING("Error releasing index buffer, ref_count " << ref_count);
		}
	}
	delete[] m_pIndexBufferCopy;
}
//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void tmeshIndexBuffer::Deallocate()
{
	m_pIndexBuffer->Release();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void tmeshIndexBuffer::Reallocate()
{
	g2dD3D11IndexBufferPtr index_buffer;

	// Create the index buffer
	if (GetSizeOfIndex() == sizeof(envType::UInt16))
	{
		bool bNeed32Bit = false;
		g3dDX11BufferUtil::CreateAndFillIndexBuffer( reinterpret_cast<envType::UInt16*>( m_pIndexBufferCopy ),
													GetNumIndices(),
													bNeed32Bit,
													index_buffer,
													this->GetLockable() );
	}
	else
	{
		DBG_ASSERT(GetSizeOfIndex() == sizeof(envType::UInt32), "Unrecognized size of index");
		bool bNeed32Bit = true;
		g3dDX11BufferUtil::CreateAndFillIndexBuffer( reinterpret_cast<envType::UInt32*>( m_pIndexBufferCopy ),
													GetNumIndices(),
													bNeed32Bit,
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
	//DBG_ASSERT( this->GetLockable(), "Only lockable buffers can have the indices locked" );
	DBG_ASSERT( m_pIndexBufferCopy, "IndexBufferCopy is NULL in LockIndices()" );

	return m_pIndexBufferCopy;
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
void tmeshIndexBuffer::UnlockIndices()
{
	// Lock the index buffer
	BYTE* ibuffer_mem = g3dDX11BufferUtil::LockIndexBuffer( m_pIndexBuffer, this->GetLockable() );

	// Fill the index buffer
	const int index_stride = this->GetSizeOfIndex();
	memcpy( ibuffer_mem, m_pIndexBufferCopy, m_nIndices * index_stride );

	// Unlock the index buffer
	g3dDX11BufferUtil::UnlockIndexBuffer( m_pIndexBuffer );
}

//--------------------------------------------------------------------
//  ReadOnlyLock - get read-only access to index buffer data.
//	Can be called on non-lockable buffers.
//--------------------------------------------------------------------
unsigned char* tmeshIndexBuffer::ReadOnlyLockIndices() const
{
	// if there's a sys mem copy of the vtx or index buffer, we should use it!
	BYTE* pIndexBuffer = NULL;
	if (m_pIndexBufferCopy == NULL)
	{
		g2dD3D11IndexBufferPtr pD3DIndexBuffer = m_pIndexBuffer;
		// We are just reading some data out.
		// Do not pass morphable flag here because we do not want to discard!  
		pIndexBuffer = g3dDX11BufferUtil::LockIndexBuffer(pD3DIndexBuffer, false);//this->GetMorphable());
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
		g3dDX11BufferUtil::UnlockIndexBuffer(m_pIndexBuffer);
	}
}

//--------------------------------------------------------------------
//  SetIndexBuffer
//--------------------------------------------------------------------
void tmeshIndexBuffer::SetIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer,
									 BYTE* i_pIndexBufferCopy,
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
	return g3dDX11BufferUtil::GetIndexBufferSize(m_pIndexBuffer);
}
