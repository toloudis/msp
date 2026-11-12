/*****************************************************************************
**  g3dVertexStoreOptiX.hpp
**
**      g3dVertexStoreOptiX holds a mesh's vertices (g3dType::BumpTex1Vertex)
**	in CPU memory, a copy of them on the device, and a generation number
**	that changes whenever the vertices do. Fragments hold their store by
**	shared_ptr: a fragment from g3dFragmentCreate owns its own, and the
**	fragments of an imported group share one (each with its own indices).
**
**	Writers change GetVertices() and then call Publish (as a D3D11 vertex
**	buffer is locked, written and unlocked), or call UpdateVertices, which
**	does both. Publish gives the store a new generation.
**
**	Generations come from one process-wide counter (NewGeneration), so a
**	generation is never seen on two stores, or twice on one: comparing a
**	store's generation with one recorded earlier tells whether it changed,
**	even if the store recorded was destroyed and another made at its
**	address.
**
**	Threads. Animation threads write morphable stores (skinning, morphs,
**	vertex animation, hair) while the render thread reads them, and they
**	write between Lock and Unlock without holding a lock. So a morphable
**	store keeps a second CPU copy, taken by Publish under the store's
**	mutex, and EnsureDevice uploads that copy under the same mutex: the
**	device gets the vertices of one Publish, as D3D11 and Vulkan get what
**	was there at Unlock. A static store (not morphable) is written only
**	while it is made, and is uploaded directly.
**
**	EnsureDevice and Deallocate are for the render thread, with the OptiX
**	context current (optxContext::MakeCurrent).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DVERTEXSTOREOPTIX_HPP
#error g3dVertexStoreOptiX.hpp multiply included
#endif
#define GOX_G3DVERTEXSTOREOPTIX_HPP

#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

namespace gox
{
	class g3dVertexStoreOptiX
	{
		public:
			//--------------------------------------------------------------------
			//	The device copy of the vertices: the start of the array (the
			//	position is at offset 0 of each vertex), the number of
			//	vertices, and the generation they are from
			//--------------------------------------------------------------------
			struct DeviceVertices
			{
				CUdeviceptr	m_Ptr = 0;
				unsigned	m_Count = 0;
				uint64_t	m_Generation = 0;
			};

			explicit g3dVertexStoreOptiX(bool i_bMorphable);
			g3dVertexStoreOptiX(const g3dVertexStoreOptiX&) = delete;
			g3dVertexStoreOptiX& operator=(const g3dVertexStoreOptiX&) = delete;

			bool GetMorphable() const { return m_bMorphable; }

			//--------------------------------------------------------------------
			//	GetVertices - the CPU vertices. Call Publish after changing them.
			//--------------------------------------------------------------------
			std::vector<g3dType::BumpTex1Vertex>& GetVertices() { return m_Vertices; }
			const std::vector<g3dType::BumpTex1Vertex>& GetVertices() const { return m_Vertices; }
			int GetNumVertices() const { return static_cast<int>(m_Vertices.size()); }

			//--------------------------------------------------------------------
			//	Publish - the vertices changed: gives the store a new generation
			//	(and takes the copy EnsureDevice uploads, for a morphable store)
			//--------------------------------------------------------------------
			void Publish();

			//--------------------------------------------------------------------
			//	UpdateVertices - sets the first i_NumVertices positions, and
			//	normals if i_pNormals isn't NULL, then publishes
			//--------------------------------------------------------------------
			void UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals);

			uint64_t GetGeneration() const { return m_Generation.load(); }

			//--------------------------------------------------------------------
			//	EnsureDevice - uploads the vertices if the device copy is older
			//	than the store's generation; o_Vertices gets the device copy.
			//	Returns false if device memory ran out.
			//--------------------------------------------------------------------
			bool EnsureDevice(DeviceVertices& o_Vertices);

			//--------------------------------------------------------------------
			//	Deallocate - frees the device copy, and gives the store a new
			//	generation, so whatever was built from the old copy is remade
			//--------------------------------------------------------------------
			void Deallocate();

			//--------------------------------------------------------------------
			//	GetUploadCount - uploads so far, for tests
			//--------------------------------------------------------------------
			int GetUploadCount() const { return m_nUploads; }

			//--------------------------------------------------------------------
			//	GetDevicePtr - the device copy's address (0 if none); valid
			//	until the next EnsureDevice or Deallocate
			//--------------------------------------------------------------------
			CUdeviceptr GetDevicePtr() const { return m_DeviceCount ? m_Device.Get() : 0; }

			//--------------------------------------------------------------------
			//	NewGeneration - the next value of the process-wide counter
			//	(never 0)
			//--------------------------------------------------------------------
			static uint64_t NewGeneration();

		private:
			const bool								m_bMorphable;
			std::vector<g3dType::BumpTex1Vertex>	m_Vertices;
			std::vector<g3dType::BumpTex1Vertex>	m_Published;	// morphable only: the vertices of the last Publish
			std::mutex								m_Mutex;		// guards m_Published and the generation it is from
			std::atomic<uint64_t>					m_Generation;

			//	Render thread
			cuBuffer								m_Device;
			unsigned								m_DeviceCount;
			uint64_t								m_DeviceGeneration;	// 0: nothing uploaded
			int										m_nUploads;
	};
}
