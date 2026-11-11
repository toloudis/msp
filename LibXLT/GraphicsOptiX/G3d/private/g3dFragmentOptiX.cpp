/*****************************************************************************
**  g3dFragmentOptiX.cpp
**
**      See g3dFragmentOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DFRAGMENTOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cstddef>

namespace gox
{
	namespace
	{
		//	Fragments aren't drawn by a raster renderer, so no renderer claims them
		const int c_RenderMode = 0;

		static_assert(offsetof(g3dType::BumpTex1Vertex, m_Vertex) == 0, "the GAS reads positions at the start of each vertex");
		static_assert(sizeof(g3dType::BumpTex1Vertex) % 4 == 0, "OptiX needs a vertex stride that is a multiple of 4");
	}

	g3dFragmentOptiX::g3dFragmentOptiX(const maPoint3d* i_Vertices,
									   const maPoint3d* i_Normals,
									   const maPoint2d* i_UVs,
									   const maPoint3d* i_Ss,
									   const maPoint3d* i_Ts,
									   int i_nVertices,
									   const g3dIndexPtr& i_Indices,
									   int i_nIndices,
									   matMaterial* i_pMaterial,
									   bool i_bMorphable,
									   PrimitiveType i_PrimitiveType)
		: g3dFragment(i_pMaterial, c_RenderMode, i_bMorphable)
		, m_pStore(std::make_shared<g3dVertexStoreOptiX>(i_bMorphable))
		, m_IndexGeneration(g3dVertexStoreOptiX::NewGeneration())
		, m_PrimitiveType(i_PrimitiveType)
		, m_DeviceIndexGeneration(0)
		, m_MaxIndex(0)
		, m_GasHandle(0)
		, m_GasVertexGeneration(0)
		, m_GasIndexGeneration(0)
		, m_GasVertexCount(0)
		, m_GasTriangleCount(0)
		, m_nRefitsSinceBuild(0)
		, m_nGasBuilds(0)
		, m_nGasRefits(0)
	{
		const maPoint3d zero3(0.0f, 0.0f, 0.0f);
		std::vector<g3dType::BumpTex1Vertex>& vertices = m_pStore->GetVertices();
		vertices.resize(std::max(i_nVertices, 0));
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			g3dType::BumpTex1Vertex& v = vertices[i];
			v.m_Vertex = i_Vertices ? i_Vertices[i] : zero3;
			v.m_Normal = i_Normals ? i_Normals[i] : zero3;
			v.m_TexCoord = i_UVs ? i_UVs[i] : maPoint2d(0.0f, 0.0f);
			v.m_S = i_Ss ? i_Ss[i] : zero3;
			v.m_T = i_Ts ? i_Ts[i] : zero3;
		}
		m_pStore->Publish();

		m_Indices.resize(std::max(i_nIndices, 0));
		if (i_Indices.Is16BitIndices() || i_Indices.Is32BitIndices())
		{
			for (size_t i = 0; i < m_Indices.size(); ++i)
			{
				m_Indices[i] = i_Indices.GetIndex(static_cast<int>(i));
			}
		}
		update_bounding_box();
	}

	g3dFragmentOptiX::g3dFragmentOptiX(const g3dFragmentOptiX& i_Frag)
		: g3dFragment(i_Frag)
		, m_pStore(std::make_shared<g3dVertexStoreOptiX>(i_Frag.GetMorphable()))
		, m_Indices(i_Frag.m_Indices)
		, m_IndexGeneration(g3dVertexStoreOptiX::NewGeneration())
		, m_PrimitiveType(i_Frag.m_PrimitiveType)
		, m_DeviceIndexGeneration(0)
		, m_MaxIndex(0)
		, m_GasHandle(0)
		, m_GasVertexGeneration(0)
		, m_GasIndexGeneration(0)
		, m_GasVertexCount(0)
		, m_GasTriangleCount(0)
		, m_nRefitsSinceBuild(0)
		, m_nGasBuilds(0)
		, m_nGasRefits(0)
	{
		m_pStore->GetVertices() = i_Frag.m_pStore->GetVertices();
		m_pStore->Publish();
	}

	g3dFragmentOptiX::~g3dFragmentOptiX()
	{
	}

	void g3dFragmentOptiX::update_bounding_box()
	{
		const std::vector<g3dType::BumpTex1Vertex>& vertices = m_pStore->GetVertices();
		if (vertices.empty())
		{
			return;
		}
		maPoint3d lo = vertices[0].m_Vertex;
		maPoint3d hi = lo;
		for (const g3dType::BumpTex1Vertex& v : vertices)
		{
			lo.SetX(std::min(lo.GetX(), v.m_Vertex.GetX()));
			lo.SetY(std::min(lo.GetY(), v.m_Vertex.GetY()));
			lo.SetZ(std::min(lo.GetZ(), v.m_Vertex.GetZ()));
			hi.SetX(std::max(hi.GetX(), v.m_Vertex.GetX()));
			hi.SetY(std::max(hi.GetY(), v.m_Vertex.GetY()));
			hi.SetZ(std::max(hi.GetZ(), v.m_Vertex.GetZ()));
		}
		SetBoundingBox(maAxisBox(lo, hi));
	}

	//--------------------------------------------------------------------
	//	The GAS
	//--------------------------------------------------------------------
	void g3dFragmentOptiX::release_gas()
	{
		m_Gas.Free();
		m_GasTemp.Free();
		m_GasHandle = 0;
		m_GasVertexCount = 0;
		m_GasTriangleCount = 0;
		m_nRefitsSinceBuild = 0;
	}

	bool g3dFragmentOptiX::upload_indices(unsigned i_nTriangles)
	{
		const uint64_t generation = m_IndexGeneration.load();
		if (m_DeviceIndexGeneration == generation)
		{
			return true;
		}
		const size_t count = size_t(i_nTriangles) * 3;
		m_MaxIndex = 0;
		for (size_t i = 0; i < count; ++i)
		{
			m_MaxIndex = std::max(m_MaxIndex, m_Indices[i]);
		}
		if (!m_DeviceIndices.Upload(m_Indices.data(), count * sizeof(uint32_t)))
		{
			m_DeviceIndexGeneration = 0;
			return false;
		}
		m_DeviceIndexGeneration = generation;
		return true;
	}

	bool g3dFragmentOptiX::build_gas(const g3dVertexStoreOptiX::DeviceVertices& i_Vertices, unsigned i_nTriangles, bool i_bRefit)
	{
		const OptixDeviceContext context = optxContext::GetOptix();
		const CUstream stream = optxContext::GetStream();
		const bool morphable = GetMorphable();

		CUdeviceptr vertices = i_Vertices.m_Ptr;
		const unsigned int geometry_flags = OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT;
		OptixBuildInput input = {};
		input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
		input.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
		input.triangleArray.vertexStrideInBytes = sizeof(g3dType::BumpTex1Vertex);
		input.triangleArray.numVertices = i_Vertices.m_Count;
		input.triangleArray.vertexBuffers = &vertices;
		input.triangleArray.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
		input.triangleArray.indexStrideInBytes = 3 * sizeof(uint32_t);
		input.triangleArray.numIndexTriplets = i_nTriangles;
		input.triangleArray.indexBuffer = m_DeviceIndices.Get();
		input.triangleArray.flags = &geometry_flags;
		input.triangleArray.numSbtRecords = 1;

		OptixAccelBuildOptions options = {};
		options.buildFlags = morphable ? OPTIX_BUILD_FLAG_ALLOW_UPDATE
									   : (OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | OPTIX_BUILD_FLAG_ALLOW_COMPACTION);
		options.operation = i_bRefit ? OPTIX_BUILD_OPERATION_UPDATE : OPTIX_BUILD_OPERATION_BUILD;

		OptixAccelBufferSizes sizes = {};
		if (OPTIX_CHECK(optixAccelComputeMemoryUsage(context, &options, &input, 1, &sizes)))
		{
			return false;
		}

		if (morphable)
		{
			//	Keep the scratch: a refit follows nearly every frame. The GAS is
			//	refit in place.
			if (!m_GasTemp.Reserve(std::max(sizes.tempSizeInBytes, sizes.tempUpdateSizeInBytes)) ||
				(!i_bRefit && !m_Gas.Reserve(sizes.outputSizeInBytes)))
			{
				return false;
			}
			OptixTraversableHandle handle = m_GasHandle;
			if (OPTIX_CHECK(optixAccelBuild(context, stream, &options, &input, 1,
											m_GasTemp.Get(), m_GasTemp.GetSize(), m_Gas.Get(), m_Gas.GetSize(),
											&handle, NULL, 0)))
			{
				return false;
			}
			m_GasHandle = handle;
			return true;
		}

		//	Static: build, then compact into m_Gas
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
		if (OPTIX_CHECK(optixAccelBuild(context, stream, &options, &input, 1,
										temp.Get(), temp.GetSize(), output.Get(), output.GetSize(),
										&handle, &emit, 1)))
		{
			return false;
		}
		uint64_t size = 0;
		if (!compacted_size.Download(&size, sizeof(size)))	// waits for the build
		{
			return false;
		}
		if (size == 0 || size >= output.GetSize())
		{
			m_Gas = std::move(output);
			m_GasHandle = handle;
			return true;
		}
		m_Gas.Free();
		if (!m_Gas.Reserve(size) ||
			OPTIX_CHECK(optixAccelCompact(context, stream, handle, m_Gas.Get(), m_Gas.GetSize(), &handle)) ||
			!optxContext::Synchronize())	// before the uncompacted output is freed
		{
			return false;
		}
		m_GasHandle = handle;
		return true;
	}

	OptixTraversableHandle g3dFragmentOptiX::EnsureGAS()
	{
		const uint64_t vertex_generation = m_pStore->GetGeneration();
		const uint64_t index_generation = m_IndexGeneration.load();
		if (vertex_generation == m_GasVertexGeneration && index_generation == m_GasIndexGeneration)
		{
			return m_GasHandle;
		}

		const unsigned triangles = static_cast<unsigned>(m_Indices.size() / 3);
		g3dVertexStoreOptiX::DeviceVertices vertices;
		bool ok = m_PrimitiveType == e_TriangleList && triangles > 0 && m_pStore->GetNumVertices() > 0 &&
				  m_pStore->EnsureDevice(vertices) && vertices.m_Count > 0 && upload_indices(triangles);
		if (ok && m_MaxIndex >= vertices.m_Count)
		{
			DBG_LOG("GraphicsOptiX: a fragment has index " << m_MaxIndex << " but only " << vertices.m_Count
					<< " vertices; it gets no GAS");
			ok = false;
		}

		if (ok)
		{
			const bool refit = GetMorphable() && m_GasHandle != 0 && index_generation == m_GasIndexGeneration &&
							   vertices.m_Count == m_GasVertexCount && triangles == m_GasTriangleCount &&
							   m_nRefitsSinceBuild < c_MaxRefits;
			ok = build_gas(vertices, triangles, refit);
			if (ok)
			{
				if (refit)
				{
					++m_nRefitsSinceBuild;
					++m_nGasRefits;
				}
				else
				{
					m_nRefitsSinceBuild = 0;
					++m_nGasBuilds;
				}
				m_GasVertexCount = vertices.m_Count;
				m_GasTriangleCount = triangles;
			}
		}
		if (!ok)
		{
			release_gas();
		}

		//	Record what the GAS (or its absence) is from: the generation actually
		//	uploaded, which may be newer than the one read above
		m_GasVertexGeneration = ok ? vertices.m_Generation : vertex_generation;
		m_GasIndexGeneration = index_generation;
		return m_GasHandle;
	}

	void g3dFragmentOptiX::Deallocate()
	{
		release_gas();
		m_DeviceIndices.Free();
		m_DeviceIndexGeneration = 0;
		m_pStore->Deallocate();
		m_IndexGeneration = g3dVertexStoreOptiX::NewGeneration();
	}

	//	EnsureGAS remakes what Deallocate freed
	void g3dFragmentOptiX::Reallocate()
	{
	}

	void g3dFragmentOptiX::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		m_pStore->UpdateVertices(i_NumVertices, i_pVertices, i_pNormals);
		update_bounding_box();
	}

	int g3dFragmentOptiX::GetNumVertices() const
	{
		return m_pStore->GetNumVertices();
	}

	int g3dFragmentOptiX::GetNumIndices() const
	{
		return static_cast<int>(m_Indices.size());
	}

	g3dType::VertexFormat g3dFragmentOptiX::GetVertexFormat() const
	{
		return g3dType::e_BumpTex1Vertex;
	}

	unsigned char* g3dFragmentOptiX::Lock()
	{
		std::vector<g3dType::BumpTex1Vertex>& vertices = m_pStore->GetVertices();
		return vertices.empty() ? NULL : reinterpret_cast<unsigned char*>(vertices.data());
	}

	void g3dFragmentOptiX::Unlock()
	{
		m_pStore->Publish();
		update_bounding_box();
	}

	unsigned char* g3dFragmentOptiX::LockIndices()
	{
		return m_Indices.empty() ? NULL : reinterpret_cast<unsigned char*>(m_Indices.data());
	}

	void g3dFragmentOptiX::UnlockIndices()
	{
		m_IndexGeneration = g3dVertexStoreOptiX::NewGeneration();
	}

	unsigned char* g3dFragmentOptiX::ReadOnlyLock()
	{
		return Lock();
	}

	void g3dFragmentOptiX::ReadOnlyUnlock()
	{
	}

	unsigned char* g3dFragmentOptiX::ReadOnlyLockIndices()
	{
		return LockIndices();
	}

	void g3dFragmentOptiX::ReadOnlyUnlockIndices()
	{
	}

	void g3dFragmentOptiX::ComponentSort(const maMatrix4x4& /*i_Transform*/, const maPoint3d& /*i_CameraPos*/)
	{
	}
}
