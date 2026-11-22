/*****************************************************************************
**  gltfSceneReadTest.cpp
**
**      Tests of gltfSceneRead: a glTF scene's node tree read into the list
**      of objects it becomes (groups, meshes, lights, cameras) with their
**      identity paths, centimetre transforms, and light and camera values.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SCENEREAD_HPP
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <vector>

namespace
{
	const float c_Pi = 3.14159265358979f;

	// adds a node and returns its index
	int AddNode(tinygltf::Model& io_Model, const std::string& i_Name, std::vector<int> i_Children = {})
	{
		tinygltf::Node node;
		node.name = i_Name;
		node.children = i_Children;
		io_Model.nodes.push_back(node);
		return (int)io_Model.nodes.size() - 1;
	}

	void AddScene(tinygltf::Model& io_Model, std::vector<int> i_Roots)
	{
		tinygltf::Scene scene;
		scene.nodes = i_Roots;
		io_Model.scenes.push_back(scene);
	}

	std::vector<std::string> Identities(const std::vector<gltfSceneObject>& i_Objects)
	{
		std::vector<std::string> ids;
		for (const gltfSceneObject& o : i_Objects)
			ids.push_back(o.m_Identity);
		return ids;
	}

	void ExpectVector(const maVector3d& i_V, float x, float y, float z, float i_Tolerance = 1e-5f)
	{
		EXPECT_NEAR(x, i_V.m_X, i_Tolerance);
		EXPECT_NEAR(y, i_V.m_Y, i_Tolerance);
		EXPECT_NEAR(z, i_V.m_Z, i_Tolerance);
	}

	// the same rotation (q and -q are one rotation)
	void ExpectRotation(const maRotation& i_Expected, const maRotation& i_Actual)
	{
		const float dot = i_Expected.GetX() * i_Actual.GetX() + i_Expected.GetY() * i_Actual.GetY()
						+ i_Expected.GetZ() * i_Actual.GetZ() + i_Expected.GetW() * i_Actual.GetW();
		EXPECT_NEAR(1, fabsf(dot), 1e-5f);
	}

	void ExpectIdentityTransform(const gltfSceneObject& i_Object)
	{
		ExpectVector(i_Object.m_Translation, 0, 0, 0);
		ExpectRotation(maRotation(), i_Object.m_Rotation);
		ExpectVector(i_Object.m_Scale, 1, 1, 1);
		EXPECT_TRUE(i_Object.m_bUniformScale);
	}
}

TEST(gltfSceneRead, MeshesLightsCamerasGroups)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	model.lights.resize(1);
	model.lights[0].type = "point";
	model.cameras.resize(1);
	const int body = AddNode(model, "Body");
	model.nodes[body].mesh = 0;
	const int lamp = AddNode(model, "Lamp");
	model.nodes[lamp].light = 0;
	const int car = AddNode(model, "Car", { body, lamp });
	const int cam = AddNode(model, "Cam");
	model.nodes[cam].camera = 0;
	const int empty = AddNode(model, "Empty");
	AddScene(model, { car, cam, empty });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ((std::vector<std::string>{ "Car", "Car/Body:mesh", "Car/Lamp:light", "Cam:camera" }), Identities(objects));

	EXPECT_EQ(gltfSceneObject::e_Group, objects[0].m_Kind);
	EXPECT_EQ(-1, objects[0].m_Parent);
	EXPECT_EQ(car, objects[0].m_Node);
	EXPECT_EQ("Car", objects[0].m_Name);

	EXPECT_EQ(gltfSceneObject::e_Mesh, objects[1].m_Kind);
	EXPECT_EQ(0, objects[1].m_Parent);
	EXPECT_EQ(0, objects[1].m_Mesh);
	EXPECT_EQ("Body", objects[1].m_Name);

	EXPECT_EQ(gltfSceneObject::e_Light, objects[2].m_Kind);
	EXPECT_EQ(0, objects[2].m_Parent);
	EXPECT_EQ(0, objects[2].m_Light);
	EXPECT_EQ(-1, objects[2].m_Mesh);

	EXPECT_EQ(gltfSceneObject::e_Camera, objects[3].m_Kind);
	EXPECT_EQ(-1, objects[3].m_Parent);
	EXPECT_EQ(0, objects[3].m_Camera);
	EXPECT_EQ(cam, objects[3].m_Node);
}

TEST(gltfSceneRead, NodeWithContentAndChildren)
{
	tinygltf::Model model;
	model.meshes.resize(2);
	model.meshes[0].name = "ArmMesh";
	const int hand = AddNode(model, "Hand");
	model.nodes[hand].mesh = 1;
	const int arm = AddNode(model, "Arm", { hand });
	model.nodes[arm].mesh = 0;
	model.nodes[arm].translation = { 1, 0, 0 };
	AddScene(model, { arm });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ((std::vector<std::string>{ "Arm", "Arm:mesh", "Arm/Hand:mesh" }), Identities(objects));

	EXPECT_EQ(gltfSceneObject::e_Group, objects[0].m_Kind);
	ExpectVector(objects[0].m_Translation, 100, 0, 0);
	EXPECT_EQ(-1, objects[0].m_Mesh);

	EXPECT_EQ(gltfSceneObject::e_Mesh, objects[1].m_Kind);
	EXPECT_EQ(0, objects[1].m_Parent);
	EXPECT_EQ(0, objects[1].m_Mesh);
	EXPECT_EQ(arm, objects[1].m_Node);
	ExpectIdentityTransform(objects[1]);

	EXPECT_EQ(0, objects[2].m_Parent);
	EXPECT_EQ(1, objects[2].m_Mesh);
}

TEST(gltfSceneRead, Names)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	model.meshes[0].name = "MeshName";
	const int named = AddNode(model, "");
	model.nodes[named].mesh = 0;
	const int bare = AddNode(model, "");
	model.lights.resize(1);
	model.lights[0].type = "point";
	model.nodes[bare].light = 0;
	AddScene(model, { named, bare });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(2u, objects.size());
	EXPECT_EQ("MeshName", objects[0].m_Name);
	EXPECT_EQ("Node_1", objects[1].m_Name);
}

TEST(gltfSceneRead, IdentityFallsBackToIndex)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	const int car = AddNode(model, "Car");
	const int w1 = AddNode(model, "Wheel");
	const int w2 = AddNode(model, "Wheel");
	const int w3 = AddNode(model, "");
	const int door = AddNode(model, "Door");
	for (int n : { w1, w2, w3, door })
		model.nodes[n].mesh = 0;
	model.nodes[car].children = { w1, w2, w3, door };
	AddScene(model, { car });

	EXPECT_EQ((std::vector<std::string>{ "Car", "Car/#1:mesh", "Car/#2:mesh", "Car/#3:mesh", "Car/Door:mesh" }),
			  Identities(gltfSceneRead::ReadScene(model)));

	tinygltf::Model roots;
	roots.meshes.resize(1);
	for (int i = 0; i < 5; i++)
		roots.nodes[AddNode(roots, (i == 0 || i == 4) ? "Root" : "Other")].mesh = 0;
	AddScene(roots, { 0, 4 });
	EXPECT_EQ((std::vector<std::string>{ "#0:mesh", "#4:mesh" }), Identities(gltfSceneRead::ReadScene(roots)));
}

TEST(gltfSceneRead, WideSiblingListKeepsIdentityRules)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	std::vector<int> roots = { -1, 99999 };
	for (int i = 0; i < 6594; ++i)
	{
		const std::string name = i < 2 ? "Duplicate" : (i == 2 ? "" : "Node_" + std::to_string(i));
		const int node = AddNode(model, name);
		model.nodes[node].mesh = 0;
		roots.push_back(node);
	}
	AddScene(model, roots);
	const auto objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(6594u, objects.size());
	for (int i = 0; i < 6594; ++i)
	{
		const std::string segment = i < 3 ? "#" + std::to_string(i) : "Node_" + std::to_string(i);
		EXPECT_EQ(segment + ":mesh", objects[i].m_Identity);
		EXPECT_EQ(-1, objects[i].m_Parent);
	}
}

TEST(gltfSceneRead, RepeatedNodeIndexDoesNotMakeItsNameAmbiguous)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	const int node = AddNode(model, "Mesh");
	model.nodes[node].mesh = 0;
	AddScene(model, { node, node, -1 });
	EXPECT_EQ((std::vector<std::string>{ "Mesh:mesh", "Mesh:mesh" }),
		Identities(gltfSceneRead::ReadScene(model)));
}

TEST(gltfSceneRead, TranslationInCentimetres)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	const int a = AddNode(model, "A");
	model.nodes[a].mesh = 0;
	model.nodes[a].translation = { 1, 2, 3 };
	model.nodes[a].rotation = { 0, 0.6, 0, 0.8 };
	model.nodes[a].scale = { 2, 2, 2 };
	const int b = AddNode(model, "B");
	model.nodes[b].mesh = 0;
	model.nodes[b].scale = { 1, 2, 1 };
	AddScene(model, { a, b });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(2u, objects.size());
	ExpectVector(objects[0].m_Translation, 100, 200, 300);
	ExpectRotation(maRotation(0, 0.6f, 0, 0.8f), objects[0].m_Rotation);
	ExpectVector(objects[0].m_Scale, 2, 2, 2);
	EXPECT_TRUE(objects[0].m_bUniformScale);

	ExpectVector(objects[1].m_Translation, 0, 0, 0);
	ExpectVector(objects[1].m_Scale, 1, 2, 1);
	EXPECT_FALSE(objects[1].m_bUniformScale);
}

TEST(gltfSceneRead, MatrixDecomposed)
{
	// glTF: column-major, column vectors: M = T(1,0,0) * R(90 degrees about Y) * S(1,2,3)
	// R(90 about Y) maps x -> -z, z -> x; M's columns are R's columns scaled by S, then T
	tinygltf::Model model;
	model.meshes.resize(1);
	const int n = AddNode(model, "M");
	model.nodes[n].mesh = 0;
	model.nodes[n].matrix = {
		0, 0, -1, 0,		// column 0: R * (1,0,0) * 1
		0, 2, 0, 0,			// column 1: R * (0,1,0) * 2
		3, 0, 0, 0,			// column 2: R * (0,0,1) * 3
		1, 0, 0, 1 };		// column 3: translation
	AddScene(model, { n });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(1u, objects.size());
	ExpectVector(objects[0].m_Translation, 100, 0, 0);
	ExpectVector(objects[0].m_Scale, 1, 2, 3);
	const float s = sinf(c_Pi / 4), c = cosf(c_Pi / 4);
	ExpectRotation(maRotation(0, s, 0, c), objects[0].m_Rotation);
	EXPECT_FALSE(objects[0].m_bUniformScale);
}

TEST(gltfSceneRead, SkinJointsAreNotObjects)
{
	tinygltf::Model model;
	model.meshes.resize(2);
	const int hat = AddNode(model, "Hat");
	model.nodes[hat].mesh = 1;
	model.nodes[hat].translation = { 0, 1, 0 };
	const int hips = AddNode(model, "Hips", { hat });
	model.nodes[hips].translation = { 0, 0, 2 };
	model.nodes[hips].rotation = { 0, sinf(c_Pi / 4), 0, cosf(c_Pi / 4) };		// 90 degrees about Y
	const int body = AddNode(model, "Body");
	model.nodes[body].mesh = 0;
	model.nodes[body].skin = 0;
	const int rig = AddNode(model, "Rig", { hips, body });
	model.skins.resize(1);
	model.skins[0].joints = { hips };
	AddScene(model, { rig });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ((std::vector<std::string>{ "Rig", "Rig/Hips/Hat:mesh", "Rig/Body:mesh" }), Identities(objects));

	EXPECT_EQ(gltfSceneObject::e_Group, objects[0].m_Kind);

	const gltfSceneObject& hat_object = objects[1];
	EXPECT_EQ(gltfSceneObject::e_Mesh, hat_object.m_Kind);
	EXPECT_EQ(0, hat_object.m_Parent);
	// Hat's (0,1,0) m, turned about Y (no change), then Hips's (0,0,2) m
	ExpectVector(hat_object.m_Translation, 0, 100, 200, 1e-3f);
	ExpectRotation(maRotation(0, sinf(c_Pi / 4), 0, cosf(c_Pi / 4)), hat_object.m_Rotation);
	ExpectVector(hat_object.m_Scale, 1, 1, 1);

	// the skinned mesh sits at its skeleton root's (Hips's) parent, Rig, appended after the walk
	EXPECT_EQ(gltfSceneObject::e_SkinnedMesh, objects[2].m_Kind);
	EXPECT_EQ(0, objects[2].m_Skin);
	EXPECT_EQ(0, objects[2].m_Parent);
	ExpectIdentityTransform(objects[2]);
}

TEST(gltfSceneRead, SkinnedMeshAtSkeletonBase)
{
	// Armature (90 degrees about X, scale 0.01) -> { Hips(joint), Body(mesh 0, skin 0, translation (5,0,0)) }
	tinygltf::Model model;
	model.meshes.resize(1);
	const int hips = AddNode(model, "Hips");
	const int body = AddNode(model, "Body");
	model.nodes[body].mesh = 0;
	model.nodes[body].skin = 0;
	model.nodes[body].translation = { 5, 0, 0 };
	const int armature = AddNode(model, "Armature", { hips, body });
	model.nodes[armature].rotation = { sinf(c_Pi / 4), 0, 0, cosf(c_Pi / 4) };
	model.nodes[armature].scale = { 0.01, 0.01, 0.01 };
	model.skins.resize(1);
	model.skins[0].joints = { hips };
	AddScene(model, { armature });

	std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ((std::vector<std::string>{ "Armature", "Armature/Body:mesh" }), Identities(objects));
	EXPECT_EQ(gltfSceneObject::e_SkinnedMesh, objects[1].m_Kind);
	EXPECT_EQ(0, objects[1].m_Parent);
	ExpectIdentityTransform(objects[1]);

	// Hips a scene root instead: the skinned mesh at the top, identity
	tinygltf::Model rootModel = model;
	rootModel.nodes[armature].children = { body };
	rootModel.scenes[0].nodes = { hips, armature };
	objects = gltfSceneRead::ReadScene(rootModel);
	ASSERT_EQ(2u, objects.size());
	EXPECT_EQ(gltfSceneObject::e_SkinnedMesh, objects.back().m_Kind);
	EXPECT_EQ(-1, objects.back().m_Parent);
	ExpectIdentityTransform(objects.back());
}

TEST(gltfSceneRead, NoScene)
{
	tinygltf::Model model;
	model.meshes.resize(1);
	model.nodes[AddNode(model, "A")].mesh = 0;
	EXPECT_TRUE(gltfSceneRead::ReadScene(model).empty());

	model.nodes[AddNode(model, "B")].mesh = 0;
	AddScene(model, { 0 });
	AddScene(model, { 1 });
	ASSERT_EQ(1u, gltfSceneRead::ReadScene(model).size());
	EXPECT_EQ("A:mesh", gltfSceneRead::ReadScene(model)[0].m_Identity);

	model.defaultScene = 1;
	EXPECT_EQ("B:mesh", gltfSceneRead::ReadScene(model)[0].m_Identity);
}

TEST(gltfSceneRead, LightConversion)
{
	tinygltf::Model model;
	model.lights.resize(4);
	model.lights[0].type = "point";
	model.lights[0].color = { 1, 0.5, 0.25 };
	model.lights[0].intensity = 15;
	model.lights[0].range = 4;
	model.lights[1].type = "spot";
	model.lights[1].intensity = 20;
	model.lights[1].spot.innerConeAngle = 0.2;
	model.lights[1].spot.outerConeAngle = 0.5;
	model.lights[2].type = "directional";
	model.lights[2].intensity = 3;
	model.lights[3].type = "point";
	for (int i = 0; i < 4; i++)
		model.nodes[AddNode(model, "L" + std::to_string(i))].light = i;
	AddScene(model, { 0, 1, 2, 3 });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(4u, objects.size());

	const gltfLightDesc& point = objects[0].m_LightDesc;
	EXPECT_EQ(gltfLightDesc::e_Point, point.m_Type);
	EXPECT_FLOAT_EQ(1, point.m_Color[0]);
	EXPECT_FLOAT_EQ(0.5f, point.m_Color[1]);
	EXPECT_FLOAT_EQ(0.25f, point.m_Color[2]);
	EXPECT_FLOAT_EQ(15, point.m_Intensity);
	ExpectVector(maVector3d(point.m_Falloff.m_X, point.m_Falloff.m_Y, point.m_Falloff.m_Z), 0, 0, 1e-4f, 1e-9f);
	EXPECT_FLOAT_EQ(400, point.m_Range);

	const gltfLightDesc& spot = objects[1].m_LightDesc;
	EXPECT_EQ(gltfLightDesc::e_Spot, spot.m_Type);
	EXPECT_FLOAT_EQ(20, spot.m_Intensity);
	EXPECT_NEAR(22.918f, spot.m_Angle, 1e-3f);		// 2 * 0.2 rad in degrees
	EXPECT_NEAR(34.377f, spot.m_Penumbra, 1e-3f);	// 2 * (0.5 - 0.2) rad in degrees
	ExpectVector(maVector3d(spot.m_Falloff.m_X, spot.m_Falloff.m_Y, spot.m_Falloff.m_Z), 0, 0, 1e-4f, 1e-9f);

	const gltfLightDesc& directional = objects[2].m_LightDesc;
	EXPECT_EQ(gltfLightDesc::e_Directional, directional.m_Type);
	EXPECT_FLOAT_EQ(3, directional.m_Intensity);
	ExpectVector(maVector3d(directional.m_Falloff.m_X, directional.m_Falloff.m_Y, directional.m_Falloff.m_Z), 1, 0, 0);
	EXPECT_FLOAT_EQ(0, directional.m_Range);

	// no color: white; no range: no limit
	const gltfLightDesc& plain = objects[3].m_LightDesc;
	for (int i = 0; i < 3; i++)
		EXPECT_FLOAT_EQ(1, plain.m_Color[i]);
	EXPECT_FLOAT_EQ(0, plain.m_Range);
	EXPECT_FLOAT_EQ(1, plain.m_Intensity);
}

TEST(gltfSceneRead, CameraConversion)
{
	tinygltf::Model model;
	model.cameras.resize(3);
	model.cameras[0].type = "perspective";
	model.cameras[0].perspective.yfov = 0.8;
	model.cameras[0].perspective.aspectRatio = 1.5;
	model.cameras[0].perspective.znear = 0.1;
	model.cameras[0].perspective.zfar = 100;
	model.cameras[1].type = "perspective";
	model.cameras[1].perspective.yfov = 0.8;
	model.cameras[1].perspective.znear = 0.1;
	model.cameras[2].type = "orthographic";
	model.cameras[2].orthographic.xmag = 2;
	model.cameras[2].orthographic.ymag = 1;
	model.cameras[2].orthographic.znear = 0.01;
	model.cameras[2].orthographic.zfar = 50;
	for (int i = 0; i < 3; i++)
		model.nodes[AddNode(model, "C" + std::to_string(i))].camera = i;
	AddScene(model, { 0, 1, 2 });

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	ASSERT_EQ(3u, objects.size());

	const gltfCameraDesc& persp = objects[0].m_CameraDesc;
	EXPECT_FALSE(persp.m_bOrthographic);
	EXPECT_NEAR(45.837f, persp.m_YFov, 1e-3f);
	EXPECT_FLOAT_EQ(1.5f, persp.m_Aspect);
	EXPECT_FLOAT_EQ(10, persp.m_Near);
	EXPECT_FLOAT_EQ(10000, persp.m_Far);

	const gltfCameraDesc& open = objects[1].m_CameraDesc;
	EXPECT_FLOAT_EQ(0, open.m_Far);
	EXPECT_FLOAT_EQ(0, open.m_Aspect);

	const gltfCameraDesc& ortho = objects[2].m_CameraDesc;
	EXPECT_TRUE(ortho.m_bOrthographic);
	EXPECT_FLOAT_EQ(200, ortho.m_XMag);
	EXPECT_FLOAT_EQ(100, ortho.m_YMag);
	EXPECT_FLOAT_EQ(1, ortho.m_Near);
	EXPECT_FLOAT_EQ(5000, ortho.m_Far);
}
