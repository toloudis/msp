/*****************************************************************************
**  gltfSkeletonReadTest.cpp
**
**      Tests of gltfSkeletonRead: the node tree a glTF skin needs (its
**      root, the root's parent, bone order and inverse bind matrices).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SKELETONREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
#endif

#include "ietGltfModel.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{
	int AddNode(tinygltf::Model& io_Model, const std::string& i_Name, std::vector<int> i_Children = {})
	{
		tinygltf::Node node;
		node.name = i_Name;
		node.children = i_Children;
		io_Model.nodes.push_back(node);
		return (int)io_Model.nodes.size() - 1;
	}

	// Armature -> Hips(joint) -> { Spine(joint) -> Head(joint), LegL(joint) }; skin 0 lists { Head, Hips, LegL, Spine }
	struct Rig
	{
		tinygltf::Model m_Model;
		int m_Armature, m_Hips, m_Spine, m_Head, m_LegL;

		Rig()
		{
			m_Head = AddNode(m_Model, "Head");
			m_Spine = AddNode(m_Model, "Spine", { m_Head });
			m_LegL = AddNode(m_Model, "LegL");
			m_Hips = AddNode(m_Model, "Hips", { m_Spine, m_LegL });
			m_Armature = AddNode(m_Model, "Armature", { m_Hips });
			tinygltf::Scene scene;
			scene.nodes = { m_Armature };
			m_Model.scenes.push_back(scene);
			m_Model.skins.resize(1);
			m_Model.skins[0].joints = { m_Head, m_Hips, m_LegL, m_Spine };
		}
	};
}

TEST(gltfSkeletonRead, LowestCommonAncestor)
{
	Rig rig;
	gltfSkeleton skeleton;
	ASSERT_TRUE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));
	EXPECT_EQ(rig.m_Hips, skeleton.m_Root);
	EXPECT_EQ(rig.m_Armature, skeleton.m_Base);

	ASSERT_EQ(4u, skeleton.m_Nodes.size());
	const int nodes[] = { rig.m_Hips, rig.m_Spine, rig.m_Head, rig.m_LegL };
	const int parents[] = { -1, 0, 1, 0 };
	for (int i = 0; i < 4; i++)
	{
		EXPECT_EQ(nodes[i], skeleton.m_Nodes[i].m_Node) << i;
		EXPECT_EQ(parents[i], skeleton.m_Nodes[i].m_Parent) << i;
		EXPECT_TRUE(skeleton.m_Nodes[i].m_bJoint) << i;
	}
	EXPECT_EQ((std::vector<int>{ 2, 0, 3, 1 }), skeleton.m_BoneOfJoint);
}

TEST(gltfSkeletonRead, SkeletonPropertyUsed)
{
	Rig rig;
	rig.m_Model.skins[0].skeleton = rig.m_Armature;
	gltfSkeleton skeleton;
	ASSERT_TRUE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));
	EXPECT_EQ(rig.m_Armature, skeleton.m_Root);
	EXPECT_EQ(-1, skeleton.m_Base);
	ASSERT_EQ(5u, skeleton.m_Nodes.size());
	EXPECT_EQ(rig.m_Armature, skeleton.m_Nodes[0].m_Node);
	EXPECT_FALSE(skeleton.m_Nodes[0].m_bJoint);
	// bones count only the joints
	EXPECT_EQ((std::vector<int>{ 2, 0, 3, 1 }), skeleton.m_BoneOfJoint);

	// Spine isn't an ancestor of LegL: the joints' lowest common ancestor instead
	rig.m_Model.skins[0].skeleton = rig.m_Spine;
	ASSERT_TRUE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));
	EXPECT_EQ(rig.m_Hips, skeleton.m_Root);
	EXPECT_EQ(rig.m_Armature, skeleton.m_Base);
}

TEST(gltfSkeletonRead, InverseBindMatrices)
{
	Rig rig;
	std::vector<float> matrices;
	for (int j = 0; j < 4; j++)
	{
		// column-major: a rotation of 90 degrees about Z (x -> y), then a translation
		const float m[16] = { 0, 1, 0, 0,  -1, 0, 0, 0,  0, 0, 1, 0,  0.1f * (j + 1), 0.2f, 0.3f, 1 };
		matrices.insert(matrices.end(), m, m + 16);
	}
	rig.m_Model.skins[0].inverseBindMatrices = ietAddFloatAccessor(rig.m_Model, matrices, TINYGLTF_TYPE_MAT4);

	gltfSkeleton skeleton;
	ASSERT_TRUE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));
	// joint 0 (Head) is bone 2
	const maMatrix4x4& ibm = skeleton.m_Nodes[2].m_InvBindPose;
	EXPECT_NEAR(10, ibm(3, 0), 1e-4f);
	EXPECT_NEAR(20, ibm(3, 1), 1e-4f);
	EXPECT_NEAR(30, ibm(3, 2), 1e-4f);
	EXPECT_FLOAT_EQ(0, ibm(0, 0));
	EXPECT_FLOAT_EQ(1, ibm(0, 1));
	EXPECT_FLOAT_EQ(-1, ibm(1, 0));
	EXPECT_FLOAT_EQ(1, ibm(2, 2));
	EXPECT_FLOAT_EQ(1, ibm(3, 3));
	// joint 1 (Hips) is bone 0
	EXPECT_NEAR(20, skeleton.m_Nodes[0].m_InvBindPose(3, 0), 1e-4f);

	// no accessor: identity
	Rig plain;
	ASSERT_TRUE(gltfSkeletonRead::Read(plain.m_Model, 0, skeleton));
	EXPECT_TRUE(skeleton.m_Nodes[2].m_InvBindPose.IsIdentity());
}

TEST(gltfSkeletonRead, TranslationInCentimetres)
{
	Rig rig;
	rig.m_Model.nodes[rig.m_Hips].translation = { 0, 1, 0 };
	gltfSkeleton skeleton;
	ASSERT_TRUE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));
	EXPECT_NEAR(0, skeleton.m_Nodes[0].m_Translation.m_X, 1e-5f);
	EXPECT_NEAR(100, skeleton.m_Nodes[0].m_Translation.m_Y, 1e-5f);
	EXPECT_NEAR(0, skeleton.m_Nodes[0].m_Translation.m_Z, 1e-5f);
	EXPECT_FLOAT_EQ(1, skeleton.m_Nodes[0].m_Scale.m_X);
}

TEST(gltfSkeletonRead, NoCommonAncestor)
{
	Rig rig;
	const int other = AddNode(rig.m_Model, "Other");
	rig.m_Model.scenes[0].nodes.push_back(other);
	rig.m_Model.skins[0].joints.push_back(other);
	gltfSkeleton skeleton;
	EXPECT_FALSE(gltfSkeletonRead::Read(rig.m_Model, 0, skeleton));

	Rig empty;
	empty.m_Model.skins[0].joints.clear();
	EXPECT_FALSE(gltfSkeletonRead::Read(empty.m_Model, 0, skeleton));
}
