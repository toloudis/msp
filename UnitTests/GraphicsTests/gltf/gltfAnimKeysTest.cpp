/*****************************************************************************
**  gltfAnimKeysTest.cpp
**
**      gltfAnimKeys::Build: a glTF clip becomes character animation keys
**      whose key tree mirrors the skinned template (base node, then the
**      skeleton in gltfSkeleton order), with rest values filling the
**      channels a keyed node lacks, and morph weights as morph keys.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMKEYS_HPP
#include "ImportExport/gltf/import/private/gltfAnimKeys.hpp"
#endif
#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif
#ifndef GLTF_SCENEREAD_HPP
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#endif
#ifndef EMDL_CHARACTERANIMKEYS_HPP
#include "Graphics/emdl/emdlCharacterAnimKeys.hpp"
#endif
#ifndef SMDL_CHARACTERANIMATION_HPP
#include "Graphics/smdl/smdlCharacterAnimation.hpp"
#endif

// the glTF model builders the ImportExport tests use
#include "../ImportExportTests/ietGltfModel.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace
{
	const float c_Pi = 3.14159265358979f;
	const float c_Fps = 30;

	int AddNode(tinygltf::Model& io_Model, const std::string& i_Name, std::vector<int> i_Children = {})
	{
		tinygltf::Node node;
		node.name = i_Name;
		node.children = i_Children;
		io_Model.nodes.push_back(node);
		return (int)io_Model.nodes.size() - 1;
	}

	void AddAnimation(tinygltf::Model& io_Model, const std::string& i_Name, int i_Node, const std::string& i_Path,
					  const std::vector<float>& i_Times, const std::vector<float>& i_Values, int i_Type)
	{
		tinygltf::AnimationSampler sampler;
		sampler.input = ietAddFloatAccessor(io_Model, i_Times, TINYGLTF_TYPE_SCALAR);
		sampler.output = ietAddFloatAccessor(io_Model, i_Values, i_Type);
		tinygltf::AnimationChannel channel;
		channel.sampler = 0;
		channel.target_node = i_Node;
		channel.target_path = i_Path;
		tinygltf::Animation animation;
		animation.name = i_Name;
		animation.samplers.push_back(sampler);
		animation.channels.push_back(channel);
		io_Model.animations.push_back(animation);
	}

	// Armature -> Hips -> { Spine -> Head, LegL } (joints) with skinned Body; a morphing Box (2 targets).
	// "Bend" turns Spine 90 degrees about Y over 1 s; "Morph" sets Box's weights; "Spin" turns Box
	struct Rig
	{
		tinygltf::Model m_Model;
		int m_Hips, m_Spine, m_Head, m_LegL, m_Body, m_Box;
		std::vector<gltfSceneObject> m_Objects;

		Rig()
		{
			m_Model.meshes.resize(2);
			m_Model.meshes[1].primitives.resize(1);
			m_Model.meshes[1].primitives[0].targets.resize(2);
			m_Head = AddNode(m_Model, "Head");
			m_Spine = AddNode(m_Model, "Spine", { m_Head });
			m_Model.nodes[m_Spine].translation = { 0, 0.3, 0 };
			m_LegL = AddNode(m_Model, "LegL");
			m_Hips = AddNode(m_Model, "Hips", { m_Spine, m_LegL });
			m_Body = AddNode(m_Model, "Body");
			m_Model.nodes[m_Body].mesh = 0;
			m_Model.nodes[m_Body].skin = 0;
			const int armature = AddNode(m_Model, "Armature", { m_Hips, m_Body });
			m_Box = AddNode(m_Model, "Box");
			m_Model.nodes[m_Box].mesh = 1;
			m_Model.skins.resize(1);
			m_Model.skins[0].joints = { m_Hips, m_Spine, m_Head, m_LegL };
			tinygltf::Scene scene;
			scene.nodes = { armature, m_Box };
			m_Model.scenes.push_back(scene);

			const float s = sinf(c_Pi / 4), c = cosf(c_Pi / 4);
			AddAnimation(m_Model, "Bend", m_Spine, "rotation", { 0, 1 }, { 0, 0, 0, 1,  0, s, 0, c }, TINYGLTF_TYPE_VEC4);
			AddAnimation(m_Model, "Morph", m_Box, "weights", { 0, 1 }, { 0, 0,  1, 0.5f }, TINYGLTF_TYPE_SCALAR);
			AddAnimation(m_Model, "Spin", m_Box, "rotation", { 0, 1 }, { 0, 0, 0, 1,  0, 0, 0, 1 }, TINYGLTF_TYPE_VEC4);
			m_Objects = gltfSceneRead::ReadScene(m_Model);
		}

		const gltfSceneObject& Object(int i_Node) const
		{
			for (const gltfSceneObject& o : m_Objects)
			{
				if (o.m_Node == i_Node)
					return o;
			}
			ADD_FAILURE() << "no object for node " << i_Node;
			return m_Objects[0];
		}

		std::unique_ptr<emdlCharacterAnimKeys> Build(int i_Node, const std::string& i_Clip, gltfClip* o_pClip = NULL) const
		{
			gltfClip clip;
			EXPECT_TRUE(gltfAnimRead::ReadClip(m_Model, i_Clip, c_Fps, clip));
			if (o_pClip)
				*o_pClip = clip;
			return std::unique_ptr<emdlCharacterAnimKeys>(gltfAnimKeys::Build(m_Model, Object(i_Node), clip, c_Fps));
		}
	};

	// the key tree's iterator at the head ("" root)
	smdlTree<smdlGeoAnimKeys>::const_iterator Head(const emdlCharacterAnimKeys& i_Keys)
	{
		const smdlKeyRootMap& roots = i_Keys.GetKeyRoots();
		EXPECT_EQ(1u, roots.size());
		EXPECT_TRUE(roots.count(""));
		const smdlTree<smdlGeoAnimKeys>& tree = *roots.begin()->second;
		return tree.GetIterator();
	}
}

TEST(gltfAnimKeys, TreeMirrorsSkeleton)
{
	Rig rig;
	std::unique_ptr<emdlCharacterAnimKeys> keys = rig.Build(rig.m_Body, "Bend");
	ASSERT_TRUE(keys);
	smdlTree<smdlGeoAnimKeys>::const_iterator it = Head(*keys);
	EXPECT_FALSE(it.GetData().HasAnimation());
	ASSERT_EQ(1, it.NumChildren());			// Hips
	it.MoveToChild(0);
	ASSERT_EQ(2, it.NumChildren());			// Spine, LegL
	it.MoveToChild(0);
	EXPECT_EQ(1, it.NumChildren());			// Head
	it.MoveToParent();
	it.MoveToChild(1);
	EXPECT_EQ(0, it.NumChildren());

	EXPECT_FLOAT_EQ(c_Fps, keys->GetFramesPerSecond());
	EXPECT_FLOAT_EQ(0, keys->GetStartTime());
	EXPECT_FALSE(keys->GetAdditiveAnimation());
	EXPECT_TRUE(keys->GetIgnoreJointOrientation());
	EXPECT_FALSE(keys->GetAttachToRootJoint());
}

TEST(gltfAnimKeys, RestFilled)
{
	Rig rig;
	std::unique_ptr<emdlCharacterAnimKeys> keys = rig.Build(rig.m_Body, "Bend");
	ASSERT_TRUE(keys);
	smdlTree<smdlGeoAnimKeys>::const_iterator it = Head(*keys);
	it.MoveToChild(0);
	EXPECT_FALSE(it.GetData().HasAnimation());		// Hips: not in the clip
	it.MoveToChild(0);
	const smdlGeoAnimKeys& spine = it.GetData();
	ASSERT_TRUE(spine.HasAnimation());
	const maVector3d translation = spine.GetTranslation(10);
	EXPECT_NEAR(0, translation.m_X, 1e-4f);
	EXPECT_NEAR(30, translation.m_Y, 1e-4f);
	EXPECT_NEAR(0, translation.m_Z, 1e-4f);
	const maVector3d scale = spine.GetScale(10);
	EXPECT_FLOAT_EQ(1, scale.m_X);
	EXPECT_FLOAT_EQ(1, scale.m_Y);
	EXPECT_FLOAT_EQ(1, scale.m_Z);
}

TEST(gltfAnimKeys, Values)
{
	Rig rig;
	std::unique_ptr<emdlCharacterAnimKeys> keys = rig.Build(rig.m_Body, "Bend");
	ASSERT_TRUE(keys);
	smdlTree<smdlGeoAnimKeys>::const_iterator it = Head(*keys);
	it.MoveToChild(0);
	it.MoveToChild(0);
	const maRotation half = it.GetData().GetRotation(15);
	const float s = sinf(c_Pi / 8), c = cosf(c_Pi / 8);		// 45 degrees about Y
	const float dot = half.GetY() * s + half.GetW() * c;
	EXPECT_NEAR(1, fabsf(dot), 1e-4f);
	EXPECT_NEAR(0, half.GetX(), 1e-4f);
	EXPECT_NEAR(0, half.GetZ(), 1e-4f);
}

TEST(gltfAnimKeys, MorphKeys)
{
	Rig rig;
	std::unique_ptr<emdlCharacterAnimKeys> keys = rig.Build(rig.m_Box, "Morph");
	ASSERT_TRUE(keys);
	const std::map<std::string, smdlMorphAnimKeys>& morphs = keys->GetMorphKeys();
	ASSERT_EQ(2u, morphs.size());
	ASSERT_TRUE(morphs.count("#0"));
	ASSERT_TRUE(morphs.count("#1"));
	EXPECT_FLOAT_EQ(0, morphs.at("#0").GetBlend(0));
	EXPECT_FLOAT_EQ(1, morphs.at("#0").GetBlend(30));
	EXPECT_FLOAT_EQ(0.25f, morphs.at("#1").GetBlend(15));
	EXPECT_FLOAT_EQ(0.5f, morphs.at("#1").GetBlend(30));
	EXPECT_EQ(2u, keys->MorphChannels().size());
}

TEST(gltfAnimKeys, Length)
{
	Rig rig;
	gltfClip clip;
	std::unique_ptr<emdlCharacterAnimKeys> keys = rig.Build(rig.m_Body, "Bend", &clip);
	ASSERT_TRUE(keys);
	smdlCharacterAnimation animation(keys->GetKeyRoots(), keys->GetMorphKeys(), keys->GetVertexKeys(),
									 keys->GetSkinKeys(), keys->GetFramesPerSecond());
	EXPECT_FLOAT_EQ(clip.m_Length, animation.GetNumFrames());
	EXPECT_FLOAT_EQ(30, animation.GetNumFrames());
}

TEST(gltfAnimKeys, UntouchedClip)
{
	Rig rig;
	EXPECT_FALSE(rig.Build(rig.m_Body, "Spin"));	// Box's rotation: nothing of Body's
	EXPECT_FALSE(rig.Build(rig.m_Box, "Spin"));		// a rigid node's own transform: keys (gltfAnimTracks)
}

TEST(gltfAnimKeys, TransformClipHead)
{
	// a leaf mesh at (0, 0, 0.5) m whose scale goes (1,1,1) -> (2,1,1): it can't be keyed
	tinygltf::Model model;
	model.meshes.resize(1);
	model.meshes[0].primitives.resize(1);
	const int box = AddNode(model, "Box");
	model.nodes[box].mesh = 0;
	model.nodes[box].translation = { 0, 0, 0.5 };
	tinygltf::Scene scene;
	scene.nodes = { box };
	model.scenes.push_back(scene);
	AddAnimation(model, "Stretch", box, "scale", { 0, 1 }, { 1, 1, 1,  2, 1, 1 }, TINYGLTF_TYPE_VEC3);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(1u, objects.size());
	ASSERT_TRUE(objects[0].m_bTransformClip);
	gltfClip clip;
	ASSERT_TRUE(gltfAnimRead::ReadClip(model, "Stretch", c_Fps, clip));
	std::unique_ptr<emdlCharacterAnimKeys> keys(gltfAnimKeys::Build(model, objects[0], clip, c_Fps));
	ASSERT_TRUE(keys);
	const smdlGeoAnimKeys& head = Head(*keys).GetData();
	ASSERT_TRUE(head.HasAnimation());
	const maVector3d scale = head.GetScale(15);
	EXPECT_NEAR(1.5f, scale.m_X, 1e-4f);
	EXPECT_NEAR(1, scale.m_Y, 1e-4f);
	EXPECT_NEAR(1, scale.m_Z, 1e-4f);
	const maVector3d translation = head.GetTranslation(15);
	EXPECT_NEAR(0, translation.m_X, 1e-4f);
	EXPECT_NEAR(0, translation.m_Y, 1e-4f);
	EXPECT_NEAR(50, translation.m_Z, 1e-4f);
}
