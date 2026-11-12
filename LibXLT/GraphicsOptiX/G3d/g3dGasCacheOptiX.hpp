/*****************************************************************************
**  g3dGasCacheOptiX.hpp
**
**      g3dGasCacheOptiX holds the geometry acceleration structures (GAS)
**	OptiX traces, built from GraphicsVulkan's fragments (tmeshFrag
**	triangle lists, bumpTriMeshBumpFrag among them). It is shared by every
**	scene BVH (GraphicsOptiXLayer makes the process's one; tests make
**	their own).
**
**	A GAS has one build input per fragment: one fragment, or the fragments
**	of a group, which share one tmeshVertexBuffer and each have their own
**	tmeshIndexBuffer (g3dSceneBVHOptiX decides). Each input has its own
**	SBT record, so optixGetSbtGASIndex() says which fragment was hit. An
**	entry is keyed by the vertex buffer and each input's index buffer and
**	index count, and holds weak references to them, so a freed buffer
**	drops its entry at the next Collect.
**
**	Where the GAS is built from:
**	- A static (default-usage) buffer is read in place, from Vulkan's
**	  device memory imported into CUDA (cuExternalMemoryCache): there is no
**	  upload. Before building, CUDA waits on Vulkan's timeline for the
**	  buffer's upload (flushing Vulkan's open command buffer first if the
**	  upload is still in it). The GAS is built for fast tracing and
**	  compacted. A static buffer that isn't exportable (made before
**	  g3dVkBufferUtil::ExportStaticGeometry) is uploaded from its CPU copy.
**	- A dynamic buffer (morphable, or geometry kept out of video memory)
**	  has no device copy in Vulkan: its bytes are uploaded into the
**	  entry's own CUDA buffer whenever its generation changes. A GAS over
**	  dynamic vertices is built to allow updates and refit when only the
**	  vertices changed; it is rebuilt when an index buffer or a count
**	  changes, and after c_MaxRefits refits in a row.
**	An entry is up to date while the buffers' generations are those it was
**	built from (reordering triangles for transparency sorting keeps the
**	index generation: it changes no geometry).
**
**	Vertices must have a float3 position at offset 0: the formats the
**	fragment creators make (BumpTex1Vertex) are checked; another format
**	gets no GAS (logged once). Indices are 16- or 32-bit, as the buffer
**	has them. Triangle strips and line lists get no GAS.
**
**	Render thread, with the OptiX context current; the work goes on
**	optxContext's stream.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DGASCACHEOPTIX_HPP
#error g3dGasCacheOptiX.hpp multiply included
#endif
#define GOX_G3DGASCACHEOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_CUEXTERNALMEMORYCACHE_HPP
#include "GraphicsOptiX/cu/cuExternalMemoryCache.hpp"
#endif
#ifndef GOX_CUEXTERNALSEMAPHORE_HPP
#include "GraphicsOptiX/cu/cuExternalSemaphore.hpp"
#endif

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

namespace gvk
{
	class g3dVkBuffer;
	class tmeshFrag;
	class tmeshVertexBuffer;
	class tmeshIndexBuffer;
	class vkdContext;
	class vkdDevice;
}

namespace gox
{
	class g3dGasCacheOptiX
	{
		public:
			//	Refits of a GAS over dynamic vertices before it is rebuilt
			static const int c_MaxRefits = 64;

			//--------------------------------------------------------------------
			//	One build input of a GAS, as the instance table describes it
			//--------------------------------------------------------------------
			struct Input
			{
				CUdeviceptr	m_Indices;
				unsigned	m_IndexSize;		// 2 or 4
				unsigned	m_NumTriangles;
			};

			//--------------------------------------------------------------------
			//	A GAS and what it was built from
			//--------------------------------------------------------------------
			struct Gas
			{
				OptixTraversableHandle	m_Handle = 0;
				uint64_t				m_Generation = 0;	// changes whenever the GAS does
				CUdeviceptr				m_Vertices = 0;
				unsigned				m_VertexStride = 0;
				unsigned				m_NormalOffset = 0;	// or c_optxNoNormal
				unsigned				m_NumVertices = 0;
				unsigned				m_NumTriangles = 0;	// over all inputs
				std::vector<Input>		m_Inputs;
				size_t					m_Bytes = 0;		// the GAS's own memory
			};

			//--------------------------------------------------------------------
			//	Counts so far, for tests and the profiler
			//--------------------------------------------------------------------
			struct Stats
			{
				int		m_Builds = 0;
				int		m_Refits = 0;
				int		m_Compactions = 0;
				int		m_Uploads = 0;			// vertex or index buffers copied into CUDA memory
				int		m_UploadWaits = 0;		// waits on Vulkan's timeline for a static buffer
				int		m_Flushes = 0;			// ... that first submitted Vulkan's open command buffer
			};

			//--------------------------------------------------------------------
			//	i_Device and i_Context are Vulkan's (vkdGlobal's in MachStudio);
			//	the device must have external memory.
			//--------------------------------------------------------------------
			g3dGasCacheOptiX(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context);
			~g3dGasCacheOptiX();
			g3dGasCacheOptiX(const g3dGasCacheOptiX&) = delete;
			g3dGasCacheOptiX& operator=(const g3dGasCacheOptiX&) = delete;

			//--------------------------------------------------------------------
			//	The process's cache (GraphicsOptiXLayer's), or NULL
			//--------------------------------------------------------------------
			static g3dGasCacheOptiX* Get();
			static void Set(g3dGasCacheOptiX* i_pCache);

			//--------------------------------------------------------------------
			//	IsTraceable - whether a fragment can have a GAS: a triangle list
			//	with triangles, buffers and a vertex format the cache reads
			//--------------------------------------------------------------------
			static bool IsTraceable(const gvk::tmeshFrag& i_Fragment);

			//--------------------------------------------------------------------
			//	Ensure - the GAS for i_Fragments (one build input each, in this
			//	order), which must share one vertex buffer: built, refit or
			//	left as it is, as needed. NULL if it can't have one. The
			//	pointer stays valid until Collect or Clear.
			//--------------------------------------------------------------------
			const Gas* Ensure(const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments);
			const Gas* Ensure(const gvk::tmeshFrag& i_Fragment) { const gvk::tmeshFrag* f = &i_Fragment; return Ensure(&f, 1); }

			//--------------------------------------------------------------------
			//	Collect - drops the entries whose buffers are gone (render
			//	thread, before Ensure: a scene BVH calls it when it rebuilds)
			//--------------------------------------------------------------------
			void Collect();

			//--------------------------------------------------------------------
			//	Clear - drops every entry (tests; the layer's CleanUp)
			//--------------------------------------------------------------------
			void Clear();

			//--------------------------------------------------------------------
			//	For tests and the profiler
			//--------------------------------------------------------------------
			const Stats& GetStats() const { return m_Stats; }
			size_t GetEntryCount() const { return m_Entries.size(); }
			size_t GetGasBytes() const;
			cuExternalMemoryCache& GetMemoryCache() { return m_Memory; }

			//--------------------------------------------------------------------
			//	GetVulkanTimeline - Vulkan's timeline semaphore, imported once
			//	(CUDA refuses a second import of the same semaphore): for any
			//	CUDA work that waits for Vulkan's submissions
			//--------------------------------------------------------------------
			cuExternalSemaphore& GetVulkanTimeline() { return m_Timeline; }

		private:
			struct Key
			{
				const void*					m_pVertices;
				std::vector<const void*>	m_Indices;
				std::vector<int>			m_IndexCounts;

				bool operator<(const Key& i_Other) const;
			};
			struct Entry;

			bool same_buffers(const Entry& i_Entry, const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments) const;
			bool update(Entry& io_Entry, const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments);
			bool device_pointer(const gvk::g3dVkBuffer* i_pBuffer, const uint8_t* i_pCpuCopy, size_t i_Bytes,
								cuBuffer& io_Upload, CUdeviceptr& o_Ptr, uint64_t& io_WaitValue);
			bool wait_for_uploads(uint64_t i_Value);
			bool build(Entry& io_Entry, bool i_bRefit);

			gvk::vkdDevice&							m_Device;
			gvk::vkdContext&						m_Context;
			cuExternalMemoryCache					m_Memory;
			cuExternalSemaphore						m_Timeline;		// Vulkan's, for uploads
			std::map<Key, std::unique_ptr<Entry>>	m_Entries;
			uint64_t								m_LastGeneration;
			Stats									m_Stats;
	};
}
