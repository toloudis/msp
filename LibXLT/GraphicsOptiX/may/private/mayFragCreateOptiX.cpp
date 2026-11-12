/*****************************************************************************
**  mayFragCreateOptiX.cpp
**
**      See mayFragCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_MAYFRAGCREATEOPTIX_HPP
#include "GraphicsOptiX/may/private/mayFragCreateOptiX.hpp"
#endif
#ifndef GOX_G3DFRAGMENTOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#endif
#ifndef GOX_G3DVERTEXBUFFEROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dVertexBufferOptiX.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef MDL_MATINFO_HPP
#include "Graphics/mdl/mdlMatInfo.hpp"
#endif
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace gox
{
	namespace
	{
		template <class T>
		const T* data_or_null(const std::vector<T>& i_Vector)
		{
			return i_Vector.empty() ? NULL : i_Vector.data();
		}

		//	The group's vertices: positions, normals, UVs, S and T (each array
		//	may be shorter than the positions, or empty: the rest is zero)
		std::vector<g3dType::BumpTex1Vertex> group_vertices(const mdlFragInfo& i_Info)
		{
			const maPoint3d zero3(0.0f, 0.0f, 0.0f);
			std::vector<g3dType::BumpTex1Vertex> vertices(i_Info.m_Vertices.size());
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				g3dType::BumpTex1Vertex& v = vertices[i];
				v.m_Vertex = i_Info.m_Vertices[i];
				v.m_Normal = i < i_Info.m_Normals.size() ? i_Info.m_Normals[i] : zero3;
				v.m_TexCoord = i < i_Info.m_UVs.size() ? i_Info.m_UVs[i] : maPoint2d(0.0f, 0.0f);
				v.m_S = i < i_Info.m_Ss.size() ? i_Info.m_Ss[i] : zero3;
				v.m_T = i < i_Info.m_Ts.size() ? i_Info.m_Ts[i] : zero3;
			}
			return vertices;
		}

		//	Material i_Material's indices (mdlFragInfo::GetIndexRange)
		std::vector<uint32_t> material_indices(const mdlFragInfo& i_Info, int i_Material)
		{
			int first = 0;
			int count = static_cast<int>(i_Info.m_Indices.size());
			i_Info.GetIndexRange(i_Material, first, count);
			if (first < 0 || count <= 0 || size_t(first) + size_t(count) > i_Info.m_Indices.size())
			{
				return std::vector<uint32_t>();
			}
			return std::vector<uint32_t>(i_Info.m_Indices.begin() + first, i_Info.m_Indices.begin() + first + count);
		}

		template <class Flags>
		void set_flags(g3dFragment& io_Fragment, const Flags& i_Flags)
		{
			io_Fragment.SetCastsShadow(i_Flags.m_bCastsShadow);
			io_Fragment.SetReceivesShadow(i_Flags.m_bReceivesShadow);
			io_Fragment.SetShadowHull(i_Flags.m_bShadowHull);
			io_Fragment.SetDoubleSided(i_Flags.m_bDoubleSided);
		}
	}

	bool mayFragCreateOptiX::UseBumpFrag(int /*i_NumTexCoords*/, bool /*i_EffectQualified*/)
	{
		return false;
	}

	g3dFragment* mayFragCreateOptiX::CreateFragment(const mdlSplitFragInfo& i_Info, bool i_Morphable)
	{
		if (i_Info.m_Vertices.empty())
		{
			return NULL;
		}
		//	An attribute is used only when there is one per vertex
		const int vertices = static_cast<int>(i_Info.m_Vertices.size());
		const maPoint3d* normals = i_Info.m_Normals.size() == i_Info.m_Vertices.size() ? i_Info.m_Normals.data() : NULL;
		const maPoint2d* uvs = i_Info.m_UVs.size() == i_Info.m_Vertices.size() ? i_Info.m_UVs.data() : NULL;
		const maPoint3d* ss = i_Info.m_Ss.size() == i_Info.m_Vertices.size() ? i_Info.m_Ss.data() : NULL;
		const maPoint3d* ts = i_Info.m_Ts.size() == i_Info.m_Vertices.size() ? i_Info.m_Ts.data() : NULL;
		//	Indices past m_WeldIndex weld stencil shadow volumes: not traced
		const int all_indices = static_cast<int>(i_Info.m_Indices.size());
		const int indices = (i_Info.m_WeldIndex > 0 && i_Info.m_WeldIndex < all_indices) ? i_Info.m_WeldIndex : all_indices;
		g3dFragmentOptiX* fragment = new g3dFragmentOptiX(i_Info.m_Vertices.data(), normals, uvs, ss, ts, vertices,
														  g3dIndexPtr(data_or_null(i_Info.m_Indices)), indices,
														  i_Info.m_Material, i_Morphable, g3dFragmentOptiX::e_TriangleList);
		set_flags(*fragment, i_Info.m_Flags);
		return fragment;
	}

	g3dFragment* mayFragCreateOptiX::CreateHairFragment(const mdlHairInfo& /*i_Info*/, matMaterial* /*i_pMaterial*/)
	{
		return NULL;
	}

	void mayFragCreateOptiX::OptimizeFragment(mdlSplitFragInfo& /*io_Info*/)
	{
	}

	g3dVertexBuffer* mayFragCreateOptiX::CreateFragmentGroup(const mdlFragInfo& i_Info,
															 std::vector<g3dFragment*>& o_Fragments,
															 bool i_Morphable)
	{
		o_Fragments.clear();
		if (i_Info.m_Vertices.empty())
		{
			return NULL;
		}
		const int materials = static_cast<int>(i_Info.m_Materials.size());
		DBG_ASSERT(materials > 0, "Attempting to create fragment group without material.");
		DBG_ASSERT(i_Info.m_MaterialChanges.size() == size_t(materials - 1), "Incorrect number of material changes.");

		std::shared_ptr<g3dVertexStoreOptiX> store = std::make_shared<g3dVertexStoreOptiX>(i_Morphable);
		std::unique_ptr<g3dVertexBufferOptiX> buffer(new g3dVertexBufferOptiX(store));
		buffer->SetVertices(group_vertices(i_Info));

		o_Fragments.reserve(materials);
		for (int mi = 0; mi < materials; ++mi)
		{
			matMaterial* material = i_Info.m_Materials[mi] ? i_Info.m_Materials[mi]->m_pMaterial : NULL;
			g3dFragmentOptiX* fragment = new g3dFragmentOptiX(store, material_indices(i_Info, mi), material);
			set_flags(*fragment, i_Info.m_Flags);
			o_Fragments.push_back(fragment);
		}
		return buffer.release();
	}

	void mayFragCreateOptiX::UpdateFragmentGroup(const mdlFragInfo& i_Info,
												 g3dVertexBuffer& io_SharedVertexBuffer,
												 std::vector<g3dFragment*>& io_Fragments,
												 bool /*i_Morphable*/)
	{
		if (i_Info.m_Vertices.empty())
		{
			return;
		}
		g3dVertexBufferOptiX* buffer = dynamic_cast<g3dVertexBufferOptiX*>(&io_SharedVertexBuffer);
		DBG_ASSERT(buffer, "UpdateFragmentGroup: the vertex buffer isn't GraphicsOptiX's");
		DBG_ASSERT(io_Fragments.size() == i_Info.m_Materials.size(), "Incorrect number of fragments in group for updating.");
		if (!buffer)
		{
			return;
		}
		buffer->SetVertices(group_vertices(i_Info));
		const size_t materials = std::min(io_Fragments.size(), i_Info.m_Materials.size());
		for (size_t mi = 0; mi < materials; ++mi)
		{
			if (g3dFragmentOptiX* fragment = dynamic_cast<g3dFragmentOptiX*>(io_Fragments[mi]))
			{
				fragment->SetIndices(material_indices(i_Info, static_cast<int>(mi)));
			}
		}
	}
}
