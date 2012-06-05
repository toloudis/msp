/****************************************************************************\
**	tmeshFrag.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/tmesh/tmeshFrag.hpp"

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "GraphicsDX11/bump/bumpBufferUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"

//============================================================================
//============================================================================

int tmeshFrag::sm_RendererId = 0;

//--------------------------------------------------------------------
// Set id for fragments in order to choose renderer
//--------------------------------------------------------------------
//static
void tmeshFrag::SetRendererId(int i_RenderMode)
{
	tmeshFrag::sm_RendererId = i_RenderMode;
}

//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
tmeshFrag::tmeshFrag(   matMaterial* i_pMaterial,
				        g3dType::VertexFormat i_VertexFormat,
						DWORD i_VertexShader,
						int i_nVertexStride,
						bool i_bMorphable,
						bool i_bComponentSort )
:	g3dFragment( i_pMaterial, tmeshFrag::sm_RendererId, i_bMorphable, i_bComponentSort ),
	m_pVertexBuffer( new tmeshVertexBuffer( i_VertexFormat, i_nVertexStride, i_bMorphable ) ),
	//m_pVertexBuffer_Old( new tmeshVertexBuffer( i_VertexFormat, i_nVertexStride, i_bMorphable ) ),
	m_pIndexBuffer( new tmeshIndexBuffer( i_bComponentSort ) ),
	m_nZBias( 0 ),
	m_VertexShader( i_VertexShader ),
	m_ePrimitiveType( e_TriangleList )
{
	// No, only fragments loaded from Maya files are on by default.
	// 3D Icons are often created as tmeshFrag's and shouldn't be 
	// casting shadows by default.
	//
	// tmesh fragments are involved in shadowing by default
	//this->SetCastsShadow(true);
	//this->SetReceivesShadow(true);
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
tmeshFrag::~tmeshFrag()
{
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void tmeshFrag::Deallocate()
{
	m_pVertexBuffer->Deallocate();
	m_pIndexBuffer->Deallocate();
	if (m_pSkinningBuffer)
		m_pSkinningBuffer->Deallocate();
	if (m_pVertexBuffer_Old)
		m_pVertexBuffer_Old->Deallocate();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void tmeshFrag::Reallocate()
{
	m_pVertexBuffer->Reallocate();
	m_pIndexBuffer->Reallocate();
	if (m_pSkinningBuffer)
		m_pSkinningBuffer->Reallocate();
	if (m_pVertexBuffer_Old)
		m_pVertexBuffer_Old->Reallocate();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat tmeshFrag::GetVertexFormat() const
{
	return m_pVertexBuffer->GetVertexFormat();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat tmeshFrag::GetVertexFormat_Old() const
{
	return m_pVertexBuffer_Old->GetVertexFormat();
}

//----------------------------------------------------------------------------
//	SetZBias - causes polygons that are physically coplanar to appear separate
//----------------------------------------------------------------------------
void tmeshFrag::SetZBias( int i_nZBias )
{
	m_nZBias = i_nZBias;
}

//----------------------------------------------------------------------------
//	SetPrimitiveType - sets the primitive type of the fragment
//----------------------------------------------------------------------------
void tmeshFrag::SetPrimitiveType( tmeshFrag::PrimitiveType i_eType )
{
	m_ePrimitiveType = i_eType;
}

//--------------------------------------------------------------------
//  Lock
//--------------------------------------------------------------------
unsigned char* tmeshFrag::Lock()
{
	DBG_ASSERT( this->GetMorphable(), "Only morphable fragments can be locked" );

	//	SetOldVertexBuffer( m_pVertexBuffer_Old.get() , m_pVertexBuffer.get() );
	bumpBufferUtil::BackupVertexBuffer( m_pVertexBuffer_Old.get() , m_pVertexBuffer.get() );

	return m_pVertexBuffer->Lock();
}

//--------------------------------------------------------------------
//  Unlock
//--------------------------------------------------------------------
void tmeshFrag::Unlock()
{
	m_pVertexBuffer->Unlock();
}

//--------------------------------------------------------------------
//  SetVertexBuffer
//--------------------------------------------------------------------
void tmeshFrag::SetVertexBuffer( const shared_ptr<tmeshVertexBuffer>& i_pVertexBuffer)
{
	m_pVertexBuffer = i_pVertexBuffer;
}
void tmeshFrag::SetVertexBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer,
									  BYTE* i_pVertexBufferCopy,
									  int i_nVertices )
{
	m_pVertexBuffer->SetVertexBuffer(i_pVertexBuffer, i_pVertexBufferCopy, i_nVertices);
}

//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
unsigned char* tmeshFrag::LockIndices()
{
	//DBG_ASSERT( this->GetComponentSort(), "Only conponent-sort fragments can have the indices locked" );
	return m_pIndexBuffer->LockIndices();
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
void tmeshFrag::UnlockIndices()
{
	m_pIndexBuffer->UnlockIndices();
}

//--------------------------------------------------------------------
//  ReadOnlyLock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* tmeshFrag::ReadOnlyLock()
{
	return m_pVertexBuffer->ReadOnlyLock();
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void tmeshFrag::ReadOnlyUnlock()
{
	m_pVertexBuffer->ReadOnlyUnlock();
}

//--------------------------------------------------------------------
//  ReadOnlyLockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* tmeshFrag::ReadOnlyLockIndices()
{
	return m_pIndexBuffer->ReadOnlyLockIndices();
}

//--------------------------------------------------------------------
// ReadOnlyUnlockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void tmeshFrag::ReadOnlyUnlockIndices()
{
	m_pIndexBuffer->ReadOnlyUnlockIndices();
}

//--------------------------------------------------------------------
//  SetIndexBuffer
//--------------------------------------------------------------------
void tmeshFrag::SetIndexBuffer( const shared_ptr<tmeshIndexBuffer>& i_pIndexBuffer)
{
	m_pIndexBuffer = i_pIndexBuffer;
}
void tmeshFrag::SetIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer,
									 BYTE* i_pIndexBufferCopy,
									 int i_nIndices,
									 int i_SizeOfIndex,
									 int i_nNonShadowIndices )
{
	m_pIndexBuffer->SetIndexBuffer(i_pIndexBuffer, i_pIndexBufferCopy, i_nIndices, i_SizeOfIndex);
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this fragment
//----------------------------------------------------------------------------
unsigned int tmeshFrag::GetSize() const
{
	UINT size = 0;
	size += m_pVertexBuffer->GetSize();
	size += m_pIndexBuffer->GetSize();
	return size;
}

//--------------------------------------------------------------------
//  LockSkinning
//--------------------------------------------------------------------
unsigned char* tmeshFrag::LockSkinning()
{
	DBG_ASSERT( m_pSkinningBuffer.get(), "Skinning buffer object ot created yet." );
	DBG_ASSERT( this->GetMorphable(), "Only morphable fragments can be locked" );
	return m_pSkinningBuffer->Lock();
}

//--------------------------------------------------------------------
//  UnlockSkinning
//--------------------------------------------------------------------
void tmeshFrag::UnlockSkinning()
{
	DBG_ASSERT( m_pSkinningBuffer.get(), "Skinning buffer object ot created yet." );
	m_pSkinningBuffer->Unlock();
}

//--------------------------------------------------------------------
//  SetSkinningBuffer
//--------------------------------------------------------------------
void tmeshFrag::SetSkinningBuffer( g2dD3D11VertexBufferPtr i_pSkinningBuffer,
									  BYTE* i_pSkinningBufferCopy,
									  int i_nVertices )
{
	DBG_ASSERT( m_pSkinningBuffer.get(), "Skinning buffer object ot created yet." );
	m_pSkinningBuffer->SetVertexBuffer(i_pSkinningBuffer, i_pSkinningBufferCopy, i_nVertices);
	DBG_ASSERT(GetNumVertices() == i_nVertices, "Bad number of verts in SetSkinningBuffer");
}

//--------------------------------------------------------------------
//  SetVertexBuffer_Old
//--------------------------------------------------------------------
void tmeshFrag::SetVertexBuffer_Old( g2dD3D11VertexBufferPtr i_pVertexBuffer,
									  BYTE* i_pVertexBufferCopy,
									  int i_nVertices )
{
	DBG_ASSERT( m_pVertexBuffer_Old.get(), "Old vertex buffer object not created yet." );
	m_pVertexBuffer_Old->SetVertexBuffer(i_pVertexBuffer, i_pVertexBufferCopy, i_nVertices);
	DBG_ASSERT(GetNumVertices() == i_nVertices, "Bad number of verts in SetSkinningBuffer");
}