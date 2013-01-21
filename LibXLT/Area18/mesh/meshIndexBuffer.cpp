/****************************************************************************\
**	meshIndexBuffer.hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/mesh/meshIndexBuffer.hpp"

#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglBuffer.hpp"
#include "Area18/ogl/oglBufferUtil.hpp"
#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
meshIndexBuffer::meshIndexBuffer( bool i_bLockable )
:	m_nIndices( 0 ),
	m_bLockable( i_bLockable ),
	m_nNonShadowIndices( 0 ),
	m_SizeOfIndex(2),
	m_pIndexBufferCopy(NULL)
{
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
meshIndexBuffer::~meshIndexBuffer()
{
	m_pIndexBuffer.reset();
	delete[] m_pIndexBufferCopy;
}
//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void meshIndexBuffer::Deallocate()
{
	m_pIndexBuffer.reset();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void meshIndexBuffer::Reallocate()
{
	oglBufferHandle index_buffer;

	// Create the index buffer
	if (GetSizeOfIndex() == sizeof(envType::UInt16))
	{
		bool bNeed32Bit = false;
		oglBufferUtil::CreateAndFillIndexBuffer( m_pDevice,
			reinterpret_cast<envType::UInt16*>( m_pIndexBufferCopy ),
			GetNumIndices(),
			bNeed32Bit,
			index_buffer,
			this->GetLockable() );
	}
	else
	{
		DBG_ASSERT(GetSizeOfIndex() == sizeof(envType::UInt32), "Unrecognized size of index");
		bool bNeed32Bit = true;
		oglBufferUtil::CreateAndFillIndexBuffer( m_pDevice,
			reinterpret_cast<envType::UInt32*>( m_pIndexBufferCopy ),
			GetNumIndices(),
			bNeed32Bit,
			index_buffer,
			this->GetLockable() );
	}

	SetIndexBuffer( m_pDevice, index_buffer, m_pIndexBufferCopy, GetNumIndices(), GetSizeOfIndex() );

}


//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
unsigned char* meshIndexBuffer::LockIndices()
{
	//DBG_ASSERT( this->GetLockable(), "Only lockable buffers can have the indices locked" );
	DBG_ASSERT( m_pIndexBufferCopy, "IndexBufferCopy is NULL in LockIndices()" );

	return m_pIndexBufferCopy;
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
void meshIndexBuffer::UnlockIndices()
{
	// Lock the index buffer
	BYTE* ibuffer_mem = oglBufferUtil::LockIndexBuffer( m_pIndexBuffer, this->GetLockable() );

	// Fill the index buffer
	const int index_stride = this->GetSizeOfIndex();
	memcpy( ibuffer_mem, m_pIndexBufferCopy, m_nIndices * index_stride );

	// Unlock the index buffer
	oglBufferUtil::UnlockIndexBuffer( m_pIndexBuffer );
}

//--------------------------------------------------------------------
//  ReadOnlyLock - get read-only access to index buffer data.
//	Can be called on non-lockable buffers.
//--------------------------------------------------------------------
unsigned char* meshIndexBuffer::ReadOnlyLockIndices() const
{
	// if there's a sys mem copy of the vtx or index buffer, we should use it!
	BYTE* pIndexBuffer = NULL;
	if (m_pIndexBufferCopy == NULL)
	{
		oglBufferHandle pD3DIndexBuffer = m_pIndexBuffer;
		// We are just reading some data out.
		// Do not pass morphable flag here because we do not want to discard!  
		pIndexBuffer = oglBufferUtil::LockIndexBuffer(pD3DIndexBuffer, false);//this->GetMorphable());
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
void meshIndexBuffer::ReadOnlyUnlockIndices() const
{
	if (m_pIndexBufferCopy == NULL)
	{
		oglBufferUtil::UnlockIndexBuffer(m_pIndexBuffer);
	}
}

//--------------------------------------------------------------------
//  SetIndexBuffer
//--------------------------------------------------------------------
void meshIndexBuffer::SetIndexBuffer( oglDevice* i_pDevice,
									 oglBufferHandle i_pIndexBuffer,
									 BYTE* i_pIndexBufferCopy,
									 int i_nIndices,
									 int i_SizeOfIndex,
									 int i_nNonShadowIndices )
{
	m_pDevice = i_pDevice;
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
unsigned int meshIndexBuffer::GetSize() const
{
	return oglBufferUtil::GetIndexBufferSize(m_pIndexBuffer);
}
