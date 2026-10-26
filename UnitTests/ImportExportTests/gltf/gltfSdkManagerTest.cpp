/*****************************************************************************
**  gltfSdkManagerTest.cpp
**
**      Tests of gltfSdkManager::LoadSceneShared's one-entry parse cache and
**      gltfSceneFile::ReadObjects, on small glTF files written as JSON text.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SDKMANAGER_HPP
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"
#endif
#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include "ietTempDir.hpp"

#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

namespace
{
	// "A" (mesh 0) and "B" (camera 0), and a third root "C" (mesh 0) when asked
	std::string SceneJson(bool i_bThirdNode)
	{
		std::string roots = i_bThirdNode ? "0, 1, 2" : "0, 1";
		std::string third = i_bThirdNode ? ", { \"name\": \"C\", \"mesh\": 0 }" : "";
		return "{ \"asset\": { \"version\": \"2.0\" }, \"scene\": 0,"
			   " \"scenes\": [ { \"nodes\": [ " + roots + " ] } ],"
			   " \"nodes\": [ { \"name\": \"A\", \"mesh\": 0 }, { \"name\": \"B\", \"camera\": 0 }" + third + " ],"
			   " \"meshes\": [ { \"primitives\": [] } ],"
			   " \"cameras\": [ { \"type\": \"perspective\", \"perspective\": { \"yfov\": 0.8, \"znear\": 0.1 } } ] }";
	}

	std::string WriteScene(const ietTempDir& i_Dir, bool i_bThirdNode)
	{
		std::filesystem::path path = i_Dir.GetPath() / "scene.gltf";
		std::ofstream(path, std::ios::binary | std::ios::trunc) << SceneJson(i_bThirdNode);
		return path.u8string();
	}
}

TEST(gltfSdkManager, CacheReturnsSameModel)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	std::shared_ptr<const tinygltf::Model> first = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(first);
	EXPECT_EQ(2u, first->nodes.size());
	EXPECT_EQ(first, gltfSdkManager::LoadSceneShared(file.c_str()));
}

TEST(gltfSdkManager, CacheReloadsChangedFile)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	std::shared_ptr<const tinygltf::Model> first = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(first);

	WriteScene(dir, true);
	std::shared_ptr<const tinygltf::Model> second = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(second);
	EXPECT_NE(first, second);
	EXPECT_EQ(3u, second->nodes.size());
}

TEST(gltfSdkManager, MissingFile)
{
	ietTempDir dir;
	const std::string file = (dir.GetPath() / "missing.gltf").u8string();
	EXPECT_FALSE(gltfSdkManager::LoadSceneShared(file.c_str()));
}

TEST(gltfSdkManager, ReusesDerivedObjectsForEveryMeshLoad)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	const auto model = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(model);
	const auto objects = gltfSdkManager::ReadSceneObjectsShared(model);
	ASSERT_EQ(2u, objects->size());
	EXPECT_EQ("A:mesh", objects->front().m_Identity);
	for (int i = 0; i < 6594; ++i)
		EXPECT_EQ(objects, gltfSdkManager::ReadSceneObjectsShared(model));
}

TEST(gltfSdkManager, DerivedObjectsFollowReloadedModel)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	const auto first_model = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(first_model);
	const auto first = gltfSdkManager::ReadSceneObjectsShared(first_model);
	WriteScene(dir, true);
	const auto second_model = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(second_model);
	const auto second = gltfSdkManager::ReadSceneObjectsShared(second_model);
	EXPECT_NE(first, second);
	EXPECT_EQ(2u, first->size());
	ASSERT_EQ(3u, second->size());
	EXPECT_EQ("C:mesh", second->back().m_Identity);
	EXPECT_EQ(second, gltfSdkManager::ReadSceneObjectsShared(second_model));
}

TEST(gltfSdkManager, CleanUpReleasesDerivedObjects)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	std::weak_ptr<const tinygltf::Model> model_ref;
	std::weak_ptr<const std::vector<gltfSceneObject>> objects_ref;
	{
		const auto model = gltfSdkManager::LoadSceneShared(file.c_str());
		ASSERT_TRUE(model);
		model_ref = model;
		objects_ref = gltfSdkManager::ReadSceneObjectsShared(model);
	}
	EXPECT_FALSE(model_ref.expired());
	EXPECT_FALSE(objects_ref.expired());
	gltfSdkManager::CleanUp();
	EXPECT_TRUE(model_ref.expired());
	EXPECT_TRUE(objects_ref.expired());
}

TEST(gltfSceneFile, ReadObjectsReturnsIndependentCopy)
{
	ietTempDir dir;
	const std::string file = WriteScene(dir, false);
	std::vector<gltfSceneObject> objects;
	ASSERT_TRUE(gltfSceneFile::ReadObjects(dir.GetLocator("scene.gltf"), objects));
	objects.front().m_Name = "Edited";
	objects.clear();
	const auto model = gltfSdkManager::LoadSceneShared(file.c_str());
	ASSERT_TRUE(model);
	const auto cached = gltfSdkManager::ReadSceneObjectsShared(model);
	ASSERT_EQ(2u, cached->size());
	EXPECT_EQ("A", cached->front().m_Name);
}

TEST(gltfSceneFile, ReadObjects)
{
	ietTempDir dir;
	WriteScene(dir, false);
	std::vector<gltfSceneObject> objects;
	ASSERT_TRUE(gltfSceneFile::ReadObjects(dir.GetLocator("scene.gltf"), objects));
	ASSERT_EQ(2u, objects.size());
	EXPECT_EQ("A:mesh", objects[0].m_Identity);
	EXPECT_EQ("B:camera", objects[1].m_Identity);

	EXPECT_FALSE(gltfSceneFile::ReadObjects(dir.GetLocator("missing.gltf"), objects));
	EXPECT_TRUE(objects.empty());
}
