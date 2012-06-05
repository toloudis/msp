/****************************************************************************\
**	tmeshVertexBuffer.hpp
**
**	A tmeshVertexBuffer represents a D3D vertex buffer with an optional 
**	system memory copy for updating.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TMESH_VERTEXBUFFER_HPP
#error tmeshVertexBuffer.hpp multiply included
#endif
#define TMESH_VERTEXBUFFER_HPP

#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif 
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class tmeshVertexBuffer 
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		tmeshVertexBuffer(  g3dType::VertexFormat i_VertexFormat,
							int i_nVertexStride,
							bool i_bLockable = false);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~tmeshVertexBuffer();

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		void Deallocate();

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		void Reallocate();

		//----------------------------------------------------------------------------
		//	GetLockable - return whether the vertex buffer is dynamic and can
		//  be modified
		//----------------------------------------------------------------------------
		inline bool GetLockable() const;

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in bytes) being
		// used by this buffer
		//----------------------------------------------------------------------------
		virtual unsigned int GetSize() const;

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline const g2dD3D11VertexBufferPtr GetVertexBuffer() const;

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline const unsigned char* GetVertexBufferCopy() const;

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetNumVertices() const;

		//----------------------------------------------------------------------------
		//	GetVertexStride - the size of a vertex in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetVertexStride() const;

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
		unsigned char* Lock();

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		void Unlock();

		//--------------------------------------------------------------------
		//  ReadOnlyLock - get read-only access to vertex buffer data.
		//	Can be called on non-lockable buffers.
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLock() const;

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock - closes read-only access from ReadOnlyLock().
		//	Can be called on non-lockable buffers.
		//--------------------------------------------------------------------
		void ReadOnlyUnlock() const;

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline g2dD3D11VertexBufferPtr GetVertexBuffer();

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline unsigned char* GetVertexBufferCopy();

		//--------------------------------------------------------------------
		//  SetVertexBuffer
		//--------------------------------------------------------------------
		void SetVertexBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer,
							  BYTE* i_pVertexBufferCopy,
							  int i_nVertices );

		//----------------------------------------------------------------------------
		//	BoundingBox
		//----------------------------------------------------------------------------
		const maAxisBox& GetBoundingBox() const;
		void SetBoundingBox( const maAxisBox& i_BoundingBox );

		//----------------------------------------------------------------------------
		//	Return uv scaling factors to keep uvs within 0..1
		//----------------------------------------------------------------------------
		void GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const;
		void SetUVBakeFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate);

		//----------------------------------------------------------------------------
		//	Return uv overlapping factor
		//----------------------------------------------------------------------------
		void SetUVOverlapFactor(float i_overlap);
		float GetUVOverlapFactor();

	private:
		bool m_bLockable;
		int m_nVertices;
		g3dType::VertexFormat m_VertexFormat;
		int m_nVertexStride;
		
		g2dD3D11VertexBufferPtr m_pVertexBuffer;

		BYTE* m_pVertexBufferCopy;

		float m_UVOverlapping;
		maAxisBox m_BoundingBox;
		maPoint2d m_BakeUVScale, m_BakeUVTranslate;
};

//----------------------------------------------------------------------------
//	GetLockable - return whether the vertex buffer is dynamic and can
//  be modified
//----------------------------------------------------------------------------
inline bool tmeshVertexBuffer::GetLockable() const
{
	return m_bLockable;
}

//--------------------------------------------------------------------
//  GetVertexBuffer
//--------------------------------------------------------------------
inline const g2dD3D11VertexBufferPtr tmeshVertexBuffer::GetVertexBuffer() const
{
	return m_pVertexBuffer;
}
inline g2dD3D11VertexBufferPtr tmeshVertexBuffer::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

//--------------------------------------------------------------------
//  GetVertexBuffer
//--------------------------------------------------------------------
inline const unsigned char* tmeshVertexBuffer::GetVertexBufferCopy() const
{
	return m_pVertexBufferCopy;
}
inline unsigned char* tmeshVertexBuffer::GetVertexBufferCopy()
{
	return m_pVertexBufferCopy;
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
inline int tmeshVertexBuffer::GetNumVertices() const
{
	return m_nVertices;
}


//----------------------------------------------------------------------------
//	GetVertexStride
//----------------------------------------------------------------------------
inline int tmeshVertexBuffer::GetVertexStride() const
{
	return m_nVertexStride;
}