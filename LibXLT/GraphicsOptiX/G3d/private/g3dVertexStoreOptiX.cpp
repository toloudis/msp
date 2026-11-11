/*****************************************************************************
**  g3dVertexStoreOptiX.cpp
**
**      See g3dVertexStoreOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DVERTEXSTOREOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dVertexStoreOptiX.hpp"
#endif

#include <algorithm>

namespace gox
{
	namespace
	{
		std::atomic<uint64_t> l_NextGeneration(1);
	}

	uint64_t g3dVertexStoreOptiX::NewGeneration()
	{
		return l_NextGeneration.fetch_add(1);
	}

	g3dVertexStoreOptiX::g3dVertexStoreOptiX(bool i_bMorphable)
		: m_bMorphable(i_bMorphable)
		, m_Generation(NewGeneration())
		, m_DeviceCount(0)
		, m_DeviceGeneration(0)
		, m_nUploads(0)
	{
	}

	void g3dVertexStoreOptiX::Publish()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_bMorphable)
		{
			m_Published = m_Vertices;
		}
		m_Generation = NewGeneration();
	}

	void g3dVertexStoreOptiX::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		const size_t count = std::min(static_cast<size_t>(std::max(i_NumVertices, 0)), m_Vertices.size());
		for (size_t i = 0; i < count; ++i)
		{
			if (i_pVertices)
			{
				m_Vertices[i].m_Vertex = i_pVertices[i];
			}
			if (i_pNormals)
			{
				m_Vertices[i].m_Normal = i_pNormals[i];
			}
		}
		Publish();
	}

	bool g3dVertexStoreOptiX::EnsureDevice(DeviceVertices& o_Vertices)
	{
		if (m_DeviceGeneration != m_Generation.load())
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			const std::vector<g3dType::BumpTex1Vertex>& source = m_bMorphable ? m_Published : m_Vertices;
			if (!m_Device.Upload(source.data(), source.size() * sizeof(g3dType::BumpTex1Vertex)))
			{
				m_DeviceCount = 0;
				m_DeviceGeneration = 0;
				o_Vertices = DeviceVertices();
				return false;
			}
			m_DeviceCount = static_cast<unsigned>(source.size());
			m_DeviceGeneration = m_Generation.load();
			++m_nUploads;
		}
		o_Vertices.m_Ptr = m_DeviceCount ? m_Device.Get() : 0;
		o_Vertices.m_Count = m_DeviceCount;
		o_Vertices.m_Generation = m_DeviceGeneration;
		return true;
	}

	void g3dVertexStoreOptiX::Deallocate()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Device.Free();
		m_DeviceCount = 0;
		m_DeviceGeneration = 0;
		m_Generation = NewGeneration();
	}
}
