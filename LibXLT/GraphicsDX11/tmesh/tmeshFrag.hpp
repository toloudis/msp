/****************************************************************************\
**	tmeshFrag.hpp
**
**	A tmeshFrag represents triangle mesh geometry
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMESH_FRAG_HPP
#error tmeshFrag.hpp multiply included
#endif
#define TMESH_FRAG_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef TMESH_VERTEXBUFFER_HPP
#include "GraphicsDX11/tmesh/tmeshVertexBuffer.hpp"
#endif 
#ifndef TMESH_INDEXBUFFER_HPP
#include "GraphicsDX11/tmesh/tmeshIndexBuffer.hpp"
#endif 
#ifndef TMESH_INDEXBUFFER_HPP
#include "GraphicsDX11/tmesh/tmeshIndexBuffer.hpp"
#endif 
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matMaterial;

class tmeshFrag : public g3dFragment
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);

		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		tmeshFrag(  matMaterial* i_pMaterial,
				    g3dType::VertexFormat i_VertexFormat,
					DWORD i_VertexShader,
					int i_nVertexStride,
					bool i_bMorphable = false,
					bool i_bComponentSort = false);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~tmeshFrag() = 0;

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in bytes) being
		// used by this fragment
		//----------------------------------------------------------------------------
		virtual unsigned int GetSize() const;

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetNumVertices() const;

		//----------------------------------------------------------------------------
		//	GetNumIndices - the number of indices in the index buffer
		//----------------------------------------------------------------------------
		inline int GetNumIndices() const;

		//----------------------------------------------------------------------------
		//	GetNumNonShadowIndices - the number of indices in the index buffer
		//		that are not used for shadows welding (always first in the index).
		//----------------------------------------------------------------------------
		inline int GetNumNonShadowIndices() const;

		//----------------------------------------------------------------------------
		// Get size of an index in bytes (16 -> 2, 32 -> 4)
		//----------------------------------------------------------------------------
		inline int GetSizeOfIndex() const;

		//----------------------------------------------------------------------------
		//	GetVertexStride - the size of a vertex in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetVertexStride() const;

		//----------------------------------------------------------------------------
		//	GetVertexShader - handle to custom vertex shader or FVF code
		//----------------------------------------------------------------------------
		inline DWORD GetVertexShader() const;

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const;
		virtual g3dType::VertexFormat GetVertexFormat_Old() const;

		//----------------------------------------------------------------------------
		//	GetZBias - causes polygons that are physically coplanar to appear separate
		//----------------------------------------------------------------------------
		inline int GetZBias() const;

		//----------------------------------------------------------------------------
		//	SetZBias - causes polygons that are physically coplanar to appear separate
		//----------------------------------------------------------------------------
		void SetZBias( int i_nZBias );

		enum PrimitiveType
		{
			e_TriangleList = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			e_LineList = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			e_TriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			e_QuadList = 7
		};

		//----------------------------------------------------------------------------
		//	GetPrimitiveType - return the primitve type of the fragment
		//----------------------------------------------------------------------------
		inline PrimitiveType GetPrimitiveType() const;

		//----------------------------------------------------------------------------
		//	SetPrimitiveType - sets the primitive type of the fragment
		//----------------------------------------------------------------------------
		void SetPrimitiveType( PrimitiveType i_eType );

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
		//  Lock - returns the pointer to the index buffer copy in system memory.
		//  This should be called when you need to modify the indices
		//  ONLY should be called on component-sort fragments
		//--------------------------------------------------------------------
		unsigned char* LockIndices();

		//--------------------------------------------------------------------
		//  Unlock - updates the index buffer in VRAM. This should be called
		//  when you are done modifying the indices.
		//	ONLY should be called on component-sort fragments
		//--------------------------------------------------------------------
		void UnlockIndices();

		//--------------------------------------------------------------------
		//  ReadOnlyLock
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLock();

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock
		//--------------------------------------------------------------------
		void ReadOnlyUnlock();

		//--------------------------------------------------------------------
		//  ReadOnlyLockIndices
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLockIndices();

		//--------------------------------------------------------------------
		// ReadOnlyUnlockIndices
		//--------------------------------------------------------------------
		void ReadOnlyUnlockIndices();

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<tmeshVertexBuffer>& GetVertexBuffer() const;
		inline shared_ptr<tmeshVertexBuffer>& GetVertexBuffer();

		//--------------------------------------------------------------------
		//  GetVertexBuffer_Old
		//--------------------------------------------------------------------
		inline const shared_ptr<tmeshVertexBuffer>& GetVertexBuffer_Old() const;
		inline shared_ptr<tmeshVertexBuffer>& GetVertexBuffer_Old();

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<tmeshIndexBuffer>& GetIndexBuffer() const;
		inline shared_ptr<tmeshIndexBuffer>& GetIndexBuffer();

		//--------------------------------------------------------------------
		//  GetSkinningBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<tmeshVertexBuffer>& GetSkinningBuffer() const;
		inline shared_ptr<tmeshVertexBuffer>& GetSkinningBuffer();


		//--------------------------------------------------------------------
		//  LockSkinning - returns the pointer to the skinning buffer copy in 
		//	system memory. This should be called when you need to modify the 
		//	vertices.  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* LockSkinning();

		//--------------------------------------------------------------------
		//  UnlockSkinning - updates the skinning buffer in VRAM. This should 
		//	be called when you are done modifying the data.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void UnlockSkinning();

	protected:


		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		//inline unsigned char* GetVertexBufferCopy();

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		//inline unsigned char* GetIndexBufferCopy();

		//--------------------------------------------------------------------
		//  GetSkinningBuffer
		//--------------------------------------------------------------------
		//inline unsigned char* GetSkinningBufferCopy();

		//--------------------------------------------------------------------
		//  SetVertexBuffer
		//--------------------------------------------------------------------
		void SetVertexBuffer( const shared_ptr<tmeshVertexBuffer>& i_pVertexBuffer);
		void SetVertexBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer,
							  BYTE* i_pVertexBufferCopy,
							  int i_nVertices );
		void SetVertexBuffer_Old( g2dD3D11VertexBufferPtr i_pVertexBuffer,
								  BYTE* i_pVertexBufferCopy,
								  int inVertices );

		//--------------------------------------------------------------------
		//  SetIndexBuffer
		//--------------------------------------------------------------------
		void SetIndexBuffer( const shared_ptr<tmeshIndexBuffer>& i_pIndexBuffer);
		void SetIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer,
							 BYTE* i_pIndexBufferCopy,
							 int i_nIndices,
							 int i_SizeOfIndex,
							 int i_nNonShadowIndices = -1 );

		//--------------------------------------------------------------------
		//  SetSkinningBuffer
		//--------------------------------------------------------------------
		void SetSkinningBuffer( g2dD3D11VertexBufferPtr i_pSkinningBuffer,
											  BYTE* i_pSkinningBufferCopy,
											  int i_nVertices );

		void SetVelocityBuffer( g2dD3D11VertexBufferPtr i_pVertexBuffer_Old,
									  BYTE* i_pVertexBuffer_OldCopy,
									  int i_nVertices );

	private:
		shared_ptr<tmeshVertexBuffer> m_pVertexBuffer;
		shared_ptr<tmeshVertexBuffer> m_pVertexBuffer_Old;
		shared_ptr<tmeshVertexBuffer> m_pSkinningBuffer;
		shared_ptr<tmeshIndexBuffer> m_pIndexBuffer;
		
		DWORD m_VertexShader;
		int m_nZBias;
		PrimitiveType m_ePrimitiveType;
		static int sm_RendererId;
};

//--------------------------------------------------------------------
//  GetVertexBuffer
//--------------------------------------------------------------------
inline const shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetVertexBuffer() const
{
	return m_pVertexBuffer;
}
inline shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

//--------------------------------------------------------------------
//  GetIndexBuffer
//--------------------------------------------------------------------
inline const shared_ptr<tmeshIndexBuffer>& tmeshFrag::GetIndexBuffer() const
{
	return m_pIndexBuffer;
}
inline shared_ptr<tmeshIndexBuffer>& tmeshFrag::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

//--------------------------------------------------------------------
//  GetSkinningBuffer
//--------------------------------------------------------------------
inline const shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetSkinningBuffer() const
{
	return m_pSkinningBuffer;
}
inline shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetSkinningBuffer()
{
	return m_pSkinningBuffer;
}

//--------------------------------------------------------------------
//  GetVertexBuffer_Old
//--------------------------------------------------------------------
inline const shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetVertexBuffer_Old() const
{
	return m_pVertexBuffer_Old;
}
inline shared_ptr<tmeshVertexBuffer>& tmeshFrag::GetVertexBuffer_Old()
{
	return m_pVertexBuffer_Old;
}

//--------------------------------------------------------------------
//  GetSkinningBuffer
//--------------------------------------------------------------------
//inline const unsigned char* tmeshFrag::GetSkinningBufferCopy() const
//{
//	return m_pSkinningBuffer->GetVertexBufferCopy();
//}
//inline unsigned char* tmeshFrag::GetSkinningBufferCopy()
//{
//	return m_pSkinningBuffer->GetVertexBufferCopy();
//}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
inline int tmeshFrag::GetNumVertices() const
{
	return m_pVertexBuffer->GetNumVertices();
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
inline int tmeshFrag::GetNumIndices() const
{
	return m_pIndexBuffer->GetNumIndices();
}

//----------------------------------------------------------------------------
//	GetNumNonShadowIndices - the number of indices in the index buffer
//		that are not used for shadows welding (always first in the index).
//----------------------------------------------------------------------------
inline int tmeshFrag::GetNumNonShadowIndices() const
{
	return m_pIndexBuffer->GetNumNonShadowIndices();
}

//----------------------------------------------------------------------------
// Get size of an index in bytes (16 -> 2, 32 -> 4)
//----------------------------------------------------------------------------
inline int tmeshFrag::GetSizeOfIndex() const
{
	return m_pIndexBuffer->GetSizeOfIndex();
}

//----------------------------------------------------------------------------
//	GetVertexStride
//----------------------------------------------------------------------------
inline int tmeshFrag::GetVertexStride() const
{
	return m_pVertexBuffer->GetVertexStride();
}

//----------------------------------------------------------------------------
//	GetVertexShader
//----------------------------------------------------------------------------
inline DWORD tmeshFrag::GetVertexShader() const
{
	return m_VertexShader;
}

//----------------------------------------------------------------------------
//	GetZBias - causes polygons that are physically coplanar to appear separate
//----------------------------------------------------------------------------
inline int tmeshFrag::GetZBias() const
{
	return m_nZBias;
}

//----------------------------------------------------------------------------
//	GetPrimitiveType - return the primitve type of the fragment
//----------------------------------------------------------------------------
inline tmeshFrag::PrimitiveType tmeshFrag::GetPrimitiveType() const
{
	return m_ePrimitiveType;
}
