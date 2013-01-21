/****************************************************************************\
**	meshMdlVertexBuffer.hpp
**
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/mesh/meshMdlVertexBuffer.hpp"

#include "Area18/mesh/meshBufferUtil.hpp"

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Area18/ogl/oglBufferUtil.hpp"
//#include "Area18/g3d/g3dFVFWin.hpp"

#include <algorithm>
#include <vector>
#include <limits>

namespace
{
}

//--------------------------------------------------------------------
//	Constructor taking shared buffers
//--------------------------------------------------------------------
meshMdlVertexBuffer::meshMdlVertexBuffer(shared_ptr<meshVertexBuffer> &i_pVertexBuffer)
: m_pVertexBuffer(i_pVertexBuffer)
{
	CreateVelocityBuffer(i_pVertexBuffer->GetLockable());
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
meshMdlVertexBuffer::~meshMdlVertexBuffer()
{
}

//---------------------------------------------------------------------------
// Update to use new vertex buffer, presumably with a different number
// of vertices.
//---------------------------------------------------------------------------
void meshMdlVertexBuffer::Update(shared_ptr<meshVertexBuffer> &i_pVertexBuffer)
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
void meshMdlVertexBuffer::UpdateVertices( int i_NumVertices, 
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
		DBG_ASSERT(false, "Unexpected vertex stride in meshMdlVertexBuffer::UpdateVertices");
	}

	m_pVertexBuffer->SetBoundingBox( bounding_box );
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
int meshMdlVertexBuffer::GetNumVertices() const
{
	return m_pVertexBuffer->GetNumVertices();
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
g3dType::VertexFormat meshMdlVertexBuffer::GetVertexFormat() const
{
	return m_pVertexBuffer->GetVertexFormat();
}


//--------------------------------------------------------------------
//  Lock
//--------------------------------------------------------------------
unsigned char* meshMdlVertexBuffer::Lock()
{
	DBG_ASSERT( m_pVertexBuffer->GetLockable(), "Only morphable fragments can be locked" );
	meshBufferUtil::BackupVertexBuffer( m_pVertexBuffer_Old.get() , m_pVertexBuffer.get() );
	return m_pVertexBuffer->Lock();
}


//--------------------------------------------------------------------
//  Unlock
//--------------------------------------------------------------------
void meshMdlVertexBuffer::Unlock()
{
	m_pVertexBuffer->Unlock();	
}

//----------------------------------------------------------------------------
//	SetBoundingBox - Objects with a dynamic vertex buffer should set the
//  the bounding box after modifying the buffer
//----------------------------------------------------------------------------
void meshMdlVertexBuffer::SetBoundingBox( const maAxisBox& i_BoundingBox )
{
	m_pVertexBuffer->SetBoundingBox(i_BoundingBox);	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void meshMdlVertexBuffer::CreateVelocityBuffer(bool i_bMorphable)
{
	if (i_bMorphable)
	{
		// if velocity map data already created, then don't do anything.
		// assumes num vertices won't change for lifetime of fragment.

		if (m_pVertexBuffer_Old.get() == NULL){

			// create velocity buffer
			meshBufferUtil::CreateVelocityBuffer(m_pVertexBuffer->GetDevice(), 
				m_pVertexBuffer, m_pVertexBuffer_Old, i_bMorphable);
		}
	}
	else
	{
		// destroy velocity buffer.
		m_pVertexBuffer_Old.reset();
	}
}
