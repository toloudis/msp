/*****************************************************************************
**  g3dVkBufferUtil.hpp
**
**      GraphicsVulkan port of GraphicsDX11's g3dDX11BufferUtil: functions
**	for dealing with vertex and index buffers. g3dVkBuffer stands where
**	DX11 has an ID3D11Buffer:
**
**	- A default-usage buffer (DX11 creates it with its initial data) is a
**	  device-local vkdBuffer, filled through the ring buffer when created.
**	  D3D11 can't map it; mapping one here asserts and returns NULL, as
**	  DX11's Map fails.
**	- A dynamic buffer (morphable, or geometry kept out of video memory),
**	  which DX11 maps with WRITE_DISCARD, keeps its bytes on the CPU and
**	  is written to the ring buffer when it is bound: after each Unmap,
**	  and again for each submission that uses it (the ring reuses memory
**	  once a submission completes), as fxEffectVk does with constants.
**
**	Both stay readable by the GPU until it is done with them when they
**	are released (vkdBuffer defers its destruction; ring memory is held
**	until its submission completes).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_G3DVKBUFFERUTIL_HPP
#error g3dVkBufferUtil.hpp multiply included
#endif
#define GVK_G3DVKBUFFERUTIL_HPP

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace gvk
{
	class vkdBuffer;
	class vkdContext;

	//--------------------------------------------------------------------
	//	g3dVkBuffer - a vertex or index buffer (see the header comment)
	//--------------------------------------------------------------------
	class g3dVkBuffer
	{
		public:
			enum Kind
			{
				e_Vertex,
				e_Index
			};

			//	i_pData: i_Size bytes of initial data, or NULL
			g3dVkBuffer(Kind i_Kind, uint32_t i_Size, bool i_bDynamic, const void* i_pData, const char* i_pName);
			~g3dVkBuffer();

			uint32_t	GetSize() const		{ return m_Size; }
			bool		IsDynamic() const	{ return m_bDynamic; }

			//	D3D11 Map: WRITE_DISCARD on a dynamic buffer (the old contents
			//	stay, which D3D11 leaves undefined); a default-usage buffer
			//	can't be mapped (NULL, as DX11's Map fails)
			uint8_t*	Map();
			void		Unmap();

			//	Where the GPU reads the buffer now: uploads a dynamic buffer to
			//	the ring when needed
			void		GetBinding(vkdContext& i_Context, VkBuffer& o_Buffer, VkDeviceSize& o_Offset);

		private:
			g3dVkBuffer(const g3dVkBuffer&) = delete;
			g3dVkBuffer& operator = (const g3dVkBuffer&) = delete;

			Kind						m_Kind;
			uint32_t					m_Size;
			bool						m_bDynamic;
			std::unique_ptr<vkdBuffer>	m_pBuffer;		// default usage
			std::vector<uint8_t>		m_Cpu;			// dynamic
			bool						m_bDirty = true;
			uint64_t					m_UploadedFor = 0;
			VkBuffer					m_RingBuffer = VK_NULL_HANDLE;
			VkDeviceSize				m_RingOffset = 0;
	};

	typedef g3dVkBuffer* g2dVkVertexBufferPtr;
	typedef g3dVkBuffer* g2dVkIndexBufferPtr;

	namespace g3dVkBufferUtil
	{
		//--------------------------------------------------------------------
		// Set flag for whether to store geometry in video or system memory
		//--------------------------------------------------------------------
		void StoreGeometryInVideoMemory(bool i_bVideoMem);

		//--------------------------------------------------------------------
		// Get current video mem usage totals, in KB
		//--------------------------------------------------------------------
		float GetTotalVertexBufferMemory();
		float GetTotalIndexBufferMemory();
		unsigned int GetNumVertexBuffers();
		unsigned int GetNumIndexBuffers();

		//--------------------------------------------------------------------
		//	CreateVertexBuffer
		//--------------------------------------------------------------------
		void CreateVertexBuffer(	int i_nBufferSize,
									uint32_t i_VertexShader,
									g2dVkVertexBufferPtr& io_pVertexBuffer,
									bool i_bMorphable,
									void* i_VertexData = NULL,
									int i_VertexDataSize = 0);

		//--------------------------------------------------------------------
		//	ReleaseVertexBuffer
		//--------------------------------------------------------------------
		void ReleaseVertexBuffer(g2dVkVertexBufferPtr i_pVertexBuffer);

		//--------------------------------------------------------------------
		//	LockVertexBuffer
		//--------------------------------------------------------------------
		uint8_t* LockVertexBuffer( g2dVkVertexBufferPtr& io_pVertexBuffer,
								   bool i_bMorphable  );

		//--------------------------------------------------------------------
		//	UnlockVertexBuffer
		//--------------------------------------------------------------------
		void UnlockVertexBuffer( g2dVkVertexBufferPtr i_pVertexBuffer );

		//--------------------------------------------------------------------
		//	GetVertexBufferSize
		//--------------------------------------------------------------------
		uint32_t GetVertexBufferSize( g2dVkVertexBufferPtr i_pVertexBuffer );

		//--------------------------------------------------------------------
		//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are
		//	needed, creates the appropriate format of index buffer
		//	and returns the total size of the buffer in bytes.
		//--------------------------------------------------------------------
		int CreateAndFillIndexBuffer(	const g3dIndexPtr& i_Indices,
										int i_NumIndices,
										int i_NumVertices,
										g2dVkIndexBufferPtr& io_pIndexBuffer,
										bool i_bMorphable = false );

		//--------------------------------------------------------------------
		//	ReleaseIndexBuffer
		//--------------------------------------------------------------------
		void ReleaseIndexBuffer(g2dVkIndexBufferPtr i_pIndexBuffer);

		//--------------------------------------------------------------------
		//	LockIndexBuffer
		//--------------------------------------------------------------------
		uint8_t* LockIndexBuffer(	g2dVkIndexBufferPtr& io_pIndexBuffer,
									bool i_bMorphable  );

		//--------------------------------------------------------------------
		//	UnlockIndexBuffer
		//--------------------------------------------------------------------
		void UnlockIndexBuffer( g2dVkIndexBufferPtr i_pIndexBuffer );

		//--------------------------------------------------------------------
		//	GetIndexBufferSize
		//--------------------------------------------------------------------
		uint32_t GetIndexBufferSize( g2dVkIndexBufferPtr i_pIndexBuffer );

		//--------------------------------------------------------------------
		//	CreateBufferCopy
		//--------------------------------------------------------------------
		uint8_t* CreateBufferCopy( int i_nBufferSize, const uint8_t* i_pBuffer );
		uint8_t* CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices,
			int i_indexSize, int i_nIndices );
	}
}
