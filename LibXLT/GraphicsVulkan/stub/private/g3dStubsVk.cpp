/*****************************************************************************
**  g3dStubsVk.cpp
**
**      See g3dStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

namespace gvk
{
	//------------------------------------------------------------------------
	//	hairModelFragVk
	//------------------------------------------------------------------------
	int hairModelFragVk::sm_RendererId = 0;

	void hairModelFragVk::SetRendererId(int i_RenderMode)
	{
		sm_RendererId = i_RenderMode;
	}

	hairModelFragVk::hairModelFragVk(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs,
								 int i_nVertices, const g3dIndexPtr* i_pIndices, int i_nIndices,
								 matMaterial* i_pMaterial, bool i_bMorphable)
		: g3dFragment(i_pMaterial, sm_RendererId, i_bMorphable)
	{
		for (int i = 0; i < i_nVertices; ++i)
		{
			g3dType::BumpTex1Vertex vertex;
			vertex.m_Vertex = i_Vertices ? i_Vertices[i] : maPoint3d(0, 0, 0);
			vertex.m_Normal = i_Normals ? i_Normals[i] : maPoint3d(0, 0, 1);
			vertex.m_TexCoord = i_UVs ? i_UVs[i] : maPoint2d(0, 0);
			vertex.m_S = maPoint3d(1, 0, 0);
			vertex.m_T = maPoint3d(0, 1, 0);
			m_Vertices.push_back(vertex);
		}
		const bool wide = g3dIndexPtr::Needs32Bit(i_nVertices);
		for (int i = 0; i_pIndices && i < i_nIndices; ++i)
		{
			const uint32_t index = i_pIndices->GetIndex(i);
			if (wide)
			{
				m_Indices32.push_back(index);
			}
			else
			{
				m_Indices16.push_back(static_cast<uint16_t>(index));
			}
		}
		update_bounds();
	}

	hairModelFragVk::hairModelFragVk(const hairModelFragVk& i_Other)
		: g3dFragment(i_Other)
		, m_Vertices(i_Other.m_Vertices)
		, m_Indices16(i_Other.m_Indices16)
		, m_Indices32(i_Other.m_Indices32)
	{
	}

	hairModelFragVk::~hairModelFragVk()
	{
	}

	void hairModelFragVk::update_bounds()
	{
		maAxisBox box;
		bool first = true;
		const int count = GetNumVertices();
		for (int i = 0; i < count; ++i)
		{
			const maPoint3d& p = m_Vertices[i].m_Vertex;
			if (first)
			{
				box = maAxisBox(p, p);
				first = false;
			}
			else
			{
				box.Union(p);
			}
		}
		SetBoundingBox(box);
	}

	void hairModelFragVk::Deallocate()	{}
	void hairModelFragVk::Reallocate()	{}

	void hairModelFragVk::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		const int count = std::min(i_NumVertices, GetNumVertices());
		for (int i = 0; i < count; ++i)
		{
			maPoint3d& position = m_Vertices[i].m_Vertex;
			maPoint3d& normal = m_Vertices[i].m_Normal;
			if (i_pVertices)
			{
				position = i_pVertices[i];
			}
			if (i_pNormals)
			{
				normal = maPoint3d(i_pNormals[i].GetX(), i_pNormals[i].GetY(), i_pNormals[i].GetZ());
			}
		}
		update_bounds();
	}

	int hairModelFragVk::GetNumVertices() const
	{
		return static_cast<int>(m_Vertices.size());
	}

	int hairModelFragVk::GetNumIndices() const
	{
		return static_cast<int>(m_Indices16.size() + m_Indices32.size());
	}

	g3dType::VertexFormat hairModelFragVk::GetVertexFormat() const
	{
		return g3dType::e_BumpTex1Vertex;
	}

	unsigned char* hairModelFragVk::Lock()
	{
		return reinterpret_cast<unsigned char*>(m_Vertices.data());
	}

	void hairModelFragVk::Unlock()								{ update_bounds(); }
	unsigned char* hairModelFragVk::LockIndices()
	{
		return m_Indices32.empty() ? reinterpret_cast<unsigned char*>(m_Indices16.data())
								   : reinterpret_cast<unsigned char*>(m_Indices32.data());
	}

	void hairModelFragVk::UnlockIndices()							{}
	unsigned char* hairModelFragVk::ReadOnlyLock()				{ return Lock(); }
	void hairModelFragVk::ReadOnlyUnlock()						{}
	unsigned char* hairModelFragVk::ReadOnlyLockIndices()			{ return LockIndices(); }
	void hairModelFragVk::ReadOnlyUnlockIndices()					{}
	void hairModelFragVk::ComponentSort(const maMatrix4x4&, const maPoint3d&)	{ GVK_NOT_YET("hairModelFragVk::ComponentSort"); }

	//------------------------------------------------------------------------
	//	hairRendererVk
	//------------------------------------------------------------------------
	int hairRendererVk::Render(const g3dSceneNode*, const matMaterial*, matShaderEffect*)
	{
		GVK_NOT_YET("hairRendererVk::Render (hair is Plan 7)");
		return 0;
	}
}
