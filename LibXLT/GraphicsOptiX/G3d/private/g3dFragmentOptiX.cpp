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
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif

#include <algorithm>

namespace gox
{
	namespace
	{
		//	Fragments aren't drawn yet, so no renderer claims them
		const int c_RenderMode = 0;
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
		, m_PrimitiveType(i_PrimitiveType)
	{
		const maPoint3d zero3(0.0f, 0.0f, 0.0f);
		m_Vertices.resize(std::max(i_nVertices, 0));
		for (size_t i = 0; i < m_Vertices.size(); ++i)
		{
			g3dType::BumpTex1Vertex& v = m_Vertices[i];
			v.m_Vertex = i_Vertices ? i_Vertices[i] : zero3;
			v.m_Normal = i_Normals ? i_Normals[i] : zero3;
			v.m_TexCoord = i_UVs ? i_UVs[i] : maPoint2d(0.0f, 0.0f);
			v.m_S = i_Ss ? i_Ss[i] : zero3;
			v.m_T = i_Ts ? i_Ts[i] : zero3;
		}

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
		, m_Vertices(i_Frag.m_Vertices)
		, m_Indices(i_Frag.m_Indices)
		, m_PrimitiveType(i_Frag.m_PrimitiveType)
	{
	}

	g3dFragmentOptiX::~g3dFragmentOptiX()
	{
	}

	void g3dFragmentOptiX::update_bounding_box()
	{
		if (m_Vertices.empty())
		{
			return;
		}
		maPoint3d lo = m_Vertices[0].m_Vertex;
		maPoint3d hi = lo;
		for (const g3dType::BumpTex1Vertex& v : m_Vertices)
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

	//	Nothing is on the device yet
	void g3dFragmentOptiX::Deallocate()
	{
	}

	void g3dFragmentOptiX::Reallocate()
	{
	}

	void g3dFragmentOptiX::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
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
		update_bounding_box();
	}

	int g3dFragmentOptiX::GetNumVertices() const
	{
		return static_cast<int>(m_Vertices.size());
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
		return m_Vertices.empty() ? NULL : reinterpret_cast<unsigned char*>(m_Vertices.data());
	}

	void g3dFragmentOptiX::Unlock()
	{
		update_bounding_box();
	}

	unsigned char* g3dFragmentOptiX::LockIndices()
	{
		return m_Indices.empty() ? NULL : reinterpret_cast<unsigned char*>(m_Indices.data());
	}

	void g3dFragmentOptiX::UnlockIndices()
	{
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
