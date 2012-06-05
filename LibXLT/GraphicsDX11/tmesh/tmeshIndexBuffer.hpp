/****************************************************************************\
**	tmeshIndexBuffer.hpp
**
**	A tmeshVertexBuffer represents a D3D index buffer with an optional 
**	system memory copy for updating.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TMESH_INDEXBUFFER_HPP
#error tmeshIndexBuffer.hpp multiply included
#endif
#define TMESH_INDEXBUFFER_HPP

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class tmeshIndexBuffer
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		tmeshIndexBuffer(  bool i_bLockable = false );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~tmeshIndexBuffer();

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
		//	GetLockable - return whether the index buffer is dynamic and can
		//  be modified
		//----------------------------------------------------------------------------
		inline bool GetLockable() const;

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in bytes) being
		// used by this fragment
		//----------------------------------------------------------------------------
		virtual unsigned int GetSize() const;

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline const g2dD3D11IndexBufferPtr GetIndexBuffer() const;

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline const unsigned char* GetIndexBufferCopy() const;

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
		//  ReadOnlyLock - get read-only access to index buffer data.
		//	Can be called on non-lockable buffers.
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLockIndices() const;

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock - closes read-only access from ReadOnlyLock().
		//	Can be called on non-lockable buffers.
		//--------------------------------------------------------------------
		void ReadOnlyUnlockIndices() const;

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline g2dD3D11IndexBufferPtr GetIndexBuffer();

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline unsigned char* GetIndexBufferCopy();

		//--------------------------------------------------------------------
		//  SetIndexBuffer
		//--------------------------------------------------------------------
		void SetIndexBuffer( g2dD3D11IndexBufferPtr i_pIndexBuffer,
							 BYTE* i_pIndexBufferCopy,
							 int i_nIndices,
							 int i_SizeOfIndex,
							 int i_nNonShadowIndices = -1 );

	private:
		g2dD3D11IndexBufferPtr m_pIndexBuffer;
		BYTE* m_pIndexBufferCopy;

		bool m_bLockable;
		int m_nIndices;
		int m_SizeOfIndex;
		int m_nNonShadowIndices;
};

//----------------------------------------------------------------------------
//	GetLockable - return whether the index buffer is dynamic and can
//  be modified
//----------------------------------------------------------------------------
inline bool tmeshIndexBuffer::GetLockable() const
{
	return m_bLockable;
}

//--------------------------------------------------------------------
//  GetIndexBuffer
//--------------------------------------------------------------------
inline const g2dD3D11IndexBufferPtr tmeshIndexBuffer::GetIndexBuffer() const
{
	return m_pIndexBuffer;
}
inline g2dD3D11IndexBufferPtr tmeshIndexBuffer::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

//--------------------------------------------------------------------
//  GetIndexBuffer
//--------------------------------------------------------------------
inline const unsigned char* tmeshIndexBuffer::GetIndexBufferCopy() const
{
	return m_pIndexBufferCopy;
}
inline unsigned char* tmeshIndexBuffer::GetIndexBufferCopy()
{
	return m_pIndexBufferCopy;
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
inline int tmeshIndexBuffer::GetNumIndices() const
{
	return m_nIndices;
}

//----------------------------------------------------------------------------
//	GetNumNonShadowIndices - the number of indices in the index buffer
//		that are not used for shadows welding (always first in the index).
//----------------------------------------------------------------------------
inline int tmeshIndexBuffer::GetNumNonShadowIndices() const
{
	return m_nNonShadowIndices;
}

//----------------------------------------------------------------------------
// Get size of an index in bytes (16 -> 2, 32 -> 4)
//----------------------------------------------------------------------------
inline int tmeshIndexBuffer::GetSizeOfIndex() const
{
	return m_SizeOfIndex;
}
