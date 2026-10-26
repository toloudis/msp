/*****************************************************************************
**  gltfAnimTracksTest.cpp
**
**      Tests of gltfAnimTracks: when each object's animations play,
**      reducing keys, sampling tracks, and the keys-vs-clip rule.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMTRACKS_HPP
#include "ImportExport/gltf/import/private/gltfAnimTracks.hpp"
#endif
#ifndef GLTF_SCENEREAD_HPP
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
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

	int AddMeshNode(tinygltf::Model& io_Model, const std::string& i_Name, std::vector<int> i_Children = {})
	{
		if (io_Model.meshes.empty())
		{
			io_Model.meshes.resize(1);
			io_Model.meshes[0].primitives.resize(1);
		}
		const int node = AddNode(io_Model, i_Name, i_Children);
		io_Model.nodes[node].mesh = 0;
		return node;
	}

	void SetScene(tinygltf::Model& io_Model, const std::vector<int>& i_Roots)
	{
		tinygltf::Scene scene;
		scene.nodes = i_Roots;
		io_Model.scenes.push_back(scene);
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

	// a translation along x from 0 to i_Metres over i_Seconds
	int AddMove(tinygltf::Model& io_Model, const std::string& i_Name, int i_Node, float i_Seconds, float i_Metres = 1)
	{
		return AddAnimation(io_Model, i_Name, i_Node, "translation", { 0, i_Seconds },
							{ 0, 0, 0,  i_Metres, 0, 0 }, TINYGLTF_TYPE_VEC3);
	}

	const gltfSceneObject* Find(const std::vector<gltfSceneObject>& i_Objects, int i_Node, gltfSceneObject::Kind i_Kind)
	{
		for (const gltfSceneObject& o : i_Objects)
		{
			if (o.m_Node == i_Node && o.m_Kind == i_Kind)
				return &o;
		}
		ADD_FAILURE() << "no object for node " << i_Node;
		return NULL;
	}

	const gltfTrack* FindTrack(const gltfSceneObject& i_Object, gltfTrack::Property i_Property)
	{
		for (const gltfTrack& track : i_Object.m_Tracks)
		{
			if (track.m_Property == i_Property)
				return &track;
		}
		return NULL;
	}

	gltfTrack Track(gltfTrack::Property i_Property, int i_Components, bool i_bStep = false)
	{
		gltfTrack track;
		track.m_Property = i_Property;
		track.m_Components = i_Components;
		track.m_bStep = i_bStep;
		return track;
	}

	void AddKey(gltfTrack& io_Track, float i_Time, std::vector<float> i_Value)
	{
		io_Track.m_Times.push_back(i_Time);
		io_Track.m_Values.insert(io_Track.m_Values.end(), i_Value.begin(), i_Value.end());
	}

	// a rotation of i_Degrees about Y, x y z w
	std::vector<float> AboutY(float i_Degrees)
	{
		const float half = i_Degrees * c_Pi / 360;
		return { 0, sinf(half), 0, cosf(half) };
	}
}

TEST(gltfAnimTracks, ReduceLinear)
{
	gltfTrack track = Track(gltfTrack::e_Translation, 3);
	for (int k = 0; k <= 60; k++)
		AddKey(track, k / 30.0f, { 100.0f * k / 60, 0, 0 });
	gltfAnimTracks::Reduce(track);
	ASSERT_EQ(2u, track.m_Times.size());
	EXPECT_FLOAT_EQ(0, track.m_Times[0]);
	EXPECT_FLOAT_EQ(2, track.m_Times[1]);
	EXPECT_NEAR(100, track.m_Values[3], 1e-4f);
}

TEST(gltfAnimTracks, ReduceKeepsCorners)
{
	gltfTrack track = Track(gltfTrack::e_Translation, 3);
	for (int k = 0; k <= 60; k++)
		AddKey(track, k / 30.0f, { k <= 30 ? 100.0f * k / 30 : 100.0f * (60 - k) / 30, 0, 0 });
	gltfAnimTracks::Reduce(track);
	ASSERT_EQ(3u, track.m_Times.size());
	EXPECT_NEAR(0, track.m_Times[0], 1e-5f);
	EXPECT_NEAR(1, track.m_Times[1], 1e-5f);
	EXPECT_NEAR(2, track.m_Times[2], 1e-5f);
}

TEST(gltfAnimTracks, ReduceRotation)
{
	gltfTrack track = Track(gltfTrack::e_Rotation, 4);
	for (int k = 0; k <= 30; k++)
		AddKey(track, k / 30.0f, AboutY(3.0f * k));
	gltfTrack bumped = track;
	gltfAnimTracks::Reduce(track);
	EXPECT_EQ(2u, track.m_Times.size());

	// 0.1 degrees off the slerp at frame 15: kept
	const std::vector<float> bump = AboutY(45.1f);
	for (int c = 0; c < 4; c++)
		bumped.m_Values[15 * 4 + c] = bump[c];
	gltfAnimTracks::Reduce(bumped);
	EXPECT_GT(bumped.m_Times.size(), 2u);
	bool bKept = false;
	for (float t : bumped.m_Times)
		bKept |= fabsf(t - 0.5f) < 1e-5f;
	EXPECT_TRUE(bKept);
}

TEST(gltfAnimTracks, ReduceStep)
{
	gltfTrack track = Track(gltfTrack::e_Scale, 3, true);
	const float values[] = { 1, 1, 2, 2, 1 };
	for (int k = 0; k < 5; k++)
		AddKey(track, (float)k, { values[k], values[k], values[k] });
	gltfAnimTracks::Reduce(track);
	EXPECT_EQ((std::vector<float>{ 0, 2, 4 }), track.m_Times);
	EXPECT_EQ((std::vector<float>{ 1, 1, 1,  2, 2, 2,  1, 1, 1 }), track.m_Values);
}

TEST(gltfAnimTracks, Sample)
{
	gltfTrack linear = Track(gltfTrack::e_LightIntensity, 1);
	AddKey(linear, 1, { 10 });
	AddKey(linear, 2, { 20 });
	float v = 0;
	gltfAnimTracks::Sample(linear, 1.5f, &v);
	EXPECT_FLOAT_EQ(15, v);
	gltfAnimTracks::Sample(linear, 0, &v);
	EXPECT_FLOAT_EQ(10, v);
	gltfAnimTracks::Sample(linear, 3, &v);
	EXPECT_FLOAT_EQ(20, v);

	gltfTrack step = linear;
	step.m_bStep = true;
	gltfAnimTracks::Sample(step, 1.9f, &v);
	EXPECT_FLOAT_EQ(10, v);

	gltfTrack rotation = Track(gltfTrack::e_Rotation, 4);
	AddKey(rotation, 0, AboutY(0));
	AddKey(rotation, 1, AboutY(90));
	float q[4];
	gltfAnimTracks::Sample(rotation, 0.5f, q);
	const std::vector<float> expected = AboutY(45);
	for (int c = 0; c < 4; c++)
		EXPECT_NEAR(expected[c], q[c], 1e-4f);
}

TEST(gltfAnimTracks, Schedule)
{
	tinygltf::Model model;
	const int a = AddMeshNode(model, "A");
	const int b = AddMeshNode(model, "B");
	SetScene(model, { a, b });
	AddMove(model, "First", a, 1);
	AddMove(model, "Second", b, 2);
	AddMove(model, "Third", a, 0.5f);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const std::vector<gltfScheduledAnim> scheduleA =
		gltfAnimTracks::Schedule(model, *Find(objects, a, gltfSceneObject::e_Mesh), c_Fps);
	ASSERT_EQ(2u, scheduleA.size());
	EXPECT_EQ(0, scheduleA[0].m_Animation);
	EXPECT_NEAR(0, scheduleA[0].m_Begin, 1e-5f);
	EXPECT_EQ(2, scheduleA[1].m_Animation);
	EXPECT_NEAR(1, scheduleA[1].m_Begin, 1e-5f);

	const std::vector<gltfScheduledAnim> scheduleB =
		gltfAnimTracks::Schedule(model, *Find(objects, b, gltfSceneObject::e_Mesh), c_Fps);
	ASSERT_EQ(1u, scheduleB.size());
	EXPECT_EQ(1, scheduleB[0].m_Animation);
	EXPECT_NEAR(0, scheduleB[0].m_Begin, 1e-5f);

	// A's keys: the first move, then the third from 1 s (the third's first key replaces the first's last)
	const gltfTrack* move = FindTrack(*Find(objects, a, gltfSceneObject::e_Mesh), gltfTrack::e_Translation);
	ASSERT_TRUE(move);
	EXPECT_EQ((std::vector<float>{ 0, 1, 1.5f }), move->m_Times);
	EXPECT_NEAR(0, move->m_Values[3], 1e-4f);
	EXPECT_NEAR(100, move->m_Values[6], 1e-4f);
}

TEST(gltfAnimTracks, RigidMeshKeys)
{
	tinygltf::Model model;
	const int n = AddMeshNode(model, "Box");
	SetScene(model, { n });
	AddMove(model, "Slide", n, 1);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject& box = *Find(objects, n, gltfSceneObject::e_Mesh);
	EXPECT_FALSE(box.m_bTransformClip);
	EXPECT_TRUE(box.m_Clips.empty());
	const gltfTrack* move = FindTrack(box, gltfTrack::e_Translation);
	ASSERT_TRUE(move);
	EXPECT_EQ(3, move->m_Components);
	EXPECT_FALSE(move->m_bStep);
	EXPECT_EQ((std::vector<float>{ 0, 1 }), move->m_Times);
	EXPECT_NEAR(100, move->m_Values[3], 1e-4f);
}

TEST(gltfAnimTracks, NonUniformScaleToClip)
{
	tinygltf::Model model;
	const int n = AddMeshNode(model, "Box");
	SetScene(model, { n });
	AddAnimation(model, "Stretch", n, "scale", { 0, 1 }, { 1, 1, 1,  2, 1, 1 }, TINYGLTF_TYPE_VEC3);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject& box = *Find(objects, n, gltfSceneObject::e_Mesh);
	EXPECT_TRUE(box.m_bTransformClip);
	EXPECT_TRUE(box.m_Tracks.empty());
	ASSERT_EQ(1u, box.m_Clips.size());
	EXPECT_EQ("Stretch", box.m_Clips[0].m_Identity);
	EXPECT_FLOAT_EQ(0, box.m_Clips[0].m_Begin);
}

TEST(gltfAnimTracks, TooManyKeysToClip)
{
	tinygltf::Model model;
	const int n = AddMeshNode(model, "Box");
	SetScene(model, { n });
	std::vector<float> times, values;
	for (int k = 0; k < 400; k++)
	{
		times.push_back(k / 30.0f);
		values.insert(values.end(), { (k % 2) ? 1.0f : 0.0f, 0, 0 });
	}
	AddAnimation(model, "Shake", n, "translation", times, values, TINYGLTF_TYPE_VEC3);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	EXPECT_TRUE(Find(objects, n, gltfSceneObject::e_Mesh)->m_bTransformClip);
}

TEST(gltfAnimTracks, GroupAlwaysKeys)
{
	tinygltf::Model model;
	const int child = AddMeshNode(model, "Child");
	const int group = AddNode(model, "Group", { child });
	SetScene(model, { group });
	AddAnimation(model, "Stretch", group, "scale", { 0, 1 }, { 1, 1, 1,  2, 1, 1 }, TINYGLTF_TYPE_VEC3);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfTrack* scale = FindTrack(*Find(objects, group, gltfSceneObject::e_Group), gltfTrack::e_Scale);
	ASSERT_TRUE(scale);
	EXPECT_EQ(3, scale->m_Components);
	EXPECT_EQ((std::vector<float>{ 1, 1, 1,  2, 1, 1 }), scale->m_Values);
}

TEST(gltfAnimTracks, ContentChildTakesNoTransform)
{
	tinygltf::Model model;
	const int child = AddMeshNode(model, "Child");
	const int parent = AddMeshNode(model, "Parent", { child });
	SetScene(model, { parent });
	AddMove(model, "Slide", parent, 1);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	EXPECT_TRUE(FindTrack(*Find(objects, parent, gltfSceneObject::e_Group), gltfTrack::e_Translation));
	EXPECT_TRUE(Find(objects, parent, gltfSceneObject::e_Mesh)->m_Tracks.empty());
	EXPECT_TRUE(Find(objects, child, gltfSceneObject::e_Mesh)->m_Tracks.empty());
}

TEST(gltfAnimTracks, SkinnedNodeIgnored)
{
	tinygltf::Model model;
	const int hips = AddNode(model, "Hips");
	const int body = AddMeshNode(model, "Body");
	model.nodes[body].skin = 0;
	model.skins.resize(1);
	model.skins[0].joints = { hips };
	SetScene(model, { hips, body });
	AddMove(model, "Slide", body, 1);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject& skinned = *Find(objects, body, gltfSceneObject::e_SkinnedMesh);
	EXPECT_TRUE(skinned.m_Tracks.empty());
	EXPECT_TRUE(skinned.m_Clips.empty());
}

TEST(gltfAnimTracks, UnsupportedPointerSkipped)
{
	tinygltf::Model model;
	const int n = AddMeshNode(model, "Box");
	SetScene(model, { n });
	model.materials.resize(1);
	AddMove(model, "Slide", n, 1);
	tinygltf::AnimationSampler sampler;
	sampler.input = ietAddFloatAccessor(model, { 0, 1 }, TINYGLTF_TYPE_SCALAR);
	sampler.output = ietAddFloatAccessor(model, { 0, 0,  1, 1 }, TINYGLTF_TYPE_VEC2);
	model.animations[0].samplers.push_back(sampler);
	ietAddPointerChannel(model, 0, "/materials/0/extensions/KHR_texture_transform/offset", 1);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject& box = *Find(objects, n, gltfSceneObject::e_Mesh);
	EXPECT_FALSE(box.m_bTransformClip);
	EXPECT_EQ(1u, box.m_Tracks.size());
	EXPECT_TRUE(FindTrack(box, gltfTrack::e_Translation));
}

TEST(gltfSceneRead, TransformClipPlacement)
{
	tinygltf::Model model;
	const int n = AddMeshNode(model, "Box");
	model.nodes[n].translation = { 1, 2, 3 };
	SetScene(model, { n });
	AddAnimation(model, "Stretch", n, "scale", { 0, 1 }, { 1, 1, 1,  2, 1, 1 }, TINYGLTF_TYPE_VEC3);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject& box = *Find(objects, n, gltfSceneObject::e_Mesh);
	ASSERT_TRUE(box.m_bTransformClip);
	EXPECT_FLOAT_EQ(0, box.m_Translation.m_X);
	EXPECT_FLOAT_EQ(0, box.m_Translation.m_Y);
	EXPECT_FLOAT_EQ(0, box.m_Translation.m_Z);
	EXPECT_FLOAT_EQ(1, box.m_Rotation.GetW());
	EXPECT_FLOAT_EQ(1, box.m_Scale.m_X);
	ASSERT_EQ(1u, box.m_Clips.size());
	EXPECT_EQ("Stretch", box.m_Clips[0].m_Identity);
	EXPECT_FLOAT_EQ(0, box.m_Clips[0].m_Begin);
}

namespace
{
	// a pointer animation of one sampler; returns the animation's index
	int AddPointerAnimation(tinygltf::Model& io_Model, const std::string& i_Name, const std::string& i_Pointer,
							const std::vector<float>& i_Times, const std::vector<float>& i_Values, int i_Type)
	{
		tinygltf::AnimationSampler sampler;
		sampler.input = ietAddFloatAccessor(io_Model, i_Times, TINYGLTF_TYPE_SCALAR);
		sampler.output = ietAddFloatAccessor(io_Model, i_Values, i_Type);
		sampler.interpolation = "LINEAR";
		tinygltf::Animation animation;
		animation.name = i_Name;
		animation.samplers.push_back(sampler);
		io_Model.animations.push_back(animation);
		const int index = (int)io_Model.animations.size() - 1;
		ietAddPointerChannel(io_Model, index, i_Pointer, 0);
		return index;
	}

	// a scene with a light node (light 0, spot) and a camera node (camera 0)
	struct LightCameraScene
	{
		tinygltf::Model m_Model;
		int m_LightNode, m_CameraNode;

		LightCameraScene()
		{
			tinygltf::Light light;
			light.type = "spot";
			m_Model.lights.push_back(light);
			tinygltf::Camera camera;
			camera.type = "perspective";
			camera.perspective.yfov = 0.5;
			camera.perspective.znear = 0.1;
			m_Model.cameras.push_back(camera);
			m_LightNode = AddNode(m_Model, "Lamp");
			m_Model.nodes[m_LightNode].light = 0;
			m_CameraNode = AddNode(m_Model, "Eye");
			m_Model.nodes[m_CameraNode].camera = 0;
			SetScene(m_Model, { m_LightNode, m_CameraNode });
		}
	};
}

TEST(gltfAnimTracks, LightColorTrack)
{
	LightCameraScene scene;
	AddPointerAnimation(scene.m_Model, "Glow", "/extensions/KHR_lights_punctual/lights/0/color", { 0, 1 },
						{ 1, 0, 0,  0, 0, 1 }, TINYGLTF_TYPE_VEC3);
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	const gltfTrack* color = FindTrack(*Find(objects, scene.m_LightNode, gltfSceneObject::e_Light), gltfTrack::e_LightColor);
	ASSERT_TRUE(color);
	EXPECT_EQ(3, color->m_Components);
	EXPECT_EQ((std::vector<float>{ 0, 1 }), color->m_Times);
	EXPECT_EQ((std::vector<float>{ 1, 0, 0,  0, 0, 1 }), color->m_Values);
	EXPECT_TRUE(Find(objects, scene.m_CameraNode, gltfSceneObject::e_Camera)->m_Tracks.empty());
}

TEST(gltfAnimTracks, LightRangeCentimetres)
{
	LightCameraScene scene;
	AddPointerAnimation(scene.m_Model, "Reach", "/extensions/KHR_lights_punctual/lights/0/range", { 0, 1 }, { 2, 4 },
						TINYGLTF_TYPE_SCALAR);
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	const gltfTrack* range = FindTrack(*Find(objects, scene.m_LightNode, gltfSceneObject::e_Light), gltfTrack::e_LightRange);
	ASSERT_TRUE(range);
	ASSERT_EQ(2u, range->m_Values.size());
	EXPECT_NEAR(200, range->m_Values[0], 1e-4f);
	EXPECT_NEAR(400, range->m_Values[1], 1e-4f);
}

TEST(gltfAnimTracks, CameraYFovTrack)
{
	LightCameraScene scene;
	AddPointerAnimation(scene.m_Model, "Zoom", "/cameras/0/perspective/yfov", { 0, 1 }, { 0.5f, 1 }, TINYGLTF_TYPE_SCALAR);
	AddPointerAnimation(scene.m_Model, "Clip", "/cameras/0/perspective/znear", { 0, 1 }, { 0.1f, 0.2f }, TINYGLTF_TYPE_SCALAR);
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	const gltfSceneObject& camera = *Find(objects, scene.m_CameraNode, gltfSceneObject::e_Camera);
	const gltfTrack* fov = FindTrack(camera, gltfTrack::e_CameraYFov);
	ASSERT_TRUE(fov);
	EXPECT_EQ((std::vector<float>{ 0.5f, 1 }), fov->m_Values);
	const gltfTrack* znear = FindTrack(camera, gltfTrack::e_CameraNear);
	ASSERT_TRUE(znear);
	EXPECT_NEAR(10, znear->m_Values[0], 1e-4f);
	EXPECT_NEAR(1, znear->m_Times[0], 1e-5f);		// the second animation, after the first
}

TEST(gltfAnimTracks, MaterialTrackOnEachMesh)
{
	tinygltf::Model model;
	model.materials.resize(2);
	model.materials[1].name = "Paint";
	model.meshes.resize(2);
	model.meshes[0].primitives.resize(2);
	model.meshes[0].primitives[0].material = 1;
	model.meshes[0].primitives[1].material = 0;
	model.meshes[1].primitives.resize(1);
	model.meshes[1].primitives[0].material = 0;
	const int a = AddNode(model, "A");
	const int b = AddNode(model, "B");
	const int c = AddNode(model, "C");
	model.nodes[a].mesh = 0;
	model.nodes[b].mesh = 0;
	model.nodes[c].mesh = 1;
	SetScene(model, { a, b, c });
	AddPointerAnimation(model, "Fade", "/materials/1/pbrMetallicRoughness/baseColorFactor", { 0, 1 },
						{ 1, 1, 1, 1,  1, 0, 0, 1 }, TINYGLTF_TYPE_VEC4);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	EXPECT_EQ((std::vector<int>{ 0, 1 }), Find(objects, a, gltfSceneObject::e_Mesh)->m_Materials);
	for (int node : { a, b })
	{
		const gltfTrack* color = FindTrack(*Find(objects, node, gltfSceneObject::e_Mesh), gltfTrack::e_MaterialParam);
		ASSERT_TRUE(color) << "node " << node;
		EXPECT_EQ("Paint", color->m_Material);
		EXPECT_EQ(std::string(effPbrData::c_BaseColor), color->m_Param);
		EXPECT_EQ(4, color->m_Components);
	}
	EXPECT_TRUE(Find(objects, c, gltfSceneObject::e_Mesh)->m_Tracks.empty());
}

TEST(gltfAnimTracks, NormalScaleTrack)
{
	tinygltf::Model model;
	model.materials.resize(1);
	const int a = AddMeshNode(model, "A");
	model.meshes[0].primitives[0].material = 0;
	SetScene(model, { a });
	AddPointerAnimation(model, "Bump", "/materials/0/normalTexture/scale", { 0, 1 }, { 1, 2 }, TINYGLTF_TYPE_SCALAR);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfTrack* scale = FindTrack(*Find(objects, a, gltfSceneObject::e_Mesh), gltfTrack::e_MaterialNormalScale);
	ASSERT_TRUE(scale);
	EXPECT_EQ("Material_0", scale->m_Material);
}

TEST(gltfAnimTracks, PointerAnimationScheduled)
{
	LightCameraScene scene;
	AddMove(scene.m_Model, "Slide", scene.m_LightNode, 2);
	AddPointerAnimation(scene.m_Model, "Dim", "/extensions/KHR_lights_punctual/lights/0/intensity", { 0, 1 }, { 10, 5 },
						TINYGLTF_TYPE_SCALAR);
	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(scene.m_Model);
	const gltfSceneObject& light = *Find(objects, scene.m_LightNode, gltfSceneObject::e_Light);
	const gltfTrack* intensity = FindTrack(light, gltfTrack::e_LightIntensity);
	ASSERT_TRUE(intensity);
	EXPECT_EQ((std::vector<float>{ 2, 3 }), intensity->m_Times);
	EXPECT_TRUE(FindTrack(light, gltfTrack::e_Translation));
}
