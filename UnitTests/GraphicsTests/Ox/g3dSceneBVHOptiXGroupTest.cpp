/*****************************************************************************
**  g3dSceneBVHOptiXGroupTest.cpp
**
**      Fragment groups (one fragment per material, sharing one vertex
**	buffer: GraphicsVulkan's mayFragCreate::CreateFragmentGroup) in
**	g3dSceneBVHOptiX: with group pooling, a two-material group under one
**	transform is one GAS and one instance, and rays report the right build
**	input for each half; hiding one material falls back to a GAS each;
**	pooling none and group give the same hits; the group's vertex buffer
**	written through Lock/Unlock is one refit; UpdateFragmentGroup (new
**	vertices and indices) rebuilds. And a glTF file loaded through
**	gltfImport with mayFragCreate installed (fragment and instance counts,
**	bounds, rays).
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GVK_MAY_FRAGCREATE_HPP
#include "GraphicsVulkan/may/mayFragCreate.hpp"
#endif
#ifndef GLTF_IMPORT_HPP
#include "ImportExport/gltf/import/gltfImport.hpp"
#endif
#ifndef EMDL_CHARACTERTEMPLATE_HPP
#include "Graphics/emdl/emdlCharacterTemplate.hpp"
#endif
#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef G3D_VERTEXBUFFER_HPP
#include "Graphics/G3d/g3dVertexBuffer.hpp"
#endif

#include "gtTempDir.hpp"
// the glTF model builders the ImportExport tests use
#include "../ImportExportTests/ietGltfModel.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace gox;

namespace
{
	//	A hit: {distance or -1 for a miss, instance ID, build input, triangle}
	struct Hit
	{
		float		m_T;
		unsigned	m_Instance;
		unsigned	m_Input;
		unsigned	m_Triangle;
	};

	class g3dSceneBVHOptiXGroupTest : public gtOptiX
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
				m_Group.clear();
				m_pGroupBuffer.reset();
				gtOptiX::TearDown();
			}

			bool update(const gtOx::Scene& i_Scene)
			{
				return m_pBvh->Update(*i_Scene.m_pScene, std::vector<g3dLayer*>(), false);
			}

			Hit trace_down(float x, float y, float i_Z = 5.0f)
			{
				std::vector<optxRayQueryHit> hits;
				EXPECT_TRUE(m_Query.Trace(m_pBvh->GetHandle(), { gtOx::Down(x, y, i_Z) }, hits, m_pBvh->GetSbtRecordCount()));
				if (hits.size() != 1 || !hits[0].m_Hit)
				{
					return { -1.0f, ~0u, ~0u, ~0u };
				}
				return { hits[0].m_T, hits[0].m_InstanceId, hits[0].m_BuildInput, hits[0].m_PrimitiveIndex };
			}

			//	A group from mayFragCreate, kept by the test
			void make_group(const mdlFragInfo& i_Info, bool i_bMorphable)
			{
				std::vector<g3dFragment*> fragments;
				m_pGroupBuffer.reset(m_Create.CreateFragmentGroup(i_Info, fragments, i_bMorphable));
				for (g3dFragment* f : fragments)
				{
					m_Group.emplace_back(f);
				}
			}

			std::vector<g3dFragment*> group() const
			{
				std::vector<g3dFragment*> fragments;
				for (const auto& f : m_Group)
				{
					fragments.push_back(f.get());
				}
				return fragments;
			}

			const g3dGasCacheOptiX::Stats& stats() { return Cache().GetStats(); }

			gvk::mayFragCreate							m_Create;
			optxRayQuery								m_Query;
			std::unique_ptr<g3dSceneBVHOptiX>			m_pBvh;
			std::vector<std::unique_ptr<g3dFragment>>	m_Group;
			std::unique_ptr<g3dVertexBuffer>			m_pGroupBuffer;
	};

	//	A unit quad in the plane z: triangle 0 below the diagonal y = x
	//	(material 0), triangle 1 above it (material 1)
	mdlFragInfo quad_group(float z)
	{
		mdlFragInfo info;
		info.m_Vertices = { maPoint3d(-1, -1, z), maPoint3d(1, -1, z), maPoint3d(1, 1, z), maPoint3d(-1, 1, z) };
		info.m_Normals.assign(4, maVector3d(0, 0, 1));
		info.m_UVs = { maPoint2d(0, 0), maPoint2d(1, 0), maPoint2d(1, 1), maPoint2d(0, 1) };
		info.m_Indices = { 0, 1, 2, 0, 2, 3 };
		info.m_Materials = { std::make_shared<mdlMatInfo>(), std::make_shared<mdlMatInfo>() };
		info.m_MaterialChanges = { 1 };
		info.m_Flags.m_bCastsShadow = true;
		info.m_Flags.m_bReceivesShadow = true;
		info.m_Flags.m_bShadowHull = false;
		info.m_Flags.m_bDoubleSided = true;
		return info;
	}

	//	A mesh of one primitive per material in i_Materials, primitive p the
	//	triangle i_Triangles[p] (three positions, in metres), not indexed
	int add_mesh(tinygltf::Model& io_Model, const std::vector<std::vector<float>>& i_Triangles,
				 const std::vector<int>& i_Materials)
	{
		tinygltf::Mesh mesh;
		for (size_t p = 0; p < i_Triangles.size(); ++p)
		{
			tinygltf::Primitive primitive;
			primitive.attributes["POSITION"] = ietAddFloatAccessor(io_Model, i_Triangles[p], TINYGLTF_TYPE_VEC3);
			primitive.material = i_Materials[p];
			primitive.mode = TINYGLTF_MODE_TRIANGLES;
			mesh.primitives.push_back(primitive);
		}
		io_Model.meshes.push_back(mesh);
		return (int)io_Model.meshes.size() - 1;
	}

	//	Total transforms for a whole tree, as the renderers update them
	void update_total_transforms(g3dSceneNode* io_pNode)
	{
		io_pNode->UpdateTotalTransform();
		for (g3dSceneNode* child : io_pNode->GetChildren())
		{
			update_total_transforms(child);
		}
	}
}

TEST_F(g3dSceneBVHOptiXGroupTest, GroupIsOneGasAndOneInstance)
{
	make_group(quad_group(0), false);
	ASSERT_EQ(2u, m_Group.size());
	gtOx::Scene scene;
	scene.Add(m_Group[0].get());
	scene.Add(m_Group[1].get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(1u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(1u, m_pBvh->GetGasCount());
	EXPECT_EQ(2u, m_pBvh->GetSbtRecordCount());
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(0, stats().m_Uploads);

	//	Below the diagonal: material 0's triangle; above: material 1's
	const Hit below = trace_down(0.5f, -0.5f);
	EXPECT_FLOAT_EQ(5.0f, below.m_T);
	EXPECT_EQ(0u, below.m_Instance);
	EXPECT_EQ(0u, below.m_Input);
	const Hit above = trace_down(-0.5f, 0.5f);
	EXPECT_FLOAT_EQ(5.0f, above.m_T);
	EXPECT_EQ(0u, above.m_Instance);
	EXPECT_EQ(1u, above.m_Input);
	EXPECT_EQ(0u, above.m_Triangle);	// counted within its input

	std::vector<optxInstanceData> table(1);
	ASSERT_TRUE(optxContext::Synchronize());
	ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(table.data(), m_pBvh->GetInstanceTable(), sizeof(optxInstanceData)));
	EXPECT_EQ(2u, table[0].m_NumInputs);
	EXPECT_EQ(2u, table[0].m_NumTriangles);
}

TEST_F(g3dSceneBVHOptiXGroupTest, HidingOneMaterialFallsBack)
{
	make_group(quad_group(0), false);
	gtOx::Scene scene;
	scene.Add(m_Group[0].get());
	g3dSceneNode* second = scene.Add(m_Group[1].get());
	second->SetRenderable(false);
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(1u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(1u, m_pBvh->GetSbtRecordCount());
	EXPECT_FLOAT_EQ(5.0f, trace_down(0.5f, -0.5f).m_T);
	EXPECT_EQ(-1.0f, trace_down(-0.5f, 0.5f).m_T);

	//	Shown again: pooled again
	second->SetRenderable(true);
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(1u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(2u, m_pBvh->GetSbtRecordCount());

	//	Under different transforms: a GAS each
	second->SetTransform(gtOx::ScaleTranslate(1, 1, 1, 0, 0, -1));
	second->UpdateTotalTransform();
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(2u, m_pBvh->GetGasCount());
	EXPECT_FLOAT_EQ(6.0f, trace_down(-0.5f, 0.5f).m_T);
}

TEST_F(g3dSceneBVHOptiXGroupTest, PoolingNoneAndGroupHitTheSame)
{
	make_group(quad_group(0), false);
	auto other = gtOx::Triangle(-2);
	gtOx::Scene scene;
	scene.Add(m_Group[0].get());
	scene.Add(m_Group[1].get());
	scene.Add(other.get(), gtOx::ScaleTranslate(3, 3, 1, 0, 0, 0));

	//	Rays over a grid: the distance and which of the three fragments was
	//	hit (group: instance 0's input i is fragment i, instance 1 fragment 2;
	//	none: instance i is fragment i)
	auto sample = [&](bool i_bGrouped)
	{
		std::vector<std::pair<float, int>> result;
		for (float y = -2.5f; y <= 2.5f; y += 0.25f)
		{
			for (float x = -2.5f; x <= 2.5f; x += 0.25f)
			{
				const Hit hit = trace_down(x + 0.01f, y + 0.02f);
				int fragment = -1;
				if (hit.m_T >= 0)
				{
					fragment = i_bGrouped ? (hit.m_Instance == 0 ? int(hit.m_Input) : 2) : int(hit.m_Instance);
				}
				result.push_back({ hit.m_T, fragment });
			}
		}
		return result;
	};

	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(2u, m_pBvh->GetGasCount());
	const auto grouped = sample(true);

	m_pBvh->SetPooling(g3dSceneBVHOptiX::e_PoolingNone);
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(3u, m_pBvh->GetInstanceCount());
	EXPECT_EQ(3u, m_pBvh->GetGasCount());
	EXPECT_EQ(1u, m_pBvh->GetSbtRecordCount());
	EXPECT_EQ(grouped, sample(false));
}

TEST_F(g3dSceneBVHOptiXGroupTest, GroupLockIsOneRefit)
{
	make_group(quad_group(0), true);
	gtOx::Scene scene;
	scene.Add(m_Group[0].get());
	scene.Add(m_Group[1].get());
	ASSERT_TRUE(update(scene));
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(1, stats().m_Uploads);	// the shared vertices, once

	//	As smdlSubdivSurface animates a group
	g3dType::BumpTex1Vertex* v = reinterpret_cast<g3dType::BumpTex1Vertex*>(m_pGroupBuffer->Lock());
	ASSERT_NE(nullptr, v);
	for (int i = 0; i < 4; ++i)
	{
		v[i].m_Vertex.SetZ(-1.0f);
	}
	m_pGroupBuffer->Unlock();

	ASSERT_TRUE(update(scene));
	EXPECT_EQ(1, stats().m_Builds);
	EXPECT_EQ(1, stats().m_Refits);
	EXPECT_EQ(2, stats().m_Uploads);
	EXPECT_FLOAT_EQ(6.0f, trace_down(0.5f, -0.5f).m_T);
	EXPECT_FLOAT_EQ(6.0f, trace_down(-0.5f, 0.5f).m_T);
}

TEST_F(g3dSceneBVHOptiXGroupTest, UpdateFragmentGroupRebuilds)
{
	make_group(quad_group(0), true);
	gtOx::Scene scene;
	scene.Add(m_Group[0].get());
	scene.Add(m_Group[1].get());
	ASSERT_TRUE(update(scene));

	//	A finer level: the quad split into four triangles around a centre
	//	vertex, two per material, lower
	mdlFragInfo finer = quad_group(-2);
	finer.m_Vertices.push_back(maPoint3d(0, 0, -2));
	finer.m_Normals.push_back(maVector3d(0, 0, 1));
	finer.m_UVs.push_back(maPoint2d(0.5f, 0.5f));
	finer.m_Indices = { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 };
	finer.m_MaterialChanges = { 2 };
	std::vector<g3dFragment*> fragments = group();
	m_Create.UpdateFragmentGroup(finer, *m_pGroupBuffer, fragments, true);

	ASSERT_TRUE(update(scene));
	EXPECT_EQ(2, stats().m_Builds);
	EXPECT_EQ(0, stats().m_Refits);
	EXPECT_EQ(1u, m_pBvh->GetInstanceCount());

	//	Input 0 has the bottom and right triangles (0, 1), input 1 the top and left
	Hit hit = trace_down(0, -0.5f);
	EXPECT_FLOAT_EQ(7.0f, hit.m_T);
	EXPECT_EQ(0u, hit.m_Input);
	EXPECT_EQ(0u, hit.m_Triangle);
	hit = trace_down(0.5f, 0);
	EXPECT_EQ(0u, hit.m_Input);
	EXPECT_EQ(1u, hit.m_Triangle);
	hit = trace_down(0, 0.5f);
	EXPECT_EQ(1u, hit.m_Input);
	EXPECT_EQ(0u, hit.m_Triangle);
	hit = trace_down(-0.5f, 0);
	EXPECT_EQ(1u, hit.m_Input);
	EXPECT_EQ(1u, hit.m_Triangle);
}

TEST_F(g3dSceneBVHOptiXGroupTest, ImportedGltfModel)
{
	//	Two nodes: "Quad" at x = 2 m, a mesh of two triangles (the unit quad
	//	in z = 0, split along y = x) with a material each; "Tri" at z = -1 m,
	//	scaled 2 in x, one triangle (-1,-1) (1,-1) (0,1). The importer works
	//	in centimetres (gltfMeshImport::c_UnitScale)
	tinygltf::Model model;
	model.materials.resize(2);
	model.materials[0].name = "a";
	model.materials[1].name = "b";
	const int quad = add_mesh(model, { { -1, -1, 0, 1, -1, 0, 1, 1, 0 }, { -1, -1, 0, 1, 1, 0, -1, 1, 0 } }, { 0, 1 });
	const int tri = add_mesh(model, { { -1, -1, 0, 1, -1, 0, 0, 1, 0 } }, { 0 });
	model.nodes.resize(2);
	model.nodes[0].name = "Quad";
	model.nodes[0].mesh = quad;
	model.nodes[0].translation = { 2, 0, 0 };
	model.nodes[1].name = "Tri";
	model.nodes[1].mesh = tri;
	model.nodes[1].translation = { 0, 0, -1 };
	model.nodes[1].scale = { 2, 1, 1 };
	model.scenes.resize(1);
	model.scenes[0].nodes = { 0, 1 };
	model.defaultScene = 0;
	model.asset.version = "2.0";

	gtTempDir tmp;
	const std::filesystem::path file = tmp.GetPath() / "model.gltf";
	tinygltf::TinyGLTF writer;
	ASSERT_TRUE(writer.WriteGltfSceneToFile(&model, file.string(), false, true, true, false));

	//	As GraphicsVulkanLayer installs it
	mdlFragCreateImpl* previous = mdlFragCreate::SetImplementation(&m_Create);
	gltfImport import;
	std::unique_ptr<entModelTemplate> base(import.LoadGeometry(tmp.GetLocator("model.gltf")));
	mdlFragCreate::SetImplementation(previous);
	emdlCharacterTemplate* loaded = dynamic_cast<emdlCharacterTemplate*>(base.get());
	ASSERT_NE(nullptr, loaded);
	ASSERT_NE(nullptr, loaded->GetModel());

	//	One fragment per material of each mesh, all Vulkan's
	ASSERT_EQ(3u, loaded->GetFragments().size());
	for (g3dFragment* fragment : loaded->GetFragments())
	{
		gvk::tmeshFrag* vulkan = dynamic_cast<gvk::tmeshFrag*>(fragment);
		ASSERT_NE(nullptr, vulkan);
		EXPECT_EQ(1, vulkan->GetNumIndices() / 3);
	}

	{
		g3dSceneNode* root = loaded->GetModel();
		update_total_transforms(root);
		g3dScene scene(new g3dLayer(root));		// the template keeps the nodes
		ASSERT_TRUE(m_pBvh->Update(scene, std::vector<g3dLayer*>(), false));
		ASSERT_NE(0u, m_pBvh->GetHandle());
		//	The quad's two materials pooled (if they share vertices), the triangle alone
		EXPECT_GE(3u, m_pBvh->GetInstanceCount());
		EXPECT_LE(2u, m_pBvh->GetInstanceCount());
		EXPECT_EQ(3u, m_pBvh->GetTriangleCount());

		//	Quad: x 100..300, y -100..100, z 0; Tri: x -200..200, y -100..100, z -100
		const maAxisBox& bounds = m_pBvh->GetBounds();
		EXPECT_NEAR(-200.0f, bounds.GetMinX(), 1e-3f);
		EXPECT_NEAR(300.0f, bounds.GetMaxX(), 1e-3f);
		EXPECT_NEAR(-100.0f, bounds.GetMinY(), 1e-3f);
		EXPECT_NEAR(100.0f, bounds.GetMaxY(), 1e-3f);
		EXPECT_NEAR(-100.0f, bounds.GetMinZ(), 1e-3f);
		EXPECT_NEAR(0.0f, bounds.GetMaxZ(), 1e-3f);

		//	Rays down from z = 500: both halves of the quad, the scaled
		//	triangle where only its scaled copy reaches, and a miss
		EXPECT_NEAR(500.0f, trace_down(250, -50, 500).m_T, 1e-2f);
		EXPECT_NEAR(500.0f, trace_down(150, 50, 500).m_T, 1e-2f);
		EXPECT_NEAR(600.0f, trace_down(-150, -90, 500).m_T, 1e-2f);
		EXPECT_EQ(-1.0f, trace_down(250, 150, 500).m_T);

		EXPECT_FALSE(m_pBvh->Update(scene, std::vector<g3dLayer*>(), false));
	}
}
