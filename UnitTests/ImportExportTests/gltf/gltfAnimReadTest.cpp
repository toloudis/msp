/*****************************************************************************
**  gltfAnimReadTest.cpp
**
**      Tests of gltfAnimRead: a glTF animation's channels as keys in
**      frames (linear, step and cubic spline), clip identities and names,
**      and which objects each animation plays on; and the clips
**      gltfSceneRead lists on its objects.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif
#ifndef GLTF_SCENEREAD_HPP
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#endif

#include "ietGltfModel.hpp"

#include <gtest/gtest.h>

#include <cmath>
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

	// adds a one-channel animation and returns its index
	int AddAnimation(tinygltf::Model& io_Model, const std::string& i_Name, int i_Node, const std::string& i_Path,
					 const std::vector<float>& i_Times, const std::vector<float>& i_Values, int i_Type,
					 const std::string& i_Interpolation = "LINEAR")
	{
		tinygltf::AnimationSampler sampler;
		sampler.input = ietAddFloatAccessor(io_Model, i_Times, TINYGLTF_TYPE_SCALAR);
		sampler.output = ietAddFloatAccessor(io_Model, i_Values, i_Type);
		sampler.interpolation = i_Interpolation;
		tinygltf::AnimationChannel channel;
		channel.sampler = 0;
		channel.target_node = i_Node;
		channel.target_path = i_Path;
		tinygltf::Animation animation;
		animation.name = i_Name;
		animation.samplers.push_back(sampler);
		animation.channels.push_back(channel);
		io_Model.animations.push_back(animation);
		return (int)io_Model.animations.size() - 1;
	}

	gltfClip Read(const tinygltf::Model& i_Model, const std::string& i_Identity)
	{
		gltfClip clip;
		EXPECT_TRUE(gltfAnimRead::ReadClip(i_Model, i_Identity, c_Fps, clip));
		return clip;
	}

	// Hips(joint) -> Spine(joint) and skinned Body; rigid Box (mesh 1, 2 morph targets);
	// "Walk" rotates Spine, "Spin" rotates Box, "Morph" sets Box's weights
	struct Scene
	{
		tinygltf::Model m_Model;
		int m_Spine, m_Hips, m_Body, m_Box;

		Scene()
		{
			m_Model.meshes.resize(2);
			m_Model.meshes[1].primitives.resize(1);
			m_Model.meshes[1].primitives[0].targets.resize(2);
			m_Spine = AddNode(m_Model, "Spine");
			m_Hips = AddNode(m_Model, "Hips", { m_Spine });
			m_Body = AddNode(m_Model, "Body");
			m_Model.nodes[m_Body].mesh = 0;
			m_Model.nodes[m_Body].skin = 0;
			m_Box = AddNode(m_Model, "Box");
			m_Model.nodes[m_Box].mesh = 1;
			m_Model.skins.resize(1);
			m_Model.skins[0].joints = { m_Hips, m_Spine };
			tinygltf::Scene scene;
			scene.nodes = { m_Hips, m_Body, m_Box };
			m_Model.scenes.push_back(scene);

			AddAnimation(m_Model, "Walk", m_Spine, "rotation", { 0, 1 }, { 0, 0, 0, 1,  0, 0, 0, 1 }, TINYGLTF_TYPE_VEC4);
			AddAnimation(m_Model, "Spin", m_Box, "rotation", { 0, 1 }, { 0, 0, 0, 1,  0, 0, 0, 1 }, TINYGLTF_TYPE_VEC4);
			AddAnimation(m_Model, "Morph", m_Box, "weights", { 0, 1 }, { 0, 0,  1, 0.5f }, TINYGLTF_TYPE_SCALAR);
		}

		const gltfSceneObject& Object(const std::vector<gltfSceneObject>& i_Objects, int i_Node) const
		{
			for (const gltfSceneObject& o : i_Objects)
			{
				if (o.m_Node == i_Node)
					return o;
			}
			ADD_FAILURE() << "no object for node " << i_Node;
			return i_Objects[0];
		}
	};
}

TEST(gltfAnimRead, ClipIdentities)
{
	tinygltf::Model model;
	model.animations.resize(4);
	model.animations[0].name = "Walk";
	model.animations[2].name = "Run";
	model.animations[3].name = "Run";
	EXPECT_EQ((std::vector<std::string>{ "Walk", "#1", "#2", "#3" }), gltfAnimRead::ClipIdentities(model));
	EXPECT_EQ("Animation 1", gltfAnimRead::ClipName(model, 1));
	EXPECT_EQ("Run", gltfAnimRead::ClipName(model, 3));

	gltfClip clip;
	EXPECT_FALSE(gltfAnimRead::ReadClip(model, "Run", c_Fps, clip));
}

TEST(gltfAnimRead, LinearKeysInFrames)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	AddAnimation(model, "A", n, "translation", { 0, 0.5f, 1 }, { 0, 0, 0,  1, 0, 0,  2, 0, 0 }, TINYGLTF_TYPE_VEC3);

	const gltfClip clip = Read(model, "A");
	EXPECT_EQ("A", clip.m_Identity);
	EXPECT_EQ("A", clip.m_Name);
	ASSERT_EQ(1u, clip.m_Channels.size());
	const gltfAnimChannel& channel = clip.m_Channels[0];
	EXPECT_EQ(n, channel.m_Node);
	EXPECT_EQ(gltfAnimChannel::e_Translation, channel.m_Path);
	EXPECT_EQ(3, channel.m_Components);
	EXPECT_EQ((std::vector<float>{ 0, 15, 30 }), channel.m_Times);
	ASSERT_EQ(9u, channel.m_Values.size());
	EXPECT_NEAR(0, channel.m_Values[0], 1e-4f);
	EXPECT_NEAR(100, channel.m_Values[3], 1e-4f);
	EXPECT_NEAR(200, channel.m_Values[6], 1e-4f);
	EXPECT_FLOAT_EQ(30, clip.m_Length);
}

TEST(gltfAnimRead, ClipStartsAtZero)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	AddAnimation(model, "A", n, "scale", { 2, 3 }, { 1, 1, 1,  2, 2, 2 }, TINYGLTF_TYPE_VEC3);

	const gltfClip clip = Read(model, "A");
	ASSERT_EQ(1u, clip.m_Channels.size());
	ASSERT_EQ(2u, clip.m_Channels[0].m_Times.size());
	EXPECT_NEAR(0, clip.m_Channels[0].m_Times[0], 1e-4f);
	EXPECT_NEAR(30, clip.m_Channels[0].m_Times[1], 1e-4f);
	EXPECT_NEAR(30, clip.m_Length, 1e-4f);
}

TEST(gltfAnimRead, Step)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	AddAnimation(model, "A", n, "scale", { 0, 1 }, { 1, 1, 1,  2, 2, 2 }, TINYGLTF_TYPE_VEC3, "STEP");

	const gltfClip clip = Read(model, "A");
	const gltfAnimChannel& channel = clip.m_Channels[0];
	ASSERT_EQ(3u, channel.m_Times.size());
	EXPECT_FLOAT_EQ(0, channel.m_Times[0]);
	EXPECT_FLOAT_EQ(30 - gltfAnimRead::c_StepEpsilon, channel.m_Times[1]);
	EXPECT_FLOAT_EQ(30, channel.m_Times[2]);
	EXPECT_FLOAT_EQ(1, channel.m_Values[0]);
	EXPECT_FLOAT_EQ(1, channel.m_Values[3]);
	EXPECT_FLOAT_EQ(2, channel.m_Values[6]);
}

TEST(gltfAnimRead, CubicSpline)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	const float s = sinf(c_Pi / 4), c = cosf(c_Pi / 4);
	// per key: in-tangent, value, out-tangent
	AddAnimation(model, "Turn", n, "rotation", { 0, 1 },
				 { 0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,
				   0, 0, 0, 0,  0, s, 0, c,  0, 0, 0, 0 }, TINYGLTF_TYPE_VEC4, "CUBICSPLINE");

	gltfClip clip = Read(model, "Turn");
	const gltfAnimChannel& rotation = clip.m_Channels[0];
	ASSERT_EQ(31u, rotation.m_Times.size());
	EXPECT_FLOAT_EQ(15, rotation.m_Times[15]);
	const float* q = &rotation.m_Values[15 * 4];
	EXPECT_NEAR(1, q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3], 1e-5f);
	EXPECT_NEAR(45, 2 * acosf(q[3]) * 180 / c_Pi, 1e-3f);

	// a translation with tangents: (2t^3 - 3t^2 + 1) v0 + (t^3 - 2t^2 + t) d b0 + (-2t^3 + 3t^2) v1 + (t^3 - t^2) d a1
	AddAnimation(model, "Move", n, "translation", { 0, 2 },
				 { 0, 0, 0,  0, 0, 0,  1, 0, 0,
				   3, 0, 0,  1, 0, 0,  0, 0, 0 }, TINYGLTF_TYPE_VEC3, "CUBICSPLINE");
	clip = Read(model, "Move");
	const gltfAnimChannel& move = clip.m_Channels[0];
	ASSERT_EQ(61u, move.m_Times.size());
	const float t = 0.25f, d = 2;		// frame 15 of 60
	const float expected = ((t * t * t - 2 * t * t + t) * d * 1 + (-2 * t * t * t + 3 * t * t) * 1 + (t * t * t - t * t) * d * 3) * 100;
	EXPECT_FLOAT_EQ(15, move.m_Times[15]);
	EXPECT_NEAR(expected, move.m_Values[15 * 3], 1e-4f);
	EXPECT_NEAR(100, move.m_Values[60 * 3], 1e-4f);
}

TEST(gltfAnimRead, Weights)
{
	Scene scene;
	const gltfClip clip = Read(scene.m_Model, "Morph");
	ASSERT_EQ(1u, clip.m_Channels.size());
	const gltfAnimChannel& channel = clip.m_Channels[0];
	EXPECT_EQ(gltfAnimChannel::e_Weights, channel.m_Path);
	EXPECT_EQ(2, channel.m_Components);
	EXPECT_EQ((std::vector<float>{ 0, 0, 1, 0.5f }), channel.m_Values);
}

TEST(gltfAnimRead, PointerChannelSkipped)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	AddAnimation(model, "A", n, "translation", { 0, 1 }, { 0, 0, 0,  1, 0, 0 }, TINYGLTF_TYPE_VEC3);
	tinygltf::AnimationChannel pointer = model.animations[0].channels[0];
	pointer.target_node = -1;
	pointer.target_path = "pointer";
	model.animations[0].channels.insert(model.animations[0].channels.begin(), pointer);

	const gltfClip clip = Read(model, "A");
	ASSERT_EQ(1u, clip.m_Channels.size());
	EXPECT_EQ(n, clip.m_Channels[0].m_Node);
}

TEST(gltfAnimRead, AnimationsFor)
{
	Scene scene;
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	EXPECT_EQ((std::vector<int>{ 0 }), gltfAnimRead::AnimationsFor(scene.m_Model, scene.Object(objects, scene.m_Body)));
	EXPECT_EQ((std::vector<int>{ 2 }), gltfAnimRead::AnimationsFor(scene.m_Model, scene.Object(objects, scene.m_Box)));
}

TEST(gltfSceneRead, ClipsOnObjects)
{
	Scene scene;
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	const gltfSceneObject& body = scene.Object(objects, scene.m_Body);
	ASSERT_EQ(1u, body.m_Clips.size());
	EXPECT_EQ("Walk", body.m_Clips[0].m_Identity);
	EXPECT_EQ("Walk", body.m_Clips[0].m_Name);
	const gltfSceneObject& box = scene.Object(objects, scene.m_Box);
	ASSERT_EQ(1u, box.m_Clips.size());
	EXPECT_EQ("Morph", box.m_Clips[0].m_Identity);
}

namespace
{
	// adds a sampler to animation i_Animation and returns its index
	int AddSampler(tinygltf::Model& io_Model, int i_Animation, const std::vector<float>& i_Times,
				   const std::vector<float>& i_Values, int i_Type, const std::string& i_Interpolation = "LINEAR")
	{
		tinygltf::AnimationSampler sampler;
		sampler.input = ietAddFloatAccessor(io_Model, i_Times, TINYGLTF_TYPE_SCALAR);
		sampler.output = ietAddFloatAccessor(io_Model, i_Values, i_Type);
		sampler.interpolation = i_Interpolation;
		io_Model.animations[i_Animation].samplers.push_back(sampler);
		return (int)io_Model.animations[i_Animation].samplers.size() - 1;
	}

	// a model with nodes N0, N1, one light, one camera, three materials and an animation "A"
	// translating N0 over 0..1 s
	tinygltf::Model PointerModel()
	{
		tinygltf::Model model;
		const int n0 = AddNode(model, "N0");
		AddNode(model, "N1");
		model.lights.resize(1);
		model.cameras.resize(1);
		model.materials.resize(3);
		AddAnimation(model, "A", n0, "translation", { 0, 1 }, { 0, 0, 0,  1, 0, 0 }, TINYGLTF_TYPE_VEC3);
		return model;
	}

	gltfAnimTarget Resolve(const tinygltf::Model& i_Model, int i_Channel)
	{
		return gltfAnimRead::ResolveTarget(i_Model, i_Model.animations[0].channels[i_Channel]);
	}
}

TEST(gltfAnimRead, NodePointerIsNodeChannel)
{
	tinygltf::Model model = PointerModel();
	const int sampler = AddSampler(model, 0, { 0, 1 }, { 0, 0, 0, 1,  0, 1, 0, 0 }, TINYGLTF_TYPE_VEC4);
	ietAddPointerChannel(model, 0, "/nodes/1/rotation", sampler);

	const gltfAnimTarget target = Resolve(model, 1);
	EXPECT_EQ(gltfAnimTarget::e_Node, target.m_Kind);
	EXPECT_EQ(1, target.m_Index);
	EXPECT_EQ(gltfAnimChannel::e_Rotation, target.m_Path);

	const gltfClip clip = Read(model, "A");
	ASSERT_EQ(2u, clip.m_Channels.size());
	EXPECT_EQ(1, clip.m_Channels[1].m_Node);
	EXPECT_EQ(gltfAnimChannel::e_Rotation, clip.m_Channels[1].m_Path);
	EXPECT_TRUE(clip.m_Pointers.empty());

	// a node pointer counts like a node channel: weights on a morphing mesh
	model.meshes.resize(1);
	model.meshes[0].primitives.resize(1);
	model.meshes[0].primitives[0].targets.resize(1);
	model.nodes[1].mesh = 0;
	const int weights = AddSampler(model, 0, { 0, 1 }, { 0, 1 }, TINYGLTF_TYPE_SCALAR);
	ietAddPointerChannel(model, 0, "/nodes/1/weights", weights);
	EXPECT_TRUE(gltfAnimRead::AnimatesWeights(model, 1));
	gltfSceneObject object;
	object.m_Kind = gltfSceneObject::e_Mesh;
	object.m_Node = 1;
	object.m_Mesh = 0;
	EXPECT_EQ((std::vector<int>{ 0 }), gltfAnimRead::AnimationsFor(model, object));
}

TEST(gltfAnimRead, LightPointer)
{
	tinygltf::Model model = PointerModel();
	const int sampler = AddSampler(model, 0, { 0, 1 }, { 0.5f, 0.75f }, TINYGLTF_TYPE_SCALAR);
	ietAddPointerChannel(model, 0, "/extensions/KHR_lights_punctual/lights/0/spot/outerConeAngle", sampler);

	const gltfAnimTarget target = Resolve(model, 1);
	EXPECT_EQ(gltfAnimTarget::e_Light, target.m_Kind);
	EXPECT_EQ(0, target.m_Index);
	EXPECT_EQ("spot/outerConeAngle", target.m_Property);

	const gltfClip clip = Read(model, "A");
	EXPECT_EQ(1u, clip.m_Channels.size());
	ASSERT_EQ(1u, clip.m_Pointers.size());
	const gltfPointerChannel& pointer = clip.m_Pointers[0];
	EXPECT_EQ(-1, pointer.m_Node);
	EXPECT_EQ(gltfAnimTarget::e_Light, pointer.m_Target.m_Kind);
	EXPECT_EQ(1, pointer.m_Components);
	EXPECT_EQ((std::vector<float>{ 0, 30 }), pointer.m_Times);
	EXPECT_EQ((std::vector<float>{ 0.5f, 0.75f }), pointer.m_Values);
}

TEST(gltfAnimRead, CameraPointer)
{
	tinygltf::Model model = PointerModel();
	const int sampler = AddSampler(model, 0, { 0, 1 }, { 0.5f, 1 }, TINYGLTF_TYPE_SCALAR);
	ietAddPointerChannel(model, 0, "/cameras/0/perspective/yfov", sampler);

	const gltfAnimTarget target = Resolve(model, 1);
	EXPECT_EQ(gltfAnimTarget::e_Camera, target.m_Kind);
	EXPECT_EQ(0, target.m_Index);
	EXPECT_EQ("perspective/yfov", target.m_Property);
}

TEST(gltfAnimRead, MaterialPointer)
{
	tinygltf::Model model = PointerModel();
	const int sampler = AddSampler(model, 0, { 0, 1 }, { 1, 0, 0, 1,  0, 0, 1, 1 }, TINYGLTF_TYPE_VEC4);
	ietAddPointerChannel(model, 0, "/materials/2/pbrMetallicRoughness/baseColorFactor", sampler);

	const gltfAnimTarget target = Resolve(model, 1);
	EXPECT_EQ(gltfAnimTarget::e_Material, target.m_Kind);
	EXPECT_EQ(2, target.m_Index);
	EXPECT_EQ("pbrMetallicRoughness/baseColorFactor", target.m_Property);

	const gltfClip clip = Read(model, "A");
	ASSERT_EQ(1u, clip.m_Pointers.size());
	EXPECT_EQ(4, clip.m_Pointers[0].m_Components);
	EXPECT_EQ(8u, clip.m_Pointers[0].m_Values.size());
}

TEST(gltfAnimRead, UnsupportedPointer)
{
	tinygltf::Model model = PointerModel();
	const int vec2 = AddSampler(model, 0, { 0, 1 }, { 0, 0,  1, 1 }, TINYGLTF_TYPE_VEC2);
	const int scalar = AddSampler(model, 0, { 0, 1 }, { 0, 1 }, TINYGLTF_TYPE_SCALAR);
	const int vec3 = AddSampler(model, 0, { 0, 1 }, { 0, 0, 0,  1, 1, 1 }, TINYGLTF_TYPE_VEC3);
	ietAddPointerChannel(model, 0, "/materials/0/extensions/KHR_texture_transform/offset", vec2);
	ietAddPointerChannel(model, 0, "/meshes/0/weights", scalar);
	ietAddPointerChannel(model, 0, "/materials/9/emissiveFactor", vec3);

	for (int c = 1; c <= 3; c++)
		EXPECT_EQ(gltfAnimTarget::e_None, Resolve(model, c).m_Kind) << "channel " << c;

	const gltfClip clip = Read(model, "A");
	EXPECT_EQ(1u, clip.m_Channels.size());
	EXPECT_TRUE(clip.m_Pointers.empty());
}

TEST(gltfAnimRead, StepKeptForKeys)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	AddAnimation(model, "A", n, "scale", { 0, 1 }, { 1, 1, 1,  2, 2, 2 }, TINYGLTF_TYPE_VEC3, "STEP");

	gltfClip clip;
	ASSERT_TRUE(gltfAnimRead::ReadClip(model, "A", c_Fps, clip, false));
	const gltfAnimChannel& channel = clip.m_Channels[0];
	EXPECT_TRUE(channel.m_bStep);
	EXPECT_EQ((std::vector<float>{ 0, 30 }), channel.m_Times);
	ASSERT_EQ(6u, channel.m_Values.size());
	EXPECT_FLOAT_EQ(1, channel.m_Values[0]);
	EXPECT_FLOAT_EQ(2, channel.m_Values[3]);

	// as holds (the default): gltfAnimRead.Step
	const gltfClip holds = Read(model, "A");
	EXPECT_FALSE(holds.m_Channels[0].m_bStep);
	EXPECT_EQ(3u, holds.m_Channels[0].m_Times.size());
}

TEST(gltfAnimRead, ClipStartIncludesPointers)
{
	tinygltf::Model model;
	const int n = AddNode(model, "N");
	model.lights.resize(1);
	AddAnimation(model, "A", n, "translation", { 1, 2 }, { 0, 0, 0,  1, 0, 0 }, TINYGLTF_TYPE_VEC3);
	const int sampler = AddSampler(model, 0, { 0.5f, 3 }, { 1, 2 }, TINYGLTF_TYPE_SCALAR);
	ietAddPointerChannel(model, 0, "/extensions/KHR_lights_punctual/lights/0/intensity", sampler);

	const gltfClip clip = Read(model, "A");
	ASSERT_EQ(1u, clip.m_Channels.size());
	EXPECT_NEAR(15, clip.m_Channels[0].m_Times[0], 1e-4f);
	EXPECT_NEAR(45, clip.m_Channels[0].m_Times[1], 1e-4f);
	ASSERT_EQ(1u, clip.m_Pointers.size());
	EXPECT_NEAR(0, clip.m_Pointers[0].m_Times[0], 1e-4f);
	EXPECT_NEAR(75, clip.m_Length, 1e-4f);
}
