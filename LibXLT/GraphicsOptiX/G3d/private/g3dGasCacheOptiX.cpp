/*****************************************************************************
**  g3dGasCacheOptiX.cpp
**
**      See g3dGasCacheOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_OPTXSCENETYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxSceneTypes.h"
#endif
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <utility>

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gox
{
	namespace
	{
		g3dGasCacheOptiX* l_pCache = NULL;

		//	The vertex formats the fragment creators make, and where the GAS and
		//	the device programs find positions and normals in them
		static_assert(offsetof(g3dType::BumpTex1Vertex, m_Vertex) == 0, "the GAS reads positions at the start of each vertex");
		static_assert(sizeof(g3dType::BumpTex1Vertex) % 4 == 0, "OptiX needs a vertex stride that is a multiple of 4");

		bool supported_format(const gvk::tmeshVertexBuffer& i_Vertices, unsigned& o_NormalOffset)
		{
			if (i_Vertices.GetVertexFormat() == g3dType::e_BumpTex1Vertex &&
				i_Vertices.GetVertexStride() == int(sizeof(g3dType::BumpTex1Vertex)))
			{
				o_NormalOffset = unsigned(offsetof(g3dType::BumpTex1Vertex, m_Normal));
				return true;
			}
			return false;
		}

		void log_format_once(const gvk::tmeshVertexBuffer& i_Vertices)
		{
			static bool s_bLogged = false;
			if (!s_bLogged)
			{
				s_bLogged = true;
				DBG_LOG("GraphicsOptiX: vertex format " << int(i_Vertices.GetVertexFormat()) << " (stride "
						<< i_Vertices.GetVertexStride() << ") has no float3 position the GAS can read; such fragments aren't traced");
			}
		}

		void log_primitive_once(int i_Type)
		{
			static bool s_bLogged = false;
			if (!s_bLogged && i_Type == gvk::tmeshFrag::e_TriangleStrip)
			{
				s_bLogged = true;
				DBG_LOG("GraphicsOptiX: a triangle strip fragment isn't traced (only triangle lists are)");
			}
		}

		const unsigned int c_GeometryFlags = OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct g3dGasCacheOptiX::Entry
	{
		weak_ptr<gvk::tmeshVertexBuffer>				m_pVertices;
		std::vector<weak_ptr<gvk::tmeshIndexBuffer>>	m_pIndices;

		//	What the GAS (or its absence) is from; 0: nothing yet
		uint64_t				m_VertexGeneration = 0;
		std::vector<uint64_t>	m_IndexGenerations;

		bool					m_bDynamicVertices = false;
		cuBuffer				m_VertexUpload;
		std::vector<cuBuffer>	m_IndexUploads;
		cuBuffer				m_Accel;
		cuBuffer				m_Temp;			// dynamic vertices: build and refit scratch
		int						m_nRefitsSinceBuild = 0;
		Gas						m_Gas;
	};

	bool g3dGasCacheOptiX::Key::operator<(const Key& i_Other) const
	{
		if (m_pVertices != i_Other.m_pVertices)
		{
			return m_pVertices < i_Other.m_pVertices;
		}
		if (m_Indices != i_Other.m_Indices)
		{
			return m_Indices < i_Other.m_Indices;
		}
		return m_IndexCounts < i_Other.m_IndexCounts;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g3dGasCacheOptiX::g3dGasCacheOptiX(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context)
		: m_Device(i_Device)
		, m_Context(i_Context)
		, m_Memory(i_Device)
		, m_LastGeneration(0)
	{
		DBG_ASSERT(i_Device.HasExternalMemory(), "g3dGasCacheOptiX needs a Vulkan device with external memory");
		optxContext::MakeCurrent();
		m_Timeline.Import(i_Device.Device(), i_Device.Timeline());
	}

	g3dGasCacheOptiX::~g3dGasCacheOptiX()
	{
		optxContext::MakeCurrent();
		optxContext::Synchronize();
		m_Entries.clear();
		m_Timeline.Release();
	}

	g3dGasCacheOptiX* g3dGasCacheOptiX::Get()
	{
		return l_pCache;
	}

	void g3dGasCacheOptiX::Set(g3dGasCacheOptiX* i_pCache)
	{
		l_pCache = i_pCache;
	}

	bool g3dGasCacheOptiX::IsTraceable(const gvk::tmeshFrag& i_Fragment)
	{
		if (i_Fragment.GetPrimitiveType() != gvk::tmeshFrag::e_TriangleList)
		{
			log_primitive_once(i_Fragment.GetPrimitiveType());
			return false;
		}
		const shared_ptr<gvk::tmeshVertexBuffer>& vertices = i_Fragment.GetVertexBuffer();
		const shared_ptr<gvk::tmeshIndexBuffer>& indices = i_Fragment.GetIndexBuffer();
		if (!vertices || !indices || !vertices->GetVertexBuffer() || !indices->GetIndexBuffer() ||
			vertices->GetNumVertices() <= 0 || indices->GetNumNonShadowIndices() < 3)
		{
			return false;
		}
		unsigned normal_offset = 0;
		if (!supported_format(*vertices, normal_offset))
		{
			log_format_once(*vertices);
			return false;
		}
		return true;
	}

	size_t g3dGasCacheOptiX::GetGasBytes() const
	{
		size_t bytes = 0;
		for (const auto& entry : m_Entries)
		{
			bytes += entry.second->m_Gas.m_Bytes;
		}
		return bytes;
	}

	//------------------------------------------------------------------------
	//	Entries
	//------------------------------------------------------------------------
	bool g3dGasCacheOptiX::same_buffers(const Entry& i_Entry, const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments) const
	{
		if (i_Entry.m_pVertices.lock() != i_Fragments[0]->GetVertexBuffer() || i_Entry.m_pIndices.size() != i_nFragments)
		{
			return false;
		}
		for (size_t i = 0; i < i_nFragments; ++i)
		{
			if (i_Entry.m_pIndices[i].lock() != i_Fragments[i]->GetIndexBuffer())
			{
				return false;
			}
		}
		return true;
	}

	const g3dGasCacheOptiX::Gas* g3dGasCacheOptiX::Ensure(const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments)
	{
		if (i_nFragments == 0)
		{
			return NULL;
		}
		const shared_ptr<gvk::tmeshVertexBuffer>& vertices = i_Fragments[0]->GetVertexBuffer();
		Key key;
		key.m_pVertices = vertices.get();
		for (size_t i = 0; i < i_nFragments; ++i)
		{
			const gvk::tmeshFrag& fragment = *i_Fragments[i];
			if (!IsTraceable(fragment) || fragment.GetVertexBuffer() != vertices)
			{
				DBG_ASSERT(fragment.GetVertexBuffer() == vertices || !IsTraceable(fragment),
						   "g3dGasCacheOptiX::Ensure: the fragments of one GAS must share their vertex buffer");
				return NULL;
			}
			key.m_Indices.push_back(fragment.GetIndexBuffer().get());
			key.m_IndexCounts.push_back(fragment.GetNumNonShadowIndices());
		}

		std::unique_ptr<Entry>& slot = m_Entries[key];
		if (!slot || !same_buffers(*slot, i_Fragments, i_nFragments))
		{
			//	New, or its buffers were freed and others made at the same addresses
			slot.reset(new Entry);
			slot->m_pVertices = vertices;
			for (size_t i = 0; i < i_nFragments; ++i)
			{
				slot->m_pIndices.push_back(i_Fragments[i]->GetIndexBuffer());
			}
			slot->m_IndexGenerations.assign(i_nFragments, 0);
			slot->m_IndexUploads.resize(i_nFragments);
		}
		return update(*slot, i_Fragments, i_nFragments) ? &slot->m_Gas : NULL;
	}

	void g3dGasCacheOptiX::Collect()
	{
		for (auto it = m_Entries.begin(); it != m_Entries.end(); )
		{
			const Entry& entry = *it->second;
			bool gone = entry.m_pVertices.expired() || entry.m_pVertices.lock().get() != it->first.m_pVertices;
			for (size_t i = 0; i < entry.m_pIndices.size() && !gone; ++i)
			{
				gone = entry.m_pIndices[i].expired() || entry.m_pIndices[i].lock().get() != it->first.m_Indices[i];
			}
			if (gone)
			{
				it = m_Entries.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void g3dGasCacheOptiX::Clear()
	{
		optxContext::Synchronize();
		m_Entries.clear();
	}

	//------------------------------------------------------------------------
	//	Building
	//------------------------------------------------------------------------
	bool g3dGasCacheOptiX::device_pointer(const gvk::g3dVkBuffer* i_pBuffer, const uint8_t* i_pCpuCopy, size_t i_Bytes,
										  cuBuffer& io_Upload, CUdeviceptr& o_Ptr, uint64_t& io_WaitValue)
	{
		o_Ptr = 0;
		if (!i_pBuffer || i_Bytes > i_pBuffer->GetSize())
		{
			return false;
		}
		if (!i_pBuffer->IsDynamic() && i_pBuffer->GetDeviceBuffer()->IsExportable())
		{
			//	In place, once Vulkan's upload has landed
			o_Ptr = m_Memory.Map(*i_pBuffer->GetDeviceBuffer());
			io_WaitValue = std::max(io_WaitValue, i_pBuffer->GetUploadValue());
			io_Upload.Free();
			return o_Ptr != 0;
		}
		//	A dynamic buffer's bytes as of its last Unmap; a static one that
		//	isn't exportable from its tmesh buffer's CPU copy
		const uint8_t* source = i_pBuffer->IsDynamic() ? i_pBuffer->GetCpuData() : i_pCpuCopy;
		if (!source)
		{
			DBG_LOG("GraphicsOptiX: a mesh buffer has neither exportable memory nor a CPU copy; it isn't traced");
			return false;
		}
		if (!io_Upload.Upload(source, i_Bytes))
		{
			return false;
		}
		++m_Stats.m_Uploads;
		o_Ptr = io_Upload.Get();
		return true;
	}

	bool g3dGasCacheOptiX::wait_for_uploads(uint64_t i_Value)
	{
		if (i_Value == 0 || i_Value <= m_Device.CompletedValue())
		{
			return true;
		}
		{
			//	Still in the open command buffer: submit it, or the wait would
			//	never end
			std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
			if (i_Value >= m_Device.PendingValue())
			{
				m_Context.Flush();
				++m_Stats.m_Flushes;
			}
		}
		++m_Stats.m_UploadWaits;
		return m_Timeline.Wait(i_Value);
	}

	bool g3dGasCacheOptiX::update(Entry& io_Entry, const gvk::tmeshFrag* const* i_Fragments, size_t i_nFragments)
	{
		const gvk::tmeshVertexBuffer& vertices = *i_Fragments[0]->GetVertexBuffer();
		const uint64_t vertex_generation = vertices.GetGeneration();
		std::vector<uint64_t> index_generations(i_nFragments);
		for (size_t i = 0; i < i_nFragments; ++i)
		{
			index_generations[i] = i_Fragments[i]->GetIndexBuffer()->GetGeneration();
		}
		if (vertex_generation == io_Entry.m_VertexGeneration && index_generations == io_Entry.m_IndexGenerations)
		{
			return io_Entry.m_Gas.m_Handle != 0;
		}

		Gas& gas = io_Entry.m_Gas;
		const unsigned old_vertex_count = gas.m_NumVertices;
		const std::vector<Input> old_inputs = gas.m_Inputs;
		const bool vertices_changed = vertex_generation != io_Entry.m_VertexGeneration || gas.m_Vertices == 0;

		bool ok = supported_format(vertices, gas.m_NormalOffset);
		const gvk::g3dVkBuffer* vk_vertices = vertices.GetVertexBuffer();
		const unsigned vertex_count = unsigned(std::max(vertices.GetNumVertices(), 0));
		uint64_t wait_value = 0;
		if (ok && vertices_changed)
		{
			ok = device_pointer(vk_vertices, vertices.GetVertexBufferCopy(), size_t(vertex_count) * vertices.GetVertexStride(),
								io_Entry.m_VertexUpload, gas.m_Vertices, wait_value);
		}
		gas.m_VertexStride = unsigned(vertices.GetVertexStride());
		gas.m_NumVertices = vertex_count;
		io_Entry.m_bDynamicVertices = vk_vertices && vk_vertices->IsDynamic();

		gas.m_Inputs.resize(i_nFragments);
		gas.m_NumTriangles = 0;
		bool indices_changed = false;
		for (size_t i = 0; i < i_nFragments && ok; ++i)
		{
			const gvk::tmeshIndexBuffer& indices = *i_Fragments[i]->GetIndexBuffer();
			Input& input = gas.m_Inputs[i];
			const unsigned triangles = unsigned(indices.GetNumNonShadowIndices() / 3);
			const unsigned index_size = unsigned(indices.GetSizeOfIndex());
			if (index_generations[i] != io_Entry.m_IndexGenerations[i] || input.m_Indices == 0)
			{
				indices_changed = true;
				ok = (index_size == 2 || index_size == 4) &&
					 device_pointer(indices.GetIndexBuffer(), indices.GetIndexBufferCopy(), size_t(triangles) * 3 * index_size,
									io_Entry.m_IndexUploads[i], input.m_Indices, wait_value);
			}
			input.m_IndexSize = index_size;
			input.m_NumTriangles = triangles;
			gas.m_NumTriangles += triangles;

			//	Indices past the vertices would read outside the buffer
			const uint8_t* copy = indices.GetIndexBufferCopy();
			if (ok && copy && (indices_changed || vertex_count != old_vertex_count))
			{
				uint32_t max_index = 0;
				for (size_t k = 0; k < size_t(triangles) * 3; ++k)
				{
					max_index = std::max(max_index, index_size == 2 ? uint32_t(reinterpret_cast<const uint16_t*>(copy)[k])
																	: reinterpret_cast<const uint32_t*>(copy)[k]);
				}
				if (max_index >= vertex_count)
				{
					DBG_LOG("GraphicsOptiX: a fragment has index " << max_index << " but only " << vertex_count
							<< " vertices; it gets no GAS");
					ok = false;
				}
			}
		}

		ok = ok && wait_for_uploads(wait_value);
		if (ok)
		{
			bool same_triangles = !indices_changed && old_inputs.size() == gas.m_Inputs.size();
			for (size_t i = 0; i < old_inputs.size() && same_triangles; ++i)
			{
				same_triangles = old_inputs[i].m_NumTriangles == gas.m_Inputs[i].m_NumTriangles;
			}
			const bool refit = io_Entry.m_bDynamicVertices && gas.m_Handle != 0 && same_triangles &&
							   vertex_count == old_vertex_count && io_Entry.m_nRefitsSinceBuild < c_MaxRefits;
			ok = build(io_Entry, refit);
		}
		if (!ok)
		{
			io_Entry.m_Accel.Free();
			io_Entry.m_Temp.Free();
			gas.m_Handle = 0;
			gas.m_Vertices = 0;
			gas.m_Bytes = 0;
			for (Input& input : gas.m_Inputs)
			{
				input.m_Indices = 0;
			}
			io_Entry.m_nRefitsSinceBuild = 0;
		}
		gas.m_Generation = ++m_LastGeneration;
		io_Entry.m_VertexGeneration = vertex_generation;
		io_Entry.m_IndexGenerations = index_generations;
		return ok;
	}

	bool g3dGasCacheOptiX::build(Entry& io_Entry, bool i_bRefit)
	{
		G3D_PROFILE_SCOPE(i_bRefit ? "GasRefit" : "GasBuild");
		const OptixDeviceContext context = optxContext::GetOptix();
		const CUstream stream = optxContext::GetStream();
		Gas& gas = io_Entry.m_Gas;
		const bool dynamic = io_Entry.m_bDynamicVertices;

		std::vector<CUdeviceptr> vertex_pointers(gas.m_Inputs.size(), gas.m_Vertices);
		std::vector<OptixBuildInput> inputs(gas.m_Inputs.size());
		for (size_t i = 0; i < inputs.size(); ++i)
		{
			OptixBuildInput& input = inputs[i];
			input = OptixBuildInput();
			input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
			input.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
			input.triangleArray.vertexStrideInBytes = gas.m_VertexStride;
			input.triangleArray.numVertices = gas.m_NumVertices;
			input.triangleArray.vertexBuffers = &vertex_pointers[i];
			const bool shorts = gas.m_Inputs[i].m_IndexSize == 2;
			input.triangleArray.indexFormat = shorts ? OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 : OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
			input.triangleArray.indexStrideInBytes = 3 * gas.m_Inputs[i].m_IndexSize;
			input.triangleArray.numIndexTriplets = gas.m_Inputs[i].m_NumTriangles;
			input.triangleArray.indexBuffer = gas.m_Inputs[i].m_Indices;
			input.triangleArray.flags = &c_GeometryFlags;
			input.triangleArray.numSbtRecords = 1;
		}

		OptixAccelBuildOptions options = {};
		options.buildFlags = dynamic ? OPTIX_BUILD_FLAG_ALLOW_UPDATE
									 : (OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | OPTIX_BUILD_FLAG_ALLOW_COMPACTION);
		options.operation = i_bRefit ? OPTIX_BUILD_OPERATION_UPDATE : OPTIX_BUILD_OPERATION_BUILD;

		OptixAccelBufferSizes sizes = {};
		if (OPTIX_CHECK(optixAccelComputeMemoryUsage(context, &options, inputs.data(), unsigned(inputs.size()), &sizes)))
		{
			return false;
		}

		if (dynamic)
		{
			//	Keep the scratch: a refit follows nearly every change. The GAS
			//	is refit in place.
			if (!io_Entry.m_Temp.Reserve(std::max(sizes.tempSizeInBytes, sizes.tempUpdateSizeInBytes)) ||
				(!i_bRefit && !io_Entry.m_Accel.Reserve(sizes.outputSizeInBytes)))
			{
				return false;
			}
			OptixTraversableHandle handle = gas.m_Handle;
			if (OPTIX_CHECK(optixAccelBuild(context, stream, &options, inputs.data(), unsigned(inputs.size()),
											io_Entry.m_Temp.Get(), io_Entry.m_Temp.GetSize(),
											io_Entry.m_Accel.Get(), io_Entry.m_Accel.GetSize(), &handle, NULL, 0)))
			{
				return false;
			}
			gas.m_Handle = handle;
			gas.m_Bytes = io_Entry.m_Accel.GetSize();
			if (i_bRefit)
			{
				++io_Entry.m_nRefitsSinceBuild;
				++m_Stats.m_Refits;
			}
			else
			{
				io_Entry.m_nRefitsSinceBuild = 0;
				++m_Stats.m_Builds;
			}
			return true;
		}

		//	Static: build, then compact into m_Accel
		io_Entry.m_Temp.Free();
		cuBuffer temp;
		cuBuffer output;
		cuBuffer compacted_size;
		if (!temp.Reserve(sizes.tempSizeInBytes) || !output.Reserve(sizes.outputSizeInBytes) ||
			!compacted_size.Reserve(sizeof(uint64_t)))
		{
			return false;
		}
		OptixAccelEmitDesc emit = {};
		emit.type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
		emit.result = compacted_size.Get();
		OptixTraversableHandle handle = 0;
		if (OPTIX_CHECK(optixAccelBuild(context, stream, &options, inputs.data(), unsigned(inputs.size()),
										temp.Get(), temp.GetSize(), output.Get(), output.GetSize(), &handle, &emit, 1)))
		{
			return false;
		}
		++m_Stats.m_Builds;
		io_Entry.m_nRefitsSinceBuild = 0;
		uint64_t size = 0;
		if (!compacted_size.Download(&size, sizeof(size)))	// waits for the build
		{
			return false;
		}
		if (size == 0 || size >= output.GetSize())
		{
			io_Entry.m_Accel = std::move(output);
			gas.m_Handle = handle;
			gas.m_Bytes = io_Entry.m_Accel.GetSize();
			return true;
		}
		io_Entry.m_Accel.Free();
		if (!io_Entry.m_Accel.Reserve(size) ||
			OPTIX_CHECK(optixAccelCompact(context, stream, handle, io_Entry.m_Accel.Get(), io_Entry.m_Accel.GetSize(), &handle)) ||
			!optxContext::Synchronize())	// before the uncompacted output is freed
		{
			return false;
		}
		++m_Stats.m_Compactions;
		gas.m_Handle = handle;
		gas.m_Bytes = io_Entry.m_Accel.GetSize();
		return true;
	}
}
