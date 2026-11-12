/*****************************************************************************
**  g3dSceneBVHOptiXTest.cpp
**
**      g3dSceneBVHOptiX: one fragment in two nodes (the row-vector to
**	OptiX transform, translation and non-uniform scale included; instance
**	IDs; the instance table; bounds); visibility masks (shadow hulls,
**	shadow casters); the walk's filters; and change detection (nothing
**	changed: nothing built; a node moved: only the IAS; a morphable
**	fragment moved: only its GAS refit; an index count change: a full GAS
**	build; a node removed; Release).
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#include "GraphicsOptiX/optx/optxRayQuery.hpp"
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace gox;

namespace
{
	const unsigned c_Camera = e_optxMaskCamera;
	const unsigned c_Occlusion = e_optxMaskOcclusion;

	//	A hit: {distance or -1 for a miss, instance ID}
	typedef std::pair<float, unsigned> Hit;
	const Hit c_Miss(-1.0f, ~0u);

	//	One triangle covering (0, 0) in the plane z: (-1,-1) (1,-1) (0,1)
	std::unique_ptr<g3dFragmentOptiX> make_triangle(float z, bool i_bMorphable = false)
	{
		const maPoint3d positions[3] = { maPoint3d(-1, -1, z), maPoint3d(1, -1, z), maPoint3d(0, 1, z) };
		const uint32_t indices[3] = { 0, 1, 2 };
		auto fragment = std::make_unique<g3dFragmentOptiX>(positions, nullptr, nullptr, nullptr, nullptr, 3,
														   g3dIndexPtr(indices), 3, nullptr, i_bMorphable,
														   g3dFragmentOptiX::e_TriangleList);
		fragment->SetCastsShadow(true);
		return fragment;
	}

	//	Row-vector matrix: scale, then translate
	maMatrix4x4 scale_translate(float sx, float sy, float sz, float tx, float ty, float tz)
	{
		return maMatrix4x4(sx, 0, 0, 0,
						   0, sy, 0, 0,
						   0, 0, sz, 0,
						   tx, ty, tz, 1);
	}

	//	A scene of one world-space layer; the test owns the root (layers
	//	don't delete their nodes), the root owns its children
	struct TestScene
	{
		std::unique_ptr<g3dSceneNode>	m_pRoot;
		std::unique_ptr<g3dScene>		m_pScene;

		TestScene()
			: m_pRoot(new g3dSceneNode())
			, m_pScene(new g3dScene(new g3dLayer(m_pRoot.get())))
		{
		}

		~TestScene()
		{
			m_pScene.reset();
			m_pRoot.reset();
		}

		g3dSceneNode* Add(g3dFragment* i_pFragment, const maMatrix4x4& i_Transform = maMatrix4x4())
		{
			g3dSceneNode* node = new g3dSceneNode(i_pFragment);
			node->SetTransform(i_Transform);
			m_pRoot->AddChild(node);
			node->UpdateTotalTransform();
			return node;
		}
	};

	class g3dSceneBVHOptiXTest : public gtOptiX
	{
		protected:
			void SetUp() override
			{
				gtOptiX::SetUp();
				if (IsSkipped())
				{
					return;
				}
				std::string error;
				ASSERT_TRUE(m_Query.Create(error)) << error;
			}

			bool update(const TestScene& i_Scene, bool i_bLowResolution = false)
			{
				return m_Bvh.Update(*i_Scene.m_pScene, m_ViewerLayers, i_bLowResolution);
			}

			//	A ray down -z from (x, y, 5)
			Hit trace_down(float x, float y, unsigned i_Mask = c_Camera)
			{
				optxRayQueryRay ray = {};
				ray.m_Origin[0] = x;
				ray.m_Origin[1] = y;
				ray.m_Origin[2] = 5.0f;
				ray.m_Direction[2] = -1.0f;
				ray.m_TMax = 1e30f;
				ray.m_Mask = i_Mask;
				std::vector<optxRayQueryHit> hits;
				EXPECT_TRUE(m_Query.Trace(m_Bvh.GetHandle(), { ray }, hits));
				return hits.size() == 1 && hits[0].m_Hit ? Hit(hits[0].m_T, hits[0].m_InstanceId) : c_Miss;
			}

			optxRayQuery			m_Query;
			g3dSceneBVHOptiX		m_Bvh;
			std::vector<g3dLayer*>	m_ViewerLayers;
	};
}

TEST_F(g3dSceneBVHOptiXTest, EmptyScene)
{
	TestScene scene;
	EXPECT_TRUE(update(scene));		// the first Update always builds
	EXPECT_EQ(0u, m_Bvh.GetHandle());
	EXPECT_EQ(0u, m_Bvh.GetInstanceCount());
	EXPECT_TRUE(m_Bvh.GetBounds().IsEmpty());
	EXPECT_FALSE(update(scene));
	EXPECT_EQ(0, m_Bvh.GetIasBuildCount());
}

TEST_F(g3dSceneBVHOptiXTest, OneFragmentInTwoNodes)
{
	auto fragment = make_triangle(0);
	TestScene scene;
	scene.Add(fragment.get());
	scene.Add(fragment.get(), scale_translate(2, 1, 1, 10, 0, -1));
	ASSERT_TRUE(update(scene));
	ASSERT_NE(0u, m_Bvh.GetHandle());
	EXPECT_EQ(2u, m_Bvh.GetInstanceCount());
	EXPECT_EQ(1, fragment->GetGasBuildCount());

	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
	//	x = 11.5, y = -0.9 is inside only the scaled copy (x within 10 +- 1.9)
	EXPECT_EQ(Hit(6.0f, 1u), trace_down(11.5f, -0.9f));
	EXPECT_EQ(c_Miss, trace_down(1.5f, -0.9f));
	EXPECT_EQ(c_Miss, trace_down(11.95f, -0.9f));

	std::vector<optxInstanceData> table(2);
	ASSERT_TRUE(optxContext::Synchronize());	// the table is uploaded on the context's stream
	ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(table.data(), m_Bvh.GetInstanceTable(), table.size() * sizeof(optxInstanceData)));
	for (const optxInstanceData& data : table)
	{
		EXPECT_EQ(fragment->GetDeviceVertices(), data.m_Vertices);
		EXPECT_EQ(fragment->GetDeviceIndices(), data.m_Indices);
		EXPECT_EQ(sizeof(g3dType::BumpTex1Vertex), data.m_VertexStride);
		EXPECT_EQ(1u, data.m_NumTriangles);
	}

	const maAxisBox& bounds = m_Bvh.GetBounds();
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinX());
	EXPECT_FLOAT_EQ(12.0f, bounds.GetMaxX());
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinY());
	EXPECT_FLOAT_EQ(1.0f, bounds.GetMaxY());
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinZ());
	EXPECT_FLOAT_EQ(0.0f, bounds.GetMaxZ());
}

TEST_F(g3dSceneBVHOptiXTest, VisibilityMasks)
{
	auto hull = make_triangle(0);		// a shadow hull: occlusion rays only
	hull->SetShadowHull(true);
	auto floor = make_triangle(-2);		// casts no shadow: camera rays only
	floor->SetCastsShadow(false);
	auto nothing = make_triangle(-4);	// a hull casting no shadow: not listed
	nothing->SetShadowHull(true);
	nothing->SetCastsShadow(false);
	TestScene scene;
	scene.Add(hull.get());
	scene.Add(floor.get());
	scene.Add(nothing.get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_Bvh.GetInstanceCount());
	EXPECT_EQ(Hit(7.0f, 1u), trace_down(0, 0, c_Camera));
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0, c_Occlusion));
	EXPECT_EQ(0, nothing->GetGasBuildCount());
}

TEST_F(g3dSceneBVHOptiXTest, Filters)
{
	auto shown = make_triangle(0);
	auto hidden = make_triangle(1);
	auto inactive = make_triangle(2);
	auto low_res = make_triangle(3);
	auto other_space = make_triangle(4);
	auto viewer = make_triangle(-1);
	TestScene scene;
	scene.Add(shown.get());
	scene.Add(hidden.get())->SetRenderable(false);
	scene.Add(inactive.get())->SetActiveInRenderLayer(false);
	scene.Add(low_res.get())->SetContentResolution(g3dSceneNode::e_LowRes);

	std::unique_ptr<g3dSceneNode> other_root(new g3dSceneNode(other_space.get()));
	std::unique_ptr<g3dLayer> other_layer(new g3dLayer(other_root.get()));
	other_layer->SetModelSpace(g3dLayer::e_Camera);
	std::unique_ptr<g3dSceneNode> viewer_root(new g3dSceneNode(viewer.get()));
	std::unique_ptr<g3dLayer> viewer_layer(new g3dLayer(viewer_root.get()));
	m_ViewerLayers = { other_layer.get(), viewer_layer.get() };

	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_Bvh.GetInstanceCount());	// shown and the viewer's
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));

	//	At low resolution the low-resolution content is drawn too
	ASSERT_TRUE(update(scene, true));
	EXPECT_EQ(3u, m_Bvh.GetInstanceCount());
	EXPECT_EQ(Hit(2.0f, 1u), trace_down(0, 0));
	m_ViewerLayers.clear();
}

TEST_F(g3dSceneBVHOptiXTest, NothingChangedBuildsNothing)
{
	auto fragment = make_triangle(0);
	TestScene scene;
	scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	const uint64_t version = m_Bvh.GetSceneVersion();
	const OptixTraversableHandle handle = m_Bvh.GetHandle();

	EXPECT_FALSE(update(scene));
	EXPECT_FALSE(update(scene));
	EXPECT_EQ(version, m_Bvh.GetSceneVersion());
	EXPECT_EQ(handle, m_Bvh.GetHandle());
	EXPECT_EQ(1, m_Bvh.GetIasBuildCount());
	EXPECT_EQ(1, fragment->GetGasBuildCount());
	EXPECT_EQ(1, fragment->GetStore()->GetUploadCount());
}

TEST_F(g3dSceneBVHOptiXTest, MovingANodeRebuildsOnlyTheIas)
{
	auto fragment = make_triangle(0);
	TestScene scene;
	g3dSceneNode* node = scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	const uint64_t version = m_Bvh.GetSceneVersion();

	node->SetTransform(scale_translate(1, 1, 1, 0, 0, -3));
	node->UpdateTotalTransform();
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(version + 1, m_Bvh.GetSceneVersion());
	EXPECT_EQ(2, m_Bvh.GetIasBuildCount());
	EXPECT_EQ(1, fragment->GetGasBuildCount());
	EXPECT_EQ(Hit(8.0f, 0u), trace_down(0, 0));
}

TEST_F(g3dSceneBVHOptiXTest, MorphableFragmentRefitAlone)
{
	auto still = make_triangle(0);
	auto moving = make_triangle(0, true);
	TestScene scene;
	scene.Add(still.get(), scale_translate(1, 1, 1, -5, 0, 0));
	scene.Add(moving.get(), scale_translate(1, 1, 1, 5, 0, 0));
	ASSERT_TRUE(update(scene));

	const maPoint3d lower[3] = { maPoint3d(-1, -1, -2), maPoint3d(1, -1, -2), maPoint3d(0, 1, -2) };
	moving->UpdateVertices(3, lower);
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(1, moving->GetGasRefitCount());
	EXPECT_EQ(1, moving->GetGasBuildCount());
	EXPECT_EQ(1, still->GetGasBuildCount());
	EXPECT_EQ(0, still->GetGasRefitCount());
	EXPECT_EQ(1, still->GetStore()->GetUploadCount());
	EXPECT_EQ(Hit(7.0f, 1u), trace_down(5, 0));
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(-5, 0));
}

TEST_F(g3dSceneBVHOptiXTest, IndexCountChangeRebuildsTheGas)
{
	//	Two triangles, the second lower; the indices grow from one to both
	const maPoint3d positions[6] = { maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0),
									 maPoint3d(2, -1, -1), maPoint3d(4, -1, -1), maPoint3d(3, 1, -1) };
	const uint32_t indices[3] = { 0, 1, 2 };
	g3dFragmentOptiX fragment(positions, nullptr, nullptr, nullptr, nullptr, 6, g3dIndexPtr(indices), 3, nullptr, true,
							  g3dFragmentOptiX::e_TriangleList);
	TestScene scene;
	scene.Add(&fragment);
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(c_Miss, trace_down(3, 0));

	fragment.SetIndices({ 0, 1, 2, 3, 4, 5 });
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(2, fragment.GetGasBuildCount());
	EXPECT_EQ(0, fragment.GetGasRefitCount());
	EXPECT_EQ(Hit(6.0f, 0u), trace_down(3, 0));
}

TEST_F(g3dSceneBVHOptiXTest, RemovingANode)
{
	auto a = make_triangle(0);
	auto b = make_triangle(-1);
	TestScene scene;
	scene.Add(a.get());
	g3dSceneNode* node = scene.Add(b.get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_Bvh.GetInstanceCount());

	scene.m_pRoot->RemoveChild(node);
	delete node;
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(1u, m_Bvh.GetInstanceCount());
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
}

TEST_F(g3dSceneBVHOptiXTest, ReleaseThenRebuild)
{
	auto fragment = make_triangle(0);
	TestScene scene;
	scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	m_Bvh.Release();
	EXPECT_EQ(0u, m_Bvh.GetHandle());
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(2, m_Bvh.GetIasBuildCount());
	EXPECT_EQ(1, fragment->GetGasBuildCount());	// the GAS belongs to the fragment
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
}
