/*****************************************************************************
**  gltfPlacementTest.cpp
**
**      Tests of gltfPlacement: the values MachStudio's lights, cameras and
**      characters need that glTF doesn't give directly (look directions,
**      directional light orientation, camera tilt and horizontal FOV, light
**      range, uniform scale), and finding a mesh object by identity.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_PLACEMENT_HPP
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
#endif
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
	const float c_Deg = c_Pi / 180;

	void ExpectVector(const maVector3d& i_Expected, const maVector3d& i_Actual, float i_Tolerance)
	{
		EXPECT_NEAR(i_Expected.m_X, i_Actual.m_X, i_Tolerance);
		EXPECT_NEAR(i_Expected.m_Y, i_Actual.m_Y, i_Tolerance);
		EXPECT_NEAR(i_Expected.m_Z, i_Actual.m_Z, i_Tolerance);
	}

	maRotation About(float i_X, float i_Y, float i_Z, float i_Degrees)
	{
		return maRotation(maVector3d(i_X, i_Y, i_Z), i_Degrees * c_Deg);
	}

	maVector3d Rotated(const maRotation& i_Rotation, maVector3d i_V)
	{
		i_Rotation.RotateVector(i_V);
		return i_V;
	}

	// the up vector cmraCameraObject builds from a view direction and tilt (degrees),
	// as LookAt keeps it: its part across the view, normalized
	maVector3d CameraUp(const maVector3d& i_View, float i_Tilt)
	{
		maVector3d up = Rotated(maRotation(i_View, i_Tilt * c_Deg), maVector3d(0, 1, 0));
		maVector3d view = i_View;
		view.Normalize();
		up = up - view * (up * view);
		up.Normalize();
		return up;
	}
}

TEST(gltfPlacement, Forward)
{
	ExpectVector(maVector3d(0, 0, -1), gltfPlacement::Forward(maRotation()), 1e-5f);
	ExpectVector(maVector3d(-1, 0, 0), gltfPlacement::Forward(About(0, 1, 0, 90)), 1e-5f);
	ExpectVector(maVector3d(0, 1, 0), gltfPlacement::Forward(About(1, 0, 0, 90)), 1e-5f);
}

TEST(gltfPlacement, DirectionalOrientation)
{
	const maRotation rotations[] = {
		maRotation(), About(0, 1, 0, 90), About(0, 1, 0, 45) * About(1, 0, 0, 30), About(0, 1, 0, 180) };
	for (const maRotation& r : rotations)
		ExpectVector(gltfPlacement::Forward(r),
					 Rotated(gltfPlacement::DirectionalOrientation(r), maVector3d(0, 0, 1)), 1e-5f);
}

TEST(gltfPlacement, CameraTilt)
{
	EXPECT_NEAR(0, gltfPlacement::CameraTilt(maRotation()), 1e-3f);

	// rolled 30 degrees about the view axis
	const maRotation roll = About(0, 0, 1, 30);
	const float tilt = gltfPlacement::CameraTilt(roll);
	EXPECT_NEAR(30, fabsf(tilt), 1e-3f);
	ExpectVector(Rotated(roll, maVector3d(0, 1, 0)), CameraUp(gltfPlacement::Forward(roll), tilt), 1e-4f);

	// a roll on a camera that also looks sideways and down
	const maRotation turned = About(0, 1, 0, 60) * About(1, 0, 0, -25) * About(0, 0, 1, -40);
	const float turned_tilt = gltfPlacement::CameraTilt(turned);
	EXPECT_NEAR(40, fabsf(turned_tilt), 1e-2f);
	ExpectVector(Rotated(turned, maVector3d(0, 1, 0)), CameraUp(gltfPlacement::Forward(turned), turned_tilt), 1e-4f);

	// pitch only: no roll
	EXPECT_NEAR(0, gltfPlacement::CameraTilt(About(1, 0, 0, 20)), 1e-3f);

	// straight down: no roll defined against world up
	const float down = gltfPlacement::CameraTilt(About(1, 0, 0, -90));
	EXPECT_FALSE(std::isnan(down));
	EXPECT_EQ(0, down);
}

TEST(gltfPlacement, CameraHorizontalFov)
{
	gltfCameraDesc camera;
	camera.m_YFov = 0.8f / c_Deg;	// 45.8366 degrees
	camera.m_Aspect = 1.5f;
	EXPECT_NEAR(64.765f, gltfPlacement::CameraHorizontalFov(camera, 1), 1e-3f);

	camera.m_Aspect = 0;
	EXPECT_NEAR(64.765f, gltfPlacement::CameraHorizontalFov(camera, 1.5f), 1e-3f);

	camera.m_Aspect = 1;
	EXPECT_NEAR(45.837f, gltfPlacement::CameraHorizontalFov(camera, 1.5f), 1e-3f);
}

TEST(gltfPlacement, LightRange)
{
	gltfLightDesc light;
	light.m_Type = gltfLightDesc::e_Point;
	light.m_Range = 400;
	EXPECT_EQ(400, gltfPlacement::LightRange(light));

	light.m_Range = 0;
	light.m_Intensity = 15;
	EXPECT_NEAR(6196.8f, gltfPlacement::LightRange(light), 0.1f);

	light.m_Intensity = 1e-9f;
	EXPECT_EQ(gltfPlacement::c_TargetDistance, gltfPlacement::LightRange(light));

	light.m_Type = gltfLightDesc::e_Directional;
	light.m_Intensity = 15;
	EXPECT_EQ(0, gltfPlacement::LightRange(light));
}

TEST(gltfPlacement, UniformScale)
{
	EXPECT_NEAR(2, gltfPlacement::UniformScale(maVector3d(2, 2, 2)), 1e-5f);
	EXPECT_NEAR(2, gltfPlacement::UniformScale(maVector3d(1, 2, 4)), 1e-5f);
	EXPECT_NEAR(1, gltfPlacement::UniformScale(maVector3d(-1, 1, 1)), 1e-5f);
}

TEST(gltfPlacement, WorldTransforms)
{
	// group (100,0,0), 90 degrees about Y, scale 2; under it a mesh at (10,0,0) turned
	// 90 degrees about X, a mesh with non-uniform scale, and a group (0,5,0) holding a light
	std::vector<gltfSceneObject> objects(5);
	objects[0].m_Kind = gltfSceneObject::e_Group;
	objects[0].m_Translation = maVector3d(100, 0, 0);
	objects[0].m_Rotation = About(0, 1, 0, 90);
	objects[0].m_Scale = maVector3d(2, 2, 2);
	objects[1].m_Kind = gltfSceneObject::e_Mesh;
	objects[1].m_Parent = 0;
	objects[1].m_Translation = maVector3d(10, 0, 0);
	objects[1].m_Rotation = About(1, 0, 0, 90);
	objects[1].m_Scale = maVector3d(3, 3, 3);
	objects[2].m_Kind = gltfSceneObject::e_Mesh;
	objects[2].m_Parent = 0;
	objects[2].m_Scale = maVector3d(1, 2, 4);
	objects[2].m_bUniformScale = false;
	objects[3].m_Kind = gltfSceneObject::e_Group;
	objects[3].m_Parent = 0;
	objects[3].m_Translation = maVector3d(0, 5, 0);
	objects[4].m_Kind = gltfSceneObject::e_Light;
	objects[4].m_Parent = 3;
	objects[4].m_Translation = maVector3d(0, 0, -1);

	const std::vector<gltfPlacement::WorldTransform> world = gltfPlacement::WorldTransforms(objects);
	ASSERT_EQ(5u, world.size());

	ExpectVector(maVector3d(100, 0, 0), world[0].m_Translation, 1e-4f);
	EXPECT_NEAR(2, world[0].m_Scale, 1e-5f);

	// (10,0,0) scaled by 2 and turned 90 degrees about Y: (0,0,-20)
	ExpectVector(maVector3d(100, 0, -20), world[1].m_Translation, 1e-4f);
	EXPECT_NEAR(6, world[1].m_Scale, 1e-5f);
	// the child's turn, then the parent's: local +Y goes to +Z, then to +X
	ExpectVector(maVector3d(1, 0, 0), Rotated(world[1].m_Rotation, maVector3d(0, 1, 0)), 1e-5f);

	// a non-uniform scale is baked into the mesh's template, so only the parent's applies
	EXPECT_NEAR(2, world[2].m_Scale, 1e-5f);

	// (0,0,-1) in the inner group is (0,0,-2) turned about Y, (-2,0,0), plus (0,10,0)
	ExpectVector(maVector3d(98, 10, 0), world[4].m_Translation, 1e-4f);
	ExpectVector(gltfPlacement::Forward(About(0, 1, 0, 90)), gltfPlacement::Forward(world[4].m_Rotation), 1e-5f);
}

TEST(gltfPlacement, FindMeshObject)
{
	// "Car" (node 0) holds two nodes named "Wheel" that share mesh 0
	tinygltf::Model model;
	model.meshes.resize(1);
	for (const char* name : { "Car", "Wheel", "Wheel" })
	{
		tinygltf::Node node;
		node.name = name;
		model.nodes.push_back(node);
	}
	model.nodes[0].children = { 1, 2 };
	model.nodes[1].mesh = 0;
	model.nodes[2].mesh = 0;
	tinygltf::Scene scene;
	scene.nodes = { 0 };
	model.scenes.push_back(scene);

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(model);
	const gltfSceneObject* first = gltfPlacement::FindMeshObject(objects, "Car/#1:mesh");
	const gltfSceneObject* second = gltfPlacement::FindMeshObject(objects, "Car/#2:mesh");
	ASSERT_TRUE(first);
	ASSERT_TRUE(second);
	EXPECT_EQ(1, first->m_Node);
	EXPECT_EQ(2, second->m_Node);

	EXPECT_EQ(nullptr, gltfPlacement::FindMeshObject(objects, "Car"));
	EXPECT_EQ(nullptr, gltfPlacement::FindMeshObject(objects, "Nope"));
}
