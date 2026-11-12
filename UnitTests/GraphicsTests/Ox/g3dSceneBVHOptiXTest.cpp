/*****************************************************************************
**  g3dSceneBVHOptiXTest.cpp
**
**      g3dSceneBVHOptiX on GraphicsVulkan's fragments: one fragment in two
**	nodes (the row-vector to OptiX transform, translation and non-uniform
**	scale included; instance IDs; the instance table; bounds); visibility
**	masks (shadow hulls, shadow casters); the walk's filters; and change
**	detection (nothing changed: nothing built; a node moved: only the IAS;
**	a morphable fragment moved: only its GAS refit; an index buffer
**	replaced: a GAS build; a node removed; Release).
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GVK_BUMP_BUFFERUTIL_HPP
#include "GraphicsVulkan/bump/bumpBufferUtil.hpp"
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
				m_pBvh.reset(new g3dSceneBVHOptiX(Cache()));
				m_pBvh->SetPooling(g3dSceneBVHOptiX::e_PoolingGroup);
			}

			void TearDown() override
			{
				m_pBvh.reset();
				m_ViewerLayers.clear();
				gtOptiX::TearDown();
			}

			bool update(const gtOx::Scene& i_Scene, bool i_bLowResolution = false)
			{
				return m_pBvh->Update(*i_Scene.m_pScene, m_ViewerLayers, i_bLowResolution);
			}

			Hit trace_down(float x, float y, unsigned i_Mask = c_Camera)
			{
				std::vector<optxRayQueryHit> hits;
				EXPECT_TRUE(m_Query.Trace(m_pBvh->GetHandle(), { gtOx::Down(x, y, 5.0f, i_Mask) }, hits, m_pBvh->GetSbtRecordCount()));
				return hits.size() == 1 && hits[0].m_Hit ? Hit(hits[0].m_T, hits[0].m_InstanceId) : c_Miss;
			}

			const g3dGasCacheOptiX::Stats& stats() { return Cache().GetStats(); }

			optxRayQuery						m_Query;
			std::unique_ptr<g3dSceneBVHOptiX>	m_pBvh;
			std::vector<g3dLayer*>				m_ViewerLayers;
	};
}

TEST_F(g3dSceneBVHOptiXTest, EmptyScene)
{
	gtOx::Scene scene;
	EXPECT_TRUE(update(scene));		// the first Update always builds
	EXPECT_EQ(0u, m_pBvh->GetHandle());
	EXPECT_EQ(0u, m_pBvh->GetInstanceCount());
	EXPECT_TRUE(m_pBvh->GetBounds().IsEmpty());
	EXPECT_FALSE(update(scene));
	EXPECT_EQ(0, m_pBvh->GetIasBuildCount());
}

TEST_F(g3dSceneBVHOptiXTest, OneFragmentInTwoNodes)
{
	auto fragment = gtOx::Triangle(0);
	gtOx::Scene scene;
	scene.Add(fragment.get());
	scene.Add(fragment.get(), gtOx::ScaleTranslate(2, 1, 1, 10, 0, -1));
	ASSERT_TRUE(update(scene));
	ASSERT_NE(0u, m_pBvh->GetHandle());
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(1u, m_pBvh->GetGasCount());
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(2u, m_pBvh->GetTriangleCount());

	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
	//	x = 11.5, y = -0.9 is inside only the scaled copy (x within 10 +- 1.9)
	EXPECT_EQ(Hit(6.0f, 1u), trace_down(11.5f, -0.9f));
	EXPECT_EQ(c_Miss, trace_down(1.5f, -0.9f));
	EXPECT_EQ(c_Miss, trace_down(11.95f, -0.9f));

	const g3dGasCacheOptiX::Gas* gas = Cache().Ensure(*fragment);
	ASSERT_NE(nullptr, gas);
	std::vector<optxInstanceData> table(2);
	ASSERT_TRUE(optxContext::Synchronize());	// the table is uploaded on the context's stream
	ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(table.data(), m_pBvh->GetInstanceTable(), table.size() * sizeof(optxInstanceData)));
	for (const optxInstanceData& data : table)
	{
		EXPECT_EQ(gas->m_Vertices, data.m_Vertices);
		EXPECT_EQ(sizeof(g3dType::BumpTex1Vertex), data.m_VertexStride);
		EXPECT_EQ(gas->m_NormalOffset, data.m_NormalOffset);
		EXPECT_EQ(1u, data.m_NumInputs);
		EXPECT_EQ(1u, data.m_NumTriangles);
		optxGeometryInput input = {};
		ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(&input, data.m_Inputs, sizeof(input)));
		EXPECT_EQ(gas->m_Inputs[0].m_Indices, input.m_Indices);
		EXPECT_EQ(2u, input.m_IndexSize);
		EXPECT_EQ(1u, input.m_NumTriangles);
	}

	const maAxisBox& bounds = m_pBvh->GetBounds();
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinX());
	EXPECT_FLOAT_EQ(12.0f, bounds.GetMaxX());
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinY());
	EXPECT_FLOAT_EQ(1.0f, bounds.GetMaxY());
	EXPECT_FLOAT_EQ(-1.0f, bounds.GetMinZ());
	EXPECT_FLOAT_EQ(0.0f, bounds.GetMaxZ());
}

TEST_F(g3dSceneBVHOptiXTest, VisibilityMasks)
{
	auto hull = gtOx::Triangle(0);		// a shadow hull: occlusion rays only
	hull->SetShadowHull(true);
	auto floor = gtOx::Triangle(-2);	// casts no shadow: camera rays only
	floor->SetCastsShadow(false);
	auto nothing = gtOx::Triangle(-4);	// a hull casting no shadow: not listed
	nothing->SetShadowHull(true);
	nothing->SetCastsShadow(false);
	gtOx::Scene scene;
	scene.Add(hull.get());
	scene.Add(floor.get());
	scene.Add(nothing.get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(Hit(7.0f, 1u), trace_down(0, 0, c_Camera));
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0, c_Occlusion));
	EXPECT_EQ(2, stats().m_Builds);		// not the unlisted one
}

TEST_F(g3dSceneBVHOptiXTest, Filters)
{
	auto shown = gtOx::Triangle(0);
	auto hidden = gtOx::Triangle(1);
	auto inactive = gtOx::Triangle(2);
	auto low_res = gtOx::Triangle(3);
	auto other_space = gtOx::Triangle(4);
	auto viewer = gtOx::Triangle(-1);
	gtOx::Scene scene;
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
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());	// shown and the viewer's
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));

	//	At low resolution the low-resolution content is drawn too
	ASSERT_TRUE(update(scene, true));
	EXPECT_EQ(3u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(Hit(2.0f, 1u), trace_down(0, 0));
	m_ViewerLayers.clear();
}

TEST_F(g3dSceneBVHOptiXTest, NothingChangedBuildsNothing)
{
	auto fragment = gtOx::Triangle(0);
	gtOx::Scene scene;
	scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	const uint64_t version = m_pBvh->GetSceneVersion();
	const OptixTraversableHandle handle = m_pBvh->GetHandle();

	EXPECT_FALSE(update(scene));
	EXPECT_FALSE(update(scene));
	EXPECT_EQ(version, m_pBvh->GetSceneVersion());
	EXPECT_EQ(handle, m_pBvh->GetHandle());
	EXPECT_EQ(1, m_pBvh->GetIasBuildCount());
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(0, stats().m_Uploads);
}

TEST_F(g3dSceneBVHOptiXTest, MovingANodeRebuildsOnlyTheIas)
{
	auto fragment = gtOx::Triangle(0);
	gtOx::Scene scene;
	g3dSceneNode* node = scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	const uint64_t version = m_pBvh->GetSceneVersion();

	node->SetTransform(gtOx::ScaleTranslate(1, 1, 1, 0, 0, -3));
	node->UpdateTotalTransform();
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(version + 1, m_pBvh->GetSceneVersion());
	EXPECT_EQ(2, m_pBvh->GetIasBuildCount());
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(Hit(8.0f, 0u), trace_down(0, 0));
}

TEST_F(g3dSceneBVHOptiXTest, MorphableFragmentRefitAlone)
{
	auto still = gtOx::Triangle(0);
	auto moving = gtOx::Triangle(0, true);
	gtOx::Scene scene;
	scene.Add(still.get(), gtOx::ScaleTranslate(1, 1, 1, -5, 0, 0));
	scene.Add(moving.get(), gtOx::ScaleTranslate(1, 1, 1, 5, 0, 0));
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2, stats().m_Builds);
	EXPECT_EQ(1, stats().m_Uploads);	// the morphable vertices

	const maPoint3d lower[3] = { maPoint3d(-1, -1, -2), maPoint3d(1, -1, -2), maPoint3d(0, 1, -2) };
	moving->UpdateVertices(3, lower, nullptr);
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(1, stats().m_Refits);
	EXPECT_EQ(2, stats().m_Builds);
	EXPECT_EQ(2, stats().m_Uploads);
	EXPECT_EQ(Hit(7.0f, 1u), trace_down(5, 0));
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(-5, 0));
}

TEST_F(g3dSceneBVHOptiXTest, ReplacedIndexBufferRebuildsTheGas)
{
	//	Two triangles, the second lower; the indices grow from one to both
	auto fragment = gtOx::Mesh({ maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0),
								 maPoint3d(2, -1, -1), maPoint3d(4, -1, -1), maPoint3d(3, 1, -1) }, { 0, 1, 2 }, true);
	gtOx::Scene scene;
	scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(c_Miss, trace_down(3, 0));

	const uint32_t both[6] = { 0, 1, 2, 3, 4, 5 };
	shared_ptr<gvk::tmeshIndexBuffer> indices = gvk::bumpBufferUtil::CreateIndexBuffer(g3dIndexPtr(both), 6, 6, 6, false);
	fragment->UpdateIndexBuffer(indices);
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(2, stats().m_Builds);
	EXPECT_EQ(0, stats().m_Refits);
	EXPECT_EQ(Hit(6.0f, 0u), trace_down(3, 0));
	EXPECT_EQ(1u, Cache().GetEntryCount());		// the rebuild collected the old entry
}

TEST_F(g3dSceneBVHOptiXTest, RemovingANode)
{
	auto a = gtOx::Triangle(0);
	auto b = gtOx::Triangle(-1);
	gtOx::Scene scene;
	scene.Add(a.get());
	g3dSceneNode* node = scene.Add(b.get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());

	scene.m_pRoot->RemoveChild(node);
	delete node;
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(1u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
}

TEST_F(g3dSceneBVHOptiXTest, ReleaseThenRebuild)
{
	auto fragment = gtOx::Triangle(0);
	gtOx::Scene scene;
	scene.Add(fragment.get());
	ASSERT_TRUE(update(scene));
	m_pBvh->Release();
	EXPECT_EQ(0u, m_pBvh->GetHandle());
	EXPECT_TRUE(update(scene));
	EXPECT_EQ(2, m_pBvh->GetIasBuildCount());
	EXPECT_EQ(1, stats().m_Builds);		// the GAS belongs to the cache
	EXPECT_EQ(Hit(5.0f, 0u), trace_down(0, 0));
}
