/*****************************************************************************
**  mayFragCreateOptiXTest.cpp
**
**      mayFragCreateOptiX, which makes imported models' fragments: one
**	from an mdlSplitFragInfo (attributes, flags, weld indices dropped, a
**	GAS rays hit); a group
**	(two materials on one shared store, uploaded once, each fragment with
**	its own triangles); the group's vertex buffer written through
**	Lock/Unlock (every fragment refit); and UpdateFragmentGroup (new
**	vertices and indices: both generations change, every GAS rebuilt);
**	and a glTF file loaded through gltfImport with mayFragCreateOptiX
**	installed (the fragment count, the scene BVH's bounds, rays).
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#include "GraphicsOptiX/G3d/g3dVertexBufferOptiX.hpp"
#include "GraphicsOptiX/may/private/mayFragCreateOptiX.hpp"
#include "GraphicsOptiX/optx/optxRayQuery.hpp"
#ifndef GLTF_IMPORT_HPP
#include "ImportExport/gltf/import/gltfImport.hpp"
#endif
#ifndef EMDL_CHARACTERTEMPLATE_HPP
#include "Graphics/emdl/emdlCharacterTemplate.hpp"
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
#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
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
	class mayFragCreateOptiXTest : public gtOptiX
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

			//	A ray down -z from (x, y, i_Z): {hit distance or -1, triangle}
			std::pair<float, unsigned> trace_down(OptixTraversableHandle i_Handle, float x, float y, float i_Z = 5.0f)
			{
				optxRayQueryRay ray = {};
				ray.m_Origin[0] = x;
				ray.m_Origin[1] = y;
				ray.m_Origin[2] = i_Z;
				ray.m_Direction[2] = -1.0f;
				ray.m_TMax = 1e30f;
				ray.m_Mask = 0xFF;
				std::vector<optxRayQueryHit> hits;
				EXPECT_TRUE(m_Query.Trace(i_Handle, { ray }, hits));
				if (hits.size() != 1 || !hits[0].m_Hit)
				{
					return { -1.0f, ~0u };
				}
				return { hits[0].m_T, hits[0].m_PrimitiveIndex };
			}

			mayFragCreateOptiX	m_Create;
			optxRayQuery		m_Query;
	};

	//	A unit quad in the plane z: triangle 0 below the diagonal y = x,
	//	triangle 1 above it, one material each
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

	struct Group
	{
		std::unique_ptr<g3dVertexBuffer>				m_pBuffer;
		std::vector<std::unique_ptr<g3dFragmentOptiX>>	m_Fragments;

		g3dVertexBufferOptiX* GetBuffer() const { return dynamic_cast<g3dVertexBufferOptiX*>(m_pBuffer.get()); }

		std::vector<g3dFragment*> raw() const
		{
			std::vector<g3dFragment*> fragments;
			for (const auto& f : m_Fragments)
			{
				fragments.push_back(f.get());
			}
			return fragments;
		}
	};

	Group make_group(mdlFragCreateImpl& io_Create, const mdlFragInfo& i_Info, bool i_bMorphable)
	{
		Group group;
		std::vector<g3dFragment*> fragments;
		group.m_pBuffer.reset(io_Create.CreateFragmentGroup(i_Info, fragments, i_bMorphable));
		for (g3dFragment* f : fragments)
		{
			group.m_Fragments.emplace_back(dynamic_cast<g3dFragmentOptiX*>(f));
		}
		return group;
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

TEST_F(mayFragCreateOptiXTest, CreateFragment)
{
	mdlSplitFragInfo info;
	info.m_Vertices = { maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0) };
	info.m_Normals.assign(3, maVector3d(0, 0, 1));
	info.m_UVs = { maPoint2d(0, 0), maPoint2d(1, 0), maPoint2d(0.5f, 1) };
	info.m_Indices = { 0, 1, 2, 0, 0, 1 };	// a shadow-volume weld triangle after the first
	info.m_WeldIndex = 3;
	info.m_Material = nullptr;
	info.m_Flags.m_bCastsShadow = false;
	info.m_Flags.m_bReceivesShadow = true;
	info.m_Flags.m_bShadowHull = true;
	info.m_Flags.m_bDoubleSided = true;

	std::unique_ptr<g3dFragment> base(m_Create.CreateFragment(info, false));
	g3dFragmentOptiX* frag = dynamic_cast<g3dFragmentOptiX*>(base.get());
	ASSERT_NE(nullptr, frag);
	EXPECT_EQ(3, frag->GetNumVertices());
	EXPECT_EQ(3, frag->GetNumIndices());	// the weld indices are dropped
	EXPECT_FALSE(frag->GetMorphable());
	EXPECT_FALSE(frag->GetCastsShadow());
	EXPECT_TRUE(frag->GetReceivesShadow());
	EXPECT_TRUE(frag->IsShadowHull());
	EXPECT_TRUE(frag->GetDoubleSided());

	const g3dType::BumpTex1Vertex* v = reinterpret_cast<const g3dType::BumpTex1Vertex*>(frag->ReadOnlyLock());
	EXPECT_FLOAT_EQ(1.0f, v[2].m_Normal.GetZ());
	EXPECT_FLOAT_EQ(0.5f, v[2].m_TexCoord.GetX());
	frag->ReadOnlyUnlock();

	EXPECT_FLOAT_EQ(5.0f, trace_down(frag->EnsureGAS(), 0, 0).first);

	//	No vertices: no fragment
	EXPECT_EQ(nullptr, m_Create.CreateFragment(mdlSplitFragInfo(), false));
}

TEST_F(mayFragCreateOptiXTest, GroupSharesOneStore)
{
	Group group = make_group(m_Create, quad_group(0), false);
	ASSERT_NE(nullptr, group.GetBuffer());
	ASSERT_EQ(2u, group.m_Fragments.size());
	g3dFragmentOptiX& a = *group.m_Fragments[0];
	g3dFragmentOptiX& b = *group.m_Fragments[1];
	EXPECT_EQ(group.GetBuffer()->GetStore(), a.GetStore());
	EXPECT_EQ(a.GetStore(), b.GetStore());
	EXPECT_EQ(4, group.m_pBuffer->GetNumVertices());
	EXPECT_EQ(3, a.GetNumIndices());
	EXPECT_EQ(3, b.GetNumIndices());
	EXPECT_TRUE(a.GetCastsShadow());
	EXPECT_TRUE(b.GetDoubleSided());

	const OptixTraversableHandle ha = a.EnsureGAS();
	const OptixTraversableHandle hb = b.EnsureGAS();
	ASSERT_NE(0u, ha);
	ASSERT_NE(0u, hb);
	EXPECT_EQ(1, a.GetStore()->GetUploadCount());

	//	Each fragment holds only its own material's triangle
	EXPECT_FLOAT_EQ(5.0f, trace_down(ha, 0.5f, -0.5f).first);
	EXPECT_EQ(-1.0f, trace_down(ha, -0.5f, 0.5f).first);
	EXPECT_FLOAT_EQ(5.0f, trace_down(hb, -0.5f, 0.5f).first);
	EXPECT_EQ(-1.0f, trace_down(hb, 0.5f, -0.5f).first);
}

TEST_F(mayFragCreateOptiXTest, GroupVertexBufferLockRefitsEveryFragment)
{
	Group group = make_group(m_Create, quad_group(0), true);
	g3dFragmentOptiX& a = *group.m_Fragments[0];
	g3dFragmentOptiX& b = *group.m_Fragments[1];
	EXPECT_TRUE(a.GetMorphable());
	ASSERT_NE(0u, a.EnsureGAS());
	ASSERT_NE(0u, b.EnsureGAS());

	//	As smdlSubdivSurface animates a group
	g3dType::BumpTex1Vertex* v = reinterpret_cast<g3dType::BumpTex1Vertex*>(group.m_pBuffer->Lock());
	ASSERT_NE(nullptr, v);
	for (int i = 0; i < 4; ++i)
	{
		v[i].m_Vertex.SetZ(-1.0f);
	}
	group.m_pBuffer->Unlock();

	const OptixTraversableHandle ha = a.EnsureGAS();
	const OptixTraversableHandle hb = b.EnsureGAS();
	EXPECT_EQ(1, a.GetGasRefitCount());
	EXPECT_EQ(1, b.GetGasRefitCount());
	EXPECT_EQ(1, a.GetGasBuildCount());
	EXPECT_EQ(2, a.GetStore()->GetUploadCount());	// once for both
	EXPECT_FLOAT_EQ(6.0f, trace_down(ha, 0.5f, -0.5f).first);
	EXPECT_FLOAT_EQ(6.0f, trace_down(hb, -0.5f, 0.5f).first);
}

TEST_F(mayFragCreateOptiXTest, UpdateFragmentGroup)
{
	Group group = make_group(m_Create, quad_group(0), true);
	g3dFragmentOptiX& a = *group.m_Fragments[0];
	g3dFragmentOptiX& b = *group.m_Fragments[1];
	ASSERT_NE(0u, a.EnsureGAS());
	ASSERT_NE(0u, b.EnsureGAS());
	const uint64_t vertex_generation = a.GetVertexGeneration();
	const uint64_t index_generation_a = a.GetIndexGeneration();
	const uint64_t index_generation_b = b.GetIndexGeneration();

	//	A finer level: the quad split into four triangles around a centre
	//	vertex, two per material, lower
	mdlFragInfo finer = quad_group(-2);
	finer.m_Vertices.push_back(maPoint3d(0, 0, -2));
	finer.m_Normals.push_back(maVector3d(0, 0, 1));
	finer.m_UVs.push_back(maPoint2d(0.5f, 0.5f));
	finer.m_Indices = { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 };
	finer.m_MaterialChanges = { 2 };
	std::vector<g3dFragment*> fragments = group.raw();
	m_Create.UpdateFragmentGroup(finer, *group.m_pBuffer, fragments, true);

	EXPECT_EQ(5, group.m_pBuffer->GetNumVertices());
	EXPECT_EQ(6, a.GetNumIndices());
	EXPECT_EQ(6, b.GetNumIndices());
	EXPECT_NE(vertex_generation, a.GetVertexGeneration());
	EXPECT_EQ(a.GetVertexGeneration(), b.GetVertexGeneration());
	EXPECT_NE(index_generation_a, a.GetIndexGeneration());
	EXPECT_NE(index_generation_b, b.GetIndexGeneration());

	const OptixTraversableHandle ha = a.EnsureGAS();
	const OptixTraversableHandle hb = b.EnsureGAS();
	EXPECT_EQ(2, a.GetGasBuildCount());
	EXPECT_EQ(2, b.GetGasBuildCount());
	EXPECT_EQ(0, a.GetGasRefitCount());
	EXPECT_EQ(2, a.GetStore()->GetUploadCount());

	//	a has the bottom and right triangles (0, 1), b the top and left
	EXPECT_EQ(std::make_pair(7.0f, 0u), trace_down(ha, 0, -0.5f));
	EXPECT_EQ(std::make_pair(7.0f, 1u), trace_down(ha, 0.5f, 0));
	EXPECT_EQ(std::make_pair(7.0f, 0u), trace_down(hb, 0, 0.5f));
	EXPECT_EQ(std::make_pair(7.0f, 1u), trace_down(hb, -0.5f, 0));
	EXPECT_EQ(-1.0f, trace_down(ha, 0, 0.5f).first);
}

TEST_F(mayFragCreateOptiXTest, ImportedGltfModel)
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

	//	As GraphicsOptiXLayer installs it
	mdlFragCreateImpl* previous = mdlFragCreate::SetImplementation(&m_Create);
	gltfImport import;
	std::unique_ptr<entModelTemplate> base(import.LoadGeometry(tmp.GetLocator("model.gltf")));
	mdlFragCreate::SetImplementation(previous);
	emdlCharacterTemplate* loaded = dynamic_cast<emdlCharacterTemplate*>(base.get());
	ASSERT_NE(nullptr, loaded);
	ASSERT_NE(nullptr, loaded->GetModel());

	//	One fragment per material of each mesh, all ours
	ASSERT_EQ(3u, loaded->GetFragments().size());
	for (g3dFragment* fragment : loaded->GetFragments())
	{
		g3dFragmentOptiX* ours = dynamic_cast<g3dFragmentOptiX*>(fragment);
		ASSERT_NE(nullptr, ours);
		EXPECT_EQ(1, ours->GetNumIndices() / 3);
	}

	{
		g3dSceneNode* root = loaded->GetModel();
		update_total_transforms(root);
		g3dScene scene(new g3dLayer(root));		// the template keeps the nodes
		g3dSceneBVHOptiX bvh;
		ASSERT_TRUE(bvh.Update(scene, std::vector<g3dLayer*>(), false));
		ASSERT_NE(0u, bvh.GetHandle());
		EXPECT_EQ(3u, bvh.GetInstanceCount());

		//	Quad: x 100..300, y -100..100, z 0; Tri: x -200..200, y -100..100, z -100
		const maAxisBox& bounds = bvh.GetBounds();
		EXPECT_NEAR(-200.0f, bounds.GetMinX(), 1e-3f);
		EXPECT_NEAR(300.0f, bounds.GetMaxX(), 1e-3f);
		EXPECT_NEAR(-100.0f, bounds.GetMinY(), 1e-3f);
		EXPECT_NEAR(100.0f, bounds.GetMaxY(), 1e-3f);
		EXPECT_NEAR(-100.0f, bounds.GetMinZ(), 1e-3f);
		EXPECT_NEAR(0.0f, bounds.GetMaxZ(), 1e-3f);

		//	Rays down from z = 500: both halves of the quad, the scaled
		//	triangle where only its scaled copy reaches, and a miss
		const OptixTraversableHandle handle = bvh.GetHandle();
		EXPECT_NEAR(500.0f, trace_down(handle, 250, -50, 500).first, 1e-2f);
		EXPECT_NEAR(500.0f, trace_down(handle, 150, 50, 500).first, 1e-2f);
		EXPECT_NEAR(600.0f, trace_down(handle, -150, -90, 500).first, 1e-2f);
		EXPECT_EQ(-1.0f, trace_down(handle, 250, 150, 500).first);

		EXPECT_FALSE(bvh.Update(scene, std::vector<g3dLayer*>(), false));
	}
}
