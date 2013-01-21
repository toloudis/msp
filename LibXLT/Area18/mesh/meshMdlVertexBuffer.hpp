/****************************************************************************\
**	meshMdlVertexBuffer.hpp
**
**	meshMdlVertexBuffer is class for a buffer of vertex data.
**	It can be altered by using the UpdateVertices() functions or
**	with the Lock() and Unlock() functions.
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MESH_MDLVERTEXBUFFER_HPP
#error meshMdlVertexBuffer.hpp multiply included
#endif
#define MESH_MDLVERTEXBUFFER_HPP

#ifndef G3D_VERTEXBUFFER_HPP
#include "Graphics/G3d/g3dVertexBuffer.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MESH_VERTEXBUFFER_HPP
#include "Area18/mesh/meshVertexBuffer.hpp"
#endif 

class meshMdlVertexBuffer : public g3dVertexBuffer
{
	public:
		//--------------------------------------------------------------------
		//	Constructor taking shared buffers
		//--------------------------------------------------------------------
		meshMdlVertexBuffer(shared_ptr<meshVertexBuffer> &i_VertexBuffer);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~meshMdlVertexBuffer();

		//---------------------------------------------------------------------------
		// Update to use new vertex buffer, presumably with a different number
		// of vertices.
		//---------------------------------------------------------------------------
		void Update(shared_ptr<meshVertexBuffer> &i_pVertexBuffer);

		//---------------------------------------------------------------------------
		// UpdateVertices - alter the position of the vertices in the
		// given fragment. i_pNormals may be NULL, in which case the
		// normals should remain as before. i_NumVertices should
		// represent the number of positions given and should match the
		// number of vertices in the fragment.
		// This method can only be called on a fragment that was created
		// with the "morphable" flag set to true.
		//---------------------------------------------------------------------------
		virtual void UpdateVertices( int i_NumVertices, 
									 const maPoint3d* i_pVertices, 
									 const maVector3d* i_pNormals );

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		virtual int GetNumVertices() const;

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const;

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the vertex buffer copy in system memory.
		//  This should be called when you need to modify the vertices
		//  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* Lock();

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void Unlock();

		//----------------------------------------------------------------------------
		//	SetBoundingBox - Objects with a dynamic vertex buffer should set the
		//  the bounding box after modifying the buffer
		//----------------------------------------------------------------------------
		virtual void SetBoundingBox( const maAxisBox& i_BoundingBox );

		//----------------------------------------------------------------------------
		// Should the fragment be animated with velocity maps (if possible)?
		//----------------------------------------------------------------------------
		void CreateVelocityBuffer(bool i_bVelocityBuffer);

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<meshVertexBuffer>& GetVertexBuffer() const;
		inline shared_ptr<meshVertexBuffer>& GetVertexBuffer();

		//--------------------------------------------------------------------
		//  GetVertexBuffer_Old
		//--------------------------------------------------------------------
		inline const shared_ptr<meshVertexBuffer>& GetVertexBuffer_Old() const;
		inline shared_ptr<meshVertexBuffer>& GetVertexBuffer_Old();
	
	private:
		shared_ptr<meshVertexBuffer> m_pVertexBuffer;
		shared_ptr<meshVertexBuffer> m_pVertexBuffer_Old;
};


//--------------------------------------------------------------------
//  GetVertexBuffer
//--------------------------------------------------------------------
inline const shared_ptr<meshVertexBuffer>& meshMdlVertexBuffer::GetVertexBuffer() const
{
	return m_pVertexBuffer;
}
inline shared_ptr<meshVertexBuffer>& meshMdlVertexBuffer::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

//--------------------------------------------------------------------
//  GetVertexBuffer_Old
//--------------------------------------------------------------------
inline const shared_ptr<meshVertexBuffer>& meshMdlVertexBuffer::GetVertexBuffer_Old() const
{
	return m_pVertexBuffer_Old;
}
inline shared_ptr<meshVertexBuffer>& meshMdlVertexBuffer::GetVertexBuffer_Old()
{
	return m_pVertexBuffer_Old;
}
