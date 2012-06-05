/****************************************************************************\
**	bumpVertexBuffer.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/bump/bumpVertexBuffer.hpp"

#include "GraphicsDX11/bump/bumpBufferUtil.hpp"

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"
//#include "GraphicsDX11/g3d/g3dFVFWin.hpp"

#include <algorithm>
#include <vector>
#include <limits>

namespace
{
}

//--------------------------------------------------------------------
//	Constructor taking shared buffers
//--------------------------------------------------------------------
bumpVertexBuffer::bumpVertexBuffer(shared_ptr<tmeshVertexBuffer> &i_pVertexBuffer)
: m_pVertexBuffer(i_pVertexBuffer)
{
	CreateVelocityBuffer(i_pVertexBuffer->GetLockable());
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
bumpVertexBuffer::~bumpVertexBuffer()
{
}

//---------------------------------------------------------------------------
// Update to use new vertex buffer, presumably with a different number
// of vertices.
//---------------------------------------------------------------------------
void bumpVertexBuffer::Update(shared_ptr<tmeshVertexBuffer> &i_pVertexBuffer)
{
	m_pVertexBuffer = i_pVertexBuffer;

	// destroy velocity buffer.
	m_pVertexBuffer_Old.reset();
	CreateVelocityBuffer(i_pVertexBuffer->GetLockable());
}

//---------------------------------------------------------------------------
// UpdateVertices - alter the position of the vertices in the
// given fragment. i_pNormals may be NULL, in which case the
// normals should remain as before. i_NumVertices should
// represent the number of positions given and should match the
// number of vertices in the fragment.
// This method can only be called on a fragment that was created
// with the "morphable" flag set to true.
//---------------------------------------------------------------------------
//virtual 
void bumpVertexBuffer::UpdateVertices( int i_NumVertices, 
						const maPoint3d* i_pVertices, 
						const maVector3d* i_pNormals )
{
	maAxisBox bounding_box;

	if (m_pVertexBuffer->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex))
	{
		// Lock the vertex buffer
		g3dType::BumpTex1Vertex* vbuffer_vertices = 
			reinterpret_cast<g3dType::BumpTex1Vertex*>( this->Lock() );

		// Fill in the vertex buffer
		for( int i = 0; i < i_NumVertices; ++i )
		{
			vbuffer_vertices[i].m_Vertex = i_pVertices[i];
			if (i_pNormals) vbuffer_vertices[i].m_Normal = i_pNormals[i];
			bounding_box.Union( i_pVertices[i] );
		}
		this->Unlock();
	}
	else
	{
		DBG_ASSERT(false, "Unexpected vertex stride in bumpVertexBuffer::UpdateVertices");
	}

	m_pVertexBuffer->SetBoundingBox( bounding_box );
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
int bumpVertexBuffer::GetNumVertices() const
{
	return m_pVertexBuffer->GetNumVertices();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat bumpVertexBuffer::GetVertexFormat() const
{
	return m_pVertexBuffer->GetVertexFormat();
}


//--------------------------------------------------------------------
//  Lock
//--------------------------------------------------------------------
unsigned char* bumpVertexBuffer::Lock()
{
	DBG_ASSERT( m_pVertexBuffer->GetLockable(), "Only morphable fragments can be locked" );
	bumpBufferUtil::BackupVertexBuffer( m_pVertexBuffer_Old.get() , m_pVertexBuffer.get() );
	return m_pVertexBuffer->Lock();
}


//--------------------------------------------------------------------
//  Unlock
//--------------------------------------------------------------------
void bumpVertexBuffer::Unlock()
{
	m_pVertexBuffer->Unlock();	
}

//----------------------------------------------------------------------------
//	SetBoundingBox - Objects with a dynamic vertex buffer should set the
//  the bounding box after modifying the buffer
//----------------------------------------------------------------------------
void bumpVertexBuffer::SetBoundingBox( const maAxisBox& i_BoundingBox )
{
	m_pVertexBuffer->SetBoundingBox(i_BoundingBox);	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void bumpVertexBuffer::CreateVelocityBuffer(bool i_bMorphable)
{
	if (i_bMorphable)
	{
		// if velocity map data already created, then don't do anything.
		// assumes num vertices won't change for lifetime of fragment.

		if (m_pVertexBuffer_Old.get() == NULL){

			// create velocity buffer
			bumpBufferUtil::CreateVelocityBuffer(m_pVertexBuffer, m_pVertexBuffer_Old, i_bMorphable);
		}
	}
	else
	{
		// destroy velocity buffer.
		m_pVertexBuffer_Old.reset();
	}
}
