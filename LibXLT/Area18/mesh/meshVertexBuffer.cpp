/****************************************************************************\
**	meshVertexBuffer.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/mesh/meshVertexBuffer.hpp"

#include "Area18/ogl/oglBufferUtil.hpp"

//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
meshVertexBuffer::meshVertexBuffer(g3dType::VertexFormat i_VertexFormat,
									 int i_nVertexStride,
									 bool i_bLockable )
:	m_nVertices( 0 ),
	m_bLockable( i_bLockable ),
	m_VertexFormat( i_VertexFormat ),
	m_nVertexStride( i_nVertexStride ),
	m_pVertexBufferCopy( NULL ),
	m_UVOverlapping( 0.0f)
{
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
meshVertexBuffer::~meshVertexBuffer()
{
	if (m_pVertexBuffer != NULL)
	{
		oglBufferUtil::ReleaseVertexBuffer(m_pDevice, m_pVertexBuffer);
	}
	delete[] m_pVertexBufferCopy;
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void meshVertexBuffer::Deallocate()
{
	oglBufferUtil::ReleaseVertexBuffer(m_pDevice, m_pVertexBuffer);
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void meshVertexBuffer::Reallocate()
{
	int nVertexBufferSize = GetNumVertices() * GetVertexStride();

	oglBufferHandle vertex_buffer;

	// Create the vertex buffer
	oglBufferUtil::CreateVertexBuffer(	m_pDevice,
		nVertexBufferSize,
		0/*GetVertexShader()*/,
		vertex_buffer,
		GetLockable() );

	SetVertexBuffer( m_pDevice, vertex_buffer, m_pVertexBufferCopy, GetNumVertices() );

	// Update the vertex buffer
	Unlock();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat meshVertexBuffer::GetVertexFormat() const
{
	return m_VertexFormat;
}

//--------------------------------------------------------------------
//  Lock
//--------------------------------------------------------------------
unsigned char* meshVertexBuffer::Lock()
{
	DBG_ASSERT( this->GetLockable(), "Only lockable buffers can be locked" );
	DBG_ASSERT( m_pVertexBufferCopy, "VertexBufferCopy is NULL in Lock()" );

	return m_pVertexBufferCopy;
}

//--------------------------------------------------------------------
//  Unlock
//--------------------------------------------------------------------
void meshVertexBuffer::Unlock()
{
	// update the gpu vertices
//	g2dDX11Global::g_pDeviceContext->UpdateSubresource(
//		m_pVertexBuffer,
//		D3D11CalcSubresource(0,0,1),
//		NULL,
//		m_pVertexBufferCopy,
//		GetNumVertices() * GetVertexStride(),
//		GetNumVertices() * GetVertexStride()
//	);

	// Lock the vertex buffer
	BYTE* vbuffer_mem = oglBufferUtil::LockVertexBuffer( m_pVertexBuffer, this->GetLockable() );

	// Fill the vertex buffer
	if (m_pVertexBufferCopy && vbuffer_mem)
		memcpy( vbuffer_mem, m_pVertexBufferCopy, m_nVertices * m_nVertexStride );

	// Unlock the vertex buffer
	if (vbuffer_mem)
		oglBufferUtil::UnlockVertexBuffer( m_pVertexBuffer );

}

//--------------------------------------------------------------------
//  ReadOnlyLock - get read-only access to vertex buffer data
//--------------------------------------------------------------------
unsigned char* meshVertexBuffer::ReadOnlyLock() const
{
	BYTE* pVertexBuffer = NULL;
	if (m_pVertexBufferCopy == NULL)
	{
		oglBufferHandle pD3DVertexBuffer = m_pVertexBuffer;
		// We are just reading some data out.
		// Do not pass morphable flag here because we do not want to discard!  
		pVertexBuffer = oglBufferUtil::LockVertexBuffer(pD3DVertexBuffer, false);//this->GetMorphable());
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
void meshVertexBuffer::ReadOnlyUnlock() const
{
	if (m_pVertexBufferCopy == NULL)
	{
		oglBufferUtil::UnlockVertexBuffer(m_pVertexBuffer);
	}
}

//--------------------------------------------------------------------
//  SetVertexBuffer
//--------------------------------------------------------------------
void meshVertexBuffer::SetVertexBuffer( oglDevice* i_pDevice,
									   oglBufferHandle i_pVertexBuffer,
									   BYTE* i_pVertexBufferCopy,
									   int i_nVertices )
{
	m_pDevice = i_pDevice;
	m_pVertexBuffer = i_pVertexBuffer;
	m_pVertexBufferCopy = i_pVertexBufferCopy;
	m_nVertices = i_nVertices;
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this buffer
//----------------------------------------------------------------------------
unsigned int meshVertexBuffer::GetSize() const
{
	return oglBufferUtil::GetVertexBufferSize(m_pVertexBuffer);
}

//----------------------------------------------------------------------------
//	BoundingBox
//----------------------------------------------------------------------------
const maAxisBox& meshVertexBuffer::GetBoundingBox() const
{
	return m_BoundingBox;
}
void meshVertexBuffer::SetBoundingBox( const maAxisBox& i_BoundingBox )
{
	m_BoundingBox = i_BoundingBox;
}


//----------------------------------------------------------------------------
//	Return uv scaling factors to keep uvs within 0..1
//----------------------------------------------------------------------------
void meshVertexBuffer::GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const
{
	o_Scale = m_BakeUVScale;
	o_Translate = m_BakeUVTranslate;
}
void meshVertexBuffer::SetUVBakeFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate)
{
	m_BakeUVScale = i_Scale;
	m_BakeUVTranslate = i_Translate;
}

//----------------------------------------------------------------------------
//	Return uv overlapping factor
//----------------------------------------------------------------------------
void meshVertexBuffer::SetUVOverlapFactor(float i_overlap)
{
	m_UVOverlapping = i_overlap;
}

float meshVertexBuffer::GetUVOverlapFactor()
{
	return m_UVOverlapping;
}
