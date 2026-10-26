/*****************************************************************************
**  gltfSceneImportTest.cpp
**
**      Tests of gltfSceneImport: glTF objects turned into MachStudio's
**      (iscnSceneDesc): kinds, world poses, light and camera values, and
**      the format registry reading a file through it.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SCENEIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfSceneImport.hpp"
#endif
#ifndef GLTF_PLACEMENT_HPP
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#include "ietTempDir.hpp"

#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

namespace
{
	const float c_Deg = 3.14159265358979f / 180;

	void ExpectPoint(const maPoint3d& i_Expected, const maPoint3d& i_Actual, float i_Tolerance = 1e-4f)
	{
		EXPECT_NEAR(i_Expected.m_X, i_Actual.m_X, i_Tolerance);
		EXPECT_NEAR(i_Expected.m_Y, i_Actual.m_Y, i_Tolerance);
		EXPECT_NEAR(i_Expected.m_Z, i_Actual.m_Z, i_Tolerance);
	}

	maVector3d Rotated(const maRotation& i_Rotation, maVector3d i_V)
	{
		i_Rotation.RotateVector(i_V);
		return i_V;
	}

	gltfSceneObject Object(gltfSceneObject::Kind i_Kind, const char* i_Name, int i_Parent = -1)
	{
		gltfSceneObject o;
		o.m_Kind = i_Kind;
		o.m_Name = i_Name;
		o.m_Identity = std::string(i_Name) + "-id";
		o.m_Parent = i_Parent;
		return o;
	}
}

TEST(gltfSceneImport, Convert)
{
	// a group at (100,0,0) turned 90 degrees about Y, scale 2, holding a mesh at (10,0,0),
	// a skinned mesh, a point, spot and directional light, and two cameras
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Group, "Group"));
	objects[0].m_Translation = maVector3d(100, 0, 0);
	objects[0].m_Rotation = maRotation(maVector3d(0, 1, 0), 90 * c_Deg);
	objects[0].m_Scale = maVector3d(2, 2, 2);
	objects.push_back(Object(gltfSceneObject::e_Mesh, "Mesh", 0));
	objects[1].m_Translation = maVector3d(10, 0, 0);
	objects.push_back(Object(gltfSceneObject::e_SkinnedMesh, "Skinned", 0));
	objects.push_back(Object(gltfSceneObject::e_Light, "Point"));
	objects[3].m_LightDesc.m_Type = gltfLightDesc::e_Point;
	objects[3].m_LightDesc.m_Color[1] = 0.5f;
	objects[3].m_LightDesc.m_Intensity = 15;
	objects[3].m_LightDesc.m_Falloff = maPoint3d(0, 0, gltfSceneRead::c_QuadraticFalloff);
	objects.push_back(Object(gltfSceneObject::e_Light, "Spot"));
	objects[4].m_LightDesc.m_Type = gltfLightDesc::e_Spot;
	objects[4].m_LightDesc.m_Range = 400;
	objects[4].m_LightDesc.m_Angle = 30;
	objects[4].m_LightDesc.m_Penumbra = 10;
	objects[4].m_Translation = maVector3d(0, 50, 0);
	objects.push_back(Object(gltfSceneObject::e_Light, "Sun"));
	objects[5].m_LightDesc.m_Type = gltfLightDesc::e_Directional;
	objects[5].m_Rotation = maRotation(maVector3d(1, 0, 0), -45 * c_Deg);
	objects.push_back(Object(gltfSceneObject::e_Camera, "Cam"));
	objects[6].m_CameraDesc.m_YFov = 0.8f / c_Deg;
	objects[6].m_CameraDesc.m_Near = 10;
	objects[6].m_Translation = maVector3d(0, 0, 500);
	objects.push_back(Object(gltfSceneObject::e_Camera, "Ortho"));
	objects[7].m_CameraDesc.m_bOrthographic = true;
	objects[7].m_CameraDesc.m_XMag = 50;
	objects[7].m_CameraDesc.m_Far = 1000;

	iscnSceneSettings settings;
	settings.m_RenderAspect = 1.5f;
	iscnSceneDesc scene;
	gltfSceneImport::Convert(objects, "test.gltf", settings, scene);
	ASSERT_EQ(8u, scene.m_Objects.size());
	const std::vector<iscnObjectDesc>& o = scene.m_Objects;

	EXPECT_EQ(iscnObjectDesc::e_Group, o[0].m_Kind);
	EXPECT_EQ("Group", o[0].m_Name);
	EXPECT_EQ("Group-id", o[0].m_Identity);
	EXPECT_NEAR(2, o[0].m_Scale, 1e-5f);

	// world pose: (10,0,0) scaled by 2 and turned about Y is (0,0,-20)
	EXPECT_EQ(iscnObjectDesc::e_Mesh, o[1].m_Kind);
	EXPECT_EQ("Mesh-id", o[1].m_Identity);
	EXPECT_EQ(0, o[1].m_Parent);
	ExpectPoint(maPoint3d(100, 0, -20), o[1].m_Position);
	EXPECT_NEAR(2, o[1].m_Scale, 1e-5f);

	// skinned meshes come in as meshes until skins are imported
	EXPECT_EQ(iscnObjectDesc::e_Mesh, o[2].m_Kind);

	EXPECT_EQ(iscnObjectDesc::e_PointLight, o[3].m_Kind);
	EXPECT_FLOAT_EQ(0.5f, o[3].m_Light.m_Color.GetGreen());
	EXPECT_FLOAT_EQ(15, o[3].m_Light.m_Intensity);
	EXPECT_FLOAT_EQ(gltfSceneRead::c_QuadraticFalloff, o[3].m_Light.m_Falloff.m_Z);
	EXPECT_NEAR(gltfPlacement::LightRange(objects[3].m_LightDesc), o[3].m_Light.m_Range, 1e-2f);

	// a spot light aims along its -Z
	EXPECT_EQ(iscnObjectDesc::e_SpotLight, o[4].m_Kind);
	EXPECT_FLOAT_EQ(400, o[4].m_Light.m_Range);
	EXPECT_FLOAT_EQ(30, o[4].m_Light.m_Angle);
	EXPECT_FLOAT_EQ(10, o[4].m_Light.m_Penumbra);
	ExpectPoint(maPoint3d(0, 50, -gltfPlacement::c_TargetDistance), o[4].m_Light.m_Target);

	// a directional light's orientation turns +Z to its direction
	EXPECT_EQ(iscnObjectDesc::e_DirectionalLight, o[5].m_Kind);
	const maVector3d sun = gltfPlacement::Forward(objects[5].m_Rotation);
	const maVector3d turned = Rotated(o[5].m_Orientation, maVector3d(0, 0, 1));
	EXPECT_NEAR(sun.m_X, turned.m_X, 1e-5f);
	EXPECT_NEAR(sun.m_Y, turned.m_Y, 1e-5f);
	EXPECT_NEAR(sun.m_Z, turned.m_Z, 1e-5f);

	// perspective: horizontal FOV from the render aspect (the file gives none), far left to the default
	EXPECT_EQ(iscnObjectDesc::e_Camera, o[6].m_Kind);
	EXPECT_FALSE(o[6].m_Camera.m_bOrthographic);
	EXPECT_NEAR(64.765f, o[6].m_Camera.m_FOV, 1e-3f);
	EXPECT_FLOAT_EQ(10, o[6].m_Camera.m_Near);
	EXPECT_EQ(0, o[6].m_Camera.m_Far);
	ExpectPoint(maPoint3d(0, 0, 500 - gltfPlacement::c_TargetDistance), o[6].m_Camera.m_Target);
	EXPECT_NEAR(0, o[6].m_Camera.m_Tilt, 1e-3f);

	EXPECT_TRUE(o[7].m_Camera.m_bOrthographic);
	EXPECT_FLOAT_EQ(100, o[7].m_Camera.m_OrthoWidth);
	EXPECT_FLOAT_EQ(1000, o[7].m_Camera.m_Far);
}

TEST(gltfSceneImport, ReadsThroughTheRegistry)
{
	ietTempDir dir;
	std::ofstream(dir.GetPath() / "scene.gltf", std::ios::binary) <<
		"{ \"asset\": { \"version\": \"2.0\" }, \"scenes\": [ { \"nodes\": [ 0 ] } ],"
		" \"nodes\": [ { \"name\": \"A\", \"mesh\": 0, \"translation\": [ 1, 0, 0 ] } ],"
		" \"meshes\": [ { \"primitives\": [] } ] }";

	gltfSceneImport* pImport = new gltfSceneImport();
	iscnSceneImport::AddImplementation(pImport);
	EXPECT_TRUE(iscnSceneImport::IsRecognized(dir.GetLocator("scene.gltf")));
	EXPECT_TRUE(iscnSceneImport::IsRecognized(dir.GetLocator("SCENE.GLB")));
	EXPECT_FALSE(iscnSceneImport::IsRecognized(dir.GetLocator("scene.mab")));
	EXPECT_EQ("*.gltf;*.glb", iscnSceneImport::GetFilePatterns());

	iscnSceneDesc scene;
	ASSERT_TRUE(iscnSceneImport::Read(dir.GetLocator("scene.gltf"), iscnSceneSettings(), scene));
	ASSERT_EQ(1u, scene.m_Objects.size());
	EXPECT_EQ("A:mesh", scene.m_Objects[0].m_Identity);
	ExpectPoint(maPoint3d(100, 0, 0), scene.m_Objects[0].m_Position);

	EXPECT_FALSE(iscnSceneImport::Read(dir.GetLocator("missing.gltf"), iscnSceneSettings(), scene));
	iscnSceneImport::RemoveImplementation(pImport);
	EXPECT_FALSE(iscnSceneImport::IsRecognized(dir.GetLocator("scene.gltf")));
}

TEST(gltfSceneImport, ClipsCopied)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Group, "Group"));
	objects.push_back(Object(gltfSceneObject::e_SkinnedMesh, "Body", 0));
	gltfClipRef walk, unnamed;
	walk.m_Identity = "Walk";
	walk.m_Name = "Walk";
	unnamed.m_Identity = "#1";
	unnamed.m_Name = "Animation 1";
	objects[1].m_Clips = { walk, unnamed };

	iscnSceneDesc scene;
	gltfSceneImport::Convert(objects, "test.gltf", iscnSceneSettings(), scene);
	ASSERT_EQ(2u, scene.m_Objects.size());
	EXPECT_TRUE(scene.m_Objects[0].m_Clips.empty());
	const std::vector<iscnClipDesc>& clips = scene.m_Objects[1].m_Clips;
	ASSERT_EQ(2u, clips.size());
	EXPECT_EQ("Walk", clips[0].m_Identity);
	EXPECT_EQ("Walk", clips[0].m_Name);
	EXPECT_EQ("#1", clips[1].m_Identity);
	EXPECT_EQ("Animation 1", clips[1].m_Name);
}
