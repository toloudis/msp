/*****************************************************************************
**  bumpFragmentVkTest.cpp
**
**      GraphicsVulkan's tmesh and bump fragments against GraphicsDX11's:
**      the same meshes made through g3dFragmentCreate under each layer
**      (gtBackend) give the same vertex format, vertex bytes (basis
**      vectors computed), index width and bytes, bounding box, sub-
**      fragments and bake factors.
**
**      Skipped without a Vulkan device (the DX11 half still runs).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtScene.hpp"

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif

//	DX11
#ifndef TMESH_FRAG_HPP
#include "GraphicsDX11/tmesh/tmeshFrag.hpp"
#endif

//	Vulkan
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif

#include <gtest/gtest.h>

#include <cstring>
#include <memory>
#include <vector>

namespace
{
	//	What a fragment holds, read through g3dFragment (and the backend's
	//	tmeshFrag for the index width)
	struct FragmentData
	{
		int							m_Format = -1;
		int							m_Vertices = 0;
		std::vector<uint8_t>		m_VertexBytes;
		int							m_IndexSize = 0;
		int							m_Indices = 0;
		std::vector<uint8_t>		m_IndexBytes;
		float						m_Box[6] = {};
		std::vector<int>			m_SubFragments;		// start index, triangle count, ...
		std::vector<float>			m_SubBoxes;
		bool						m_bVelocity = false;
		float						m_Bake[4] = {};
		float						m_Overlap = 0;
	};

	void read_box(const maAxisBox& i_Box, float* o_Six)
	{
		o_Six[0] = i_Box.GetMinX(); o_Six[1] = i_Box.GetMinY(); o_Six[2] = i_Box.GetMinZ();
		o_Six[3] = i_Box.GetMaxX(); o_Six[4] = i_Box.GetMaxY(); o_Six[5] = i_Box.GetMaxZ();
	}

	FragmentData read(gtBackend::Kind i_Kind, const gtMesh& i_Mesh, bool i_bMorphable)
	{
		FragmentData data;
		std::unique_ptr<g3dFragment> fragment(i_Mesh.CreateFragment(NULL, i_bMorphable));
		EXPECT_TRUE(fragment);
		if (!fragment)
			return data;

		data.m_Format = fragment->GetVertexFormat();
		data.m_Vertices = fragment->GetNumVertices();
		const uint8_t* vertices = fragment->ReadOnlyLock();
		data.m_VertexBytes.assign(vertices, vertices + data.m_Vertices * sizeof(g3dType::BumpTex1Vertex));
		fragment->ReadOnlyUnlock();

		if (i_Kind == gtBackend::e_DX11)
		{
			const tmeshFrag* frag = dynamic_cast<const tmeshFrag*>(fragment.get());
			EXPECT_TRUE(frag) << "DX11: not a tmeshFrag";
			data.m_IndexSize = frag ? frag->GetSizeOfIndex() : 0;
		}
		else
		{
			const gvk::tmeshFrag* frag = dynamic_cast<const gvk::tmeshFrag*>(fragment.get());
			EXPECT_TRUE(frag) << "Vulkan: not a gvk::tmeshFrag";
			data.m_IndexSize = frag ? frag->GetSizeOfIndex() : 0;
		}
		data.m_Indices = fragment->GetNumIndices();
		const uint8_t* indices = fragment->ReadOnlyLockIndices();
		data.m_IndexBytes.assign(indices, indices + data.m_Indices * data.m_IndexSize);
		fragment->ReadOnlyUnlockIndices();

		read_box(fragment->GetBoundingBox(), data.m_Box);
		data.m_bVelocity = fragment->GetHasVelocityBuffer();
		maPoint2d scale, translate;
		fragment->GetUVBakeFactors(scale, translate);
		data.m_Bake[0] = scale.m_X; data.m_Bake[1] = scale.m_Y;
		data.m_Bake[2] = translate.m_X; data.m_Bake[3] = translate.m_Y;
		data.m_Overlap = fragment->GetUVOverlapFactor();

		fragment->Split(fragment->GetBoundingBox());
		for (const g3dFragment::sSubFrag& sub : fragment->GetSubFragments())
		{
			data.m_SubFragments.push_back(sub.m_startIndex);
			data.m_SubFragments.push_back(sub.m_numTris);
			float box[6];
			read_box(sub.m_bounds, box);
			data.m_SubBoxes.insert(data.m_SubBoxes.end(), box, box + 6);
		}
		return data;
	}

	void expect_same(const gtMesh& i_Mesh, bool i_bMorphable)
	{
		FragmentData dx11, vulkan;
		gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = read(gtBackend::e_DX11, i_Mesh, i_bMorphable); });
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = read(gtBackend::e_Vulkan, i_Mesh, i_bMorphable); }))
			return;

		EXPECT_EQ(g3dType::e_BumpTex1Vertex, dx11.m_Format);
		EXPECT_EQ(dx11.m_Format, vulkan.m_Format);
		EXPECT_EQ(dx11.m_Vertices, vulkan.m_Vertices);
		EXPECT_TRUE(dx11.m_VertexBytes == vulkan.m_VertexBytes) << "vertex bytes differ";
		EXPECT_EQ(dx11.m_IndexSize, vulkan.m_IndexSize);
		EXPECT_EQ(dx11.m_Indices, vulkan.m_Indices);
		EXPECT_TRUE(dx11.m_IndexBytes == vulkan.m_IndexBytes) << "index bytes differ";
		EXPECT_EQ(0, memcmp(dx11.m_Box, vulkan.m_Box, sizeof(dx11.m_Box))) << "bounding boxes differ";
		EXPECT_EQ(dx11.m_SubFragments, vulkan.m_SubFragments);
		EXPECT_EQ(dx11.m_SubBoxes, vulkan.m_SubBoxes);
		EXPECT_EQ(dx11.m_bVelocity, vulkan.m_bVelocity);
		EXPECT_EQ(0, memcmp(dx11.m_Bake, vulkan.m_Bake, sizeof(dx11.m_Bake))) << "bake factors differ";
		EXPECT_EQ(dx11.m_Overlap, vulkan.m_Overlap);
	}
}

TEST(bumpFragmentVkTest, SameVertexAndIndexData)
{
	{
		SCOPED_TRACE("sphere");
		expect_same(gtScene::Sphere(12, 24), false);
	}
	{
		SCOPED_TRACE("sphere, morphable");
		expect_same(gtScene::Sphere(12, 24), true);
	}
	{
		SCOPED_TRACE("plane");
		expect_same(gtScene::Plane(), false);
	}
	{
		SCOPED_TRACE("grid 300 (32-bit indices)");
		const gtMesh grid = gtScene::Grid(300);
		ASSERT_GT(grid.VertexCount(), 65535);
		expect_same(grid, false);
	}
}
