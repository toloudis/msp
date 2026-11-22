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

#include <cmath>
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

namespace
{
	gltfTrack KeyTrack(gltfTrack::Property i_Property, int i_Components, std::vector<float> i_Times,
					   std::vector<float> i_Values)
	{
		gltfTrack track;
		track.m_Property = i_Property;
		track.m_Components = i_Components;
		track.m_Times = i_Times;
		track.m_Values = i_Values;
		return track;
	}

	std::vector<float> Quat(const maRotation& i_Rotation)
	{
		return { i_Rotation.GetX(), i_Rotation.GetY(), i_Rotation.GetZ(), i_Rotation.GetW() };
	}

	const iscnKeyTrack* FindKeys(const iscnObjectDesc& i_Object, iscnKeyTrack::Channel i_Channel)
	{
		for (const iscnKeyTrack& track : i_Object.m_Keys)
		{
			if (track.m_Channel == i_Channel)
				return &track;
		}
		return NULL;
	}

	iscnSceneDesc ConvertObjects(const std::vector<gltfSceneObject>& i_Objects, float i_RenderAspect = 1)
	{
		iscnSceneSettings settings;
		settings.m_RenderAspect = i_RenderAspect;
		iscnSceneDesc scene;
		gltfSceneImport::Convert(i_Objects, "test.gltf", settings, scene);
		return scene;
	}

	void ExpectRotation(const maRotation& i_Expected, const float* i_pActual)
	{
		// q and -q are the same rotation
		const float dot = i_Expected.GetX() * i_pActual[0] + i_Expected.GetY() * i_pActual[1]
						+ i_Expected.GetZ() * i_pActual[2] + i_Expected.GetW() * i_pActual[3];
		EXPECT_NEAR(1, fabsf(dot), 1e-5f);
	}
}

TEST(gltfSceneImport, MeshKeysLocal)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Group, "Group"));
	objects[0].m_Translation = maVector3d(100, 0, 0);
	objects.push_back(Object(gltfSceneObject::e_Mesh, "Mesh", 0));
	const maRotation turned(maVector3d(0, 1, 0), 30 * c_Deg);
	objects[1].m_Tracks.push_back(KeyTrack(gltfTrack::e_Translation, 3, { 0, 1 }, { 0, 0, 0,  10, 0, 0 }));
	objects[1].m_Tracks.push_back(KeyTrack(gltfTrack::e_Rotation, 4, { 0.5f }, Quat(turned)));
	objects[1].m_Tracks[1].m_bStep = true;

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnObjectDesc& mesh = scene.m_Objects[1];
	const iscnKeyTrack* position = FindKeys(mesh, iscnKeyTrack::e_Position);
	ASSERT_TRUE(position);
	EXPECT_EQ((std::vector<float>{ 0, 1 }), position->m_Times);
	EXPECT_EQ((std::vector<float>{ 0, 0, 0,  10, 0, 0 }), position->m_Values);	// the parent's space, not world
	EXPECT_FALSE(position->m_bStep);
	const iscnKeyTrack* orientation = FindKeys(mesh, iscnKeyTrack::e_Orientation);
	ASSERT_TRUE(orientation);
	EXPECT_TRUE(orientation->m_bStep);
	ASSERT_EQ(4u, orientation->m_Values.size());
	ExpectRotation(turned, &orientation->m_Values[0]);
	EXPECT_TRUE(scene.m_Objects[0].m_Keys.empty());
}

TEST(gltfSceneImport, GroupScaleMean)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Group, "Group"));
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_Scale, 3, { 0 }, { 1, 2, 4 }));

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnKeyTrack* scale = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Scale);
	ASSERT_TRUE(scale);
	ASSERT_EQ(1u, scale->m_Values.size());
	EXPECT_NEAR(2, scale->m_Values[0], 1e-5f);
}

TEST(gltfSceneImport, SpotTarget)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Light, "Spot"));
	objects[0].m_LightDesc.m_Type = gltfLightDesc::e_Spot;
	objects[0].m_Translation = maVector3d(0, 50, 0);
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_Translation, 3, { 0, 1 }, { 0, 0, 0,  100, 0, 0 }));
	const maRotation turned(maVector3d(0, 1, 0), 90 * c_Deg);
	std::vector<float> rotations = Quat(maRotation());
	const std::vector<float> end = Quat(turned);
	rotations.insert(rotations.end(), end.begin(), end.end());
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_Rotation, 4, { 0, 0.5f }, rotations));

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnKeyTrack* target = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Target);
	ASSERT_TRUE(target);
	EXPECT_EQ((std::vector<float>{ 0, 0.5f, 1 }), target->m_Times);
	ASSERT_EQ(9u, target->m_Values.size());
	const maVector3d forward = gltfPlacement::Forward(turned) * gltfPlacement::c_TargetDistance;
	ExpectPoint(maPoint3d(0, 0, -gltfPlacement::c_TargetDistance), maPoint3d(target->m_Values[0], target->m_Values[1], target->m_Values[2]));
	ExpectPoint(maPoint3d(50 + forward.m_X, forward.m_Y, forward.m_Z), maPoint3d(target->m_Values[3], target->m_Values[4], target->m_Values[5]));
	ExpectPoint(maPoint3d(100 + forward.m_X, forward.m_Y, forward.m_Z), maPoint3d(target->m_Values[6], target->m_Values[7], target->m_Values[8]));
	EXPECT_TRUE(FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Position));
	EXPECT_FALSE(FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Orientation));
}

TEST(gltfSceneImport, DirectionalFollowsAncestors)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Group, "Group"));
	const maRotation start, end(maVector3d(0, 1, 0), 90 * c_Deg);
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_Rotation, 4, { 0, 1 }, { 0, 0, 0, 1,  end.GetX(), end.GetY(), end.GetZ(), end.GetW() }));
	objects.push_back(Object(gltfSceneObject::e_Light, "Sun", 0));
	objects[1].m_LightDesc.m_Type = gltfLightDesc::e_Directional;
	const maRotation own(maVector3d(1, 0, 0), -45 * c_Deg);
	objects[1].m_Rotation = own;

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnKeyTrack* orientation = FindKeys(scene.m_Objects[1], iscnKeyTrack::e_Orientation);
	ASSERT_TRUE(orientation);
	EXPECT_EQ((std::vector<float>{ 0, 1 }), orientation->m_Times);
	ASSERT_EQ(8u, orientation->m_Values.size());
	ExpectRotation(gltfPlacement::DirectionalOrientation(start * own), &orientation->m_Values[0]);
	ExpectRotation(gltfPlacement::DirectionalOrientation(end * own), &orientation->m_Values[4]);
}

TEST(gltfSceneImport, CameraTiltAndFov)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Camera, "Cam"));
	objects[0].m_CameraDesc.m_Aspect = 2;
	const maRotation rolled(maVector3d(0, 0, 1), 30 * c_Deg);
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_Rotation, 4, { 0, 1 }, { 0, 0, 0, 1,  rolled.GetX(), rolled.GetY(), rolled.GetZ(), rolled.GetW() }));
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_CameraYFov, 1, { 0, 1 }, { 0.5f, 1 }));

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnKeyTrack* tilt = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Tilt);
	ASSERT_TRUE(tilt);
	ASSERT_EQ(2u, tilt->m_Values.size());
	EXPECT_NEAR(gltfPlacement::CameraTilt(maRotation()), tilt->m_Values[0], 1e-3f);
	EXPECT_NEAR(gltfPlacement::CameraTilt(rolled), tilt->m_Values[1], 1e-3f);
	EXPECT_GT(fabsf(tilt->m_Values[1] - tilt->m_Values[0]), 29.0f);

	const iscnKeyTrack* fov = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_FOV);
	ASSERT_TRUE(fov);
	gltfCameraDesc desc = objects[0].m_CameraDesc;
	desc.m_YFov = 0.5f / c_Deg;
	EXPECT_NEAR(gltfPlacement::CameraHorizontalFov(desc, 1), fov->m_Values[0], 1e-3f);
	desc.m_YFov = 1 / c_Deg;
	EXPECT_NEAR(gltfPlacement::CameraHorizontalFov(desc, 1), fov->m_Values[1], 1e-3f);
}

TEST(gltfSceneImport, SpotCones)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Light, "Spot"));
	objects[0].m_LightDesc.m_Type = gltfLightDesc::e_Spot;
	objects[0].m_LightDesc.m_Angle = 2 * 0.2f / c_Deg;
	objects[0].m_LightDesc.m_Penumbra = 2 * (0.6f - 0.2f) / c_Deg;
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_SpotInnerCone, 1, { 0, 1 }, { 0.2f, 0.4f }));

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnKeyTrack* angle = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Angle);
	const iscnKeyTrack* penumbra = FindKeys(scene.m_Objects[0], iscnKeyTrack::e_Penumbra);
	ASSERT_TRUE(angle);
	ASSERT_TRUE(penumbra);
	EXPECT_NEAR(2 * 0.2f / c_Deg, angle->m_Values[0], 1e-3f);
	EXPECT_NEAR(2 * 0.4f / c_Deg, angle->m_Values[1], 1e-3f);
	EXPECT_NEAR(2 * (0.6f - 0.2f) / c_Deg, penumbra->m_Values[0], 1e-3f);
	EXPECT_NEAR(2 * (0.6f - 0.4f) / c_Deg, penumbra->m_Values[1], 1e-3f);
}

TEST(gltfSceneImport, LightValues)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Light, "Point"));
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_LightColor, 3, { 0 }, { 1, 0.5f, 0 }));
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_LightIntensity, 1, { 0 }, { 7 }));
	objects[0].m_Tracks.push_back(KeyTrack(gltfTrack::e_LightRange, 1, { 0, 1 }, { 300, 0 }));

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnObjectDesc& light = scene.m_Objects[0];
	const iscnKeyTrack* color = FindKeys(light, iscnKeyTrack::e_Color);
	ASSERT_TRUE(color);
	EXPECT_EQ((std::vector<float>{ 1, 0.5f, 0, 1 }), color->m_Values);
	ASSERT_TRUE(FindKeys(light, iscnKeyTrack::e_Intensity));
	EXPECT_EQ((std::vector<float>{ 7 }), FindKeys(light, iscnKeyTrack::e_Intensity)->m_Values);
	const iscnKeyTrack* range = FindKeys(light, iscnKeyTrack::e_Range);
	ASSERT_TRUE(range);
	EXPECT_FLOAT_EQ(300, range->m_Values[0]);
	gltfLightDesc unlimited = objects[0].m_LightDesc;
	unlimited.m_Range = 0;
	EXPECT_NEAR(gltfPlacement::LightRange(unlimited), range->m_Values[1], 1e-2f);
}

TEST(gltfSceneImport, MaterialAndClipCopied)
{
	std::vector<gltfSceneObject> objects;
	objects.push_back(Object(gltfSceneObject::e_Mesh, "Mesh"));
	gltfTrack emissive = KeyTrack(gltfTrack::e_MaterialParam, 3, { 0 }, { 1, 0.5f, 0.25f });
	emissive.m_Material = "Paint";
	emissive.m_Param = "g_emissive";
	objects[0].m_Tracks.push_back(emissive);
	gltfTrack normal = KeyTrack(gltfTrack::e_MaterialNormalScale, 1, { 0 }, { 2 });
	normal.m_Material = "Paint";
	objects[0].m_Tracks.push_back(normal);
	gltfClipRef clip;
	clip.m_Identity = "Walk";
	clip.m_Begin = 1.5f;
	objects[0].m_Clips.push_back(clip);

	const iscnSceneDesc scene = ConvertObjects(objects);
	const iscnObjectDesc& mesh = scene.m_Objects[0];
	const iscnKeyTrack* param = FindKeys(mesh, iscnKeyTrack::e_MaterialParam);
	ASSERT_TRUE(param);
	EXPECT_EQ("Paint", param->m_Material);
	EXPECT_EQ("g_emissive", param->m_Param);
	EXPECT_EQ((std::vector<float>{ 1, 0.5f, 0.25f, 1 }), param->m_Values);
	const iscnKeyTrack* scale = FindKeys(mesh, iscnKeyTrack::e_MaterialNormalScale);
	ASSERT_TRUE(scale);
	EXPECT_EQ("Paint", scale->m_Material);
	ASSERT_EQ(1u, mesh.m_Clips.size());
	EXPECT_FLOAT_EQ(1.5f, mesh.m_Clips[0].m_Begin);
}
