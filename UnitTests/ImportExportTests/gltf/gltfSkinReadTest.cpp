/*****************************************************************************
**  gltfSkinReadTest.cpp
**
**      Tests of gltfSkinRead: per-vertex bone influences, morph target
**      position deltas and default morph weights.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SKINREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkinRead.hpp"
#endif

#include "ietGltfModel.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace
{
	// a primitive with 3 positions
	tinygltf::Primitive Triangle(tinygltf::Model& io_Model)
	{
		tinygltf::Primitive prim;
		prim.attributes["POSITION"] = ietAddFloatAccessor(io_Model, { 0, 0, 0,  1, 0, 0,  0, 1, 0 }, TINYGLTF_TYPE_VEC3);
		return prim;
	}

	const std::vector<int> c_BoneOfJoint = { 5, 6, 7, 8, 9, 10, 11, 12 };
}

TEST(gltfSkinRead, Influences)
{
	tinygltf::Model model;
	tinygltf::Primitive prim = Triangle(model);
	prim.attributes["JOINTS_0"] = ietAddUByteAccessor(model, { 0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3 }, TINYGLTF_TYPE_VEC4);
	prim.attributes["WEIGHTS_0"] = ietAddFloatAccessor(model, { 0.5f, 0.5f, 0, 0,  0.125f, 0.125f, 0.125f, 0.125f,  0.2f, 0.2f, 0, 0 },
													   TINYGLTF_TYPE_VEC4);
	prim.attributes["JOINTS_1"] = ietAddUByteAccessor(model, { 4, 5, 6, 7,  4, 5, 6, 7,  4, 5, 6, 7 }, TINYGLTF_TYPE_VEC4);
	prim.attributes["WEIGHTS_1"] = ietAddFloatAccessor(model, { 0, 0, 0, 0,  0.125f, 0.125f, 0.125f, 0.125f,  0, 0, 0, 0 },
													   TINYGLTF_TYPE_VEC4);

	std::vector<std::vector<gltfSkinRead::gltfInfluence>> vertices;
	bool bRenormalized = false;
	ASSERT_TRUE(gltfSkinRead::ReadInfluences(model, prim, c_BoneOfJoint, vertices, bRenormalized));
	ASSERT_EQ(3u, vertices.size());

	ASSERT_EQ(2u, vertices[0].size());
	EXPECT_EQ(5u, vertices[0][0].m_Bone);
	EXPECT_EQ(6u, vertices[0][1].m_Bone);
	EXPECT_FLOAT_EQ(0.5f, vertices[0][0].m_Weight);

	ASSERT_EQ(8u, vertices[1].size());
	for (size_t i = 0; i < 8; i++)
	{
		EXPECT_EQ(c_BoneOfJoint[i], (int)vertices[1][i].m_Bone);
		EXPECT_FLOAT_EQ(0.125f, vertices[1][i].m_Weight);
	}

	ASSERT_EQ(2u, vertices[2].size());
	EXPECT_FLOAT_EQ(0.5f, vertices[2][0].m_Weight);
	EXPECT_FLOAT_EQ(0.5f, vertices[2][1].m_Weight);
	EXPECT_TRUE(bRenormalized);
}

TEST(gltfSkinRead, JointOutOfRange)
{
	tinygltf::Model model;
	tinygltf::Primitive prim = Triangle(model);
	prim.attributes["JOINTS_0"] = ietAddUByteAccessor(model, { 9, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 }, TINYGLTF_TYPE_VEC4);
	prim.attributes["WEIGHTS_0"] = ietAddFloatAccessor(model, { 0.5f, 0.5f, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0 }, TINYGLTF_TYPE_VEC4);

	std::vector<std::vector<gltfSkinRead::gltfInfluence>> vertices;
	bool bRenormalized = false;
	ASSERT_TRUE(gltfSkinRead::ReadInfluences(model, prim, c_BoneOfJoint, vertices, bRenormalized));
	ASSERT_EQ(1u, vertices[0].size());
	EXPECT_EQ(6u, vertices[0][0].m_Bone);
	EXPECT_FLOAT_EQ(1, vertices[0][0].m_Weight);
	EXPECT_TRUE(bRenormalized);
}

TEST(gltfSkinRead, NoJoints)
{
	tinygltf::Model model;
	tinygltf::Primitive prim = Triangle(model);
	std::vector<std::vector<gltfSkinRead::gltfInfluence>> vertices;
	bool bRenormalized = false;
	EXPECT_FALSE(gltfSkinRead::ReadInfluences(model, prim, c_BoneOfJoint, vertices, bRenormalized));

	// counts that don't match the positions
	prim.attributes["JOINTS_0"] = ietAddUByteAccessor(model, { 0, 0, 0, 0 }, TINYGLTF_TYPE_VEC4);
	prim.attributes["WEIGHTS_0"] = ietAddFloatAccessor(model, { 1, 0, 0, 0 }, TINYGLTF_TYPE_VEC4);
	EXPECT_FALSE(gltfSkinRead::ReadInfluences(model, prim, c_BoneOfJoint, vertices, bRenormalized));
}

TEST(gltfSkinRead, MorphDeltas)
{
	tinygltf::Model model;
	tinygltf::Primitive prim = Triangle(model);
	prim.targets.resize(2);
	prim.targets[0]["POSITION"] = ietAddFloatAccessor(model, { 0.01f, 0, 0,  0, 0.02f, 0,  0, 0, 0.03f }, TINYGLTF_TYPE_VEC3);
	prim.targets[1]["NORMAL"] = ietAddFloatAccessor(model, { 0, 0, 1,  0, 0, 1,  0, 0, 1 }, TINYGLTF_TYPE_VEC3);

	std::vector<std::vector<maVector3d>> targets;
	ASSERT_TRUE(gltfSkinRead::ReadMorphDeltas(model, prim, 3, targets));
	ASSERT_EQ(2u, targets.size());
	ASSERT_EQ(3u, targets[0].size());
	EXPECT_NEAR(1, targets[0][0].m_X, 1e-5f);
	EXPECT_NEAR(2, targets[0][1].m_Y, 1e-5f);
	EXPECT_NEAR(3, targets[0][2].m_Z, 1e-5f);
	ASSERT_EQ(3u, targets[1].size());
	for (const maVector3d& v : targets[1])
		EXPECT_FLOAT_EQ(0, v.Length());

	EXPECT_FALSE(gltfSkinRead::ReadMorphDeltas(model, prim, 4, targets));
}

TEST(gltfSkinRead, DefaultWeights)
{
	tinygltf::Mesh mesh;
	mesh.primitives.resize(1);
	mesh.primitives[0].targets.resize(2);
	tinygltf::Node node;
	EXPECT_EQ((std::vector<float>{ 0, 0 }), gltfSkinRead::DefaultWeights(mesh, node));

	mesh.weights = { 0.25, 0.5 };
	EXPECT_EQ((std::vector<float>{ 0.25f, 0.5f }), gltfSkinRead::DefaultWeights(mesh, node));

	node.weights = { 1, 0 };
	EXPECT_EQ((std::vector<float>{ 1, 0 }), gltfSkinRead::DefaultWeights(mesh, node));

	EXPECT_EQ("#1", gltfSkinRead::MorphTargetKey(1));
}
