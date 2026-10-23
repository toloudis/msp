/*****************************************************************************
**  gltfImageExtractTest.cpp
**
**      Tests of gltfImageExtract: embedded glTF images are written to the
**      texture folder only when their bytes differ from the file there.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_IMAGEEXTRACT_HPP
#include "ImportExport/gltf/import/private/gltfImageExtract.hpp"
#endif

#include "ietTempDir.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace
{
	std::vector<unsigned char> ReadAll(const std::filesystem::path& i_Path)
	{
		std::ifstream in(i_Path, std::ios::in | std::ios::binary);
		return std::vector<unsigned char>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	gltfImageExtract::WriteResult Write(const std::filesystem::path& i_Path, const std::vector<unsigned char>& i_Bytes)
	{
		return gltfImageExtract::WriteFileIfChanged(i_Path, i_Bytes.data(), i_Bytes.size());
	}
}

TEST(WriteFileIfChanged, WritesNewFile)
{
	ietTempDir tmp;
	const std::filesystem::path path = tmp.GetPath() / "a.bin";
	const std::vector<unsigned char> bytes = { 1, 2, 3, 4 };

	EXPECT_EQ(gltfImageExtract::e_Written, Write(path, bytes));
	EXPECT_EQ(bytes, ReadAll(path));
}

TEST(WriteFileIfChanged, SkipsIdenticalFile)
{
	ietTempDir tmp;
	const std::filesystem::path path = tmp.GetPath() / "a.bin";
	const std::vector<unsigned char> bytes = { 1, 2, 3, 4 };

	ASSERT_EQ(gltfImageExtract::e_Written, Write(path, bytes));
	EXPECT_EQ(gltfImageExtract::e_Unchanged, Write(path, bytes));
}

TEST(WriteFileIfChanged, RewritesSameSizeDifferentBytes)
{
	ietTempDir tmp;
	const std::filesystem::path path = tmp.GetPath() / "a.bin";
	const std::vector<unsigned char> changed = { 1, 2, 3, 5 };

	ASSERT_EQ(gltfImageExtract::e_Written, Write(path, { 1, 2, 3, 4 }));
	EXPECT_EQ(gltfImageExtract::e_Written, Write(path, changed));
	EXPECT_EQ(changed, ReadAll(path));
}

TEST(WriteFileIfChanged, RewritesDifferentSize)
{
	ietTempDir tmp;
	const std::filesystem::path path = tmp.GetPath() / "a.bin";
	const std::vector<unsigned char> shorter = { 1, 2, 3 };

	ASSERT_EQ(gltfImageExtract::e_Written, Write(path, { 1, 2, 3, 4, 5, 6 }));
	EXPECT_EQ(gltfImageExtract::e_Written, Write(path, shorter));
	EXPECT_EQ(shorter, ReadAll(path));
}

TEST(WriteFileIfChanged, FailsInMissingFolder)
{
	ietTempDir tmp;
	EXPECT_EQ(gltfImageExtract::e_Failed, Write(tmp.GetPath() / "nope" / "a.bin", { 1, 2, 3, 4 }));
}

namespace
{
	const std::vector<unsigned char> c_EncodedBytes = { 0x89, 'P', 'N', 'G', 1, 2, 3, 4 };

	//	A model whose image 0 is still encoded, in a .glb buffer view
	tinygltf::Model MakeBufferViewModel(const char* i_pMimeType = "image/png")
	{
		tinygltf::Model model;
		model.buffers.resize(1);
		model.buffers[0].data = c_EncodedBytes;
		model.bufferViews.resize(1);
		model.bufferViews[0].buffer = 0;
		model.bufferViews[0].byteOffset = 0;
		model.bufferViews[0].byteLength = c_EncodedBytes.size();
		model.images.resize(1);
		model.images[0].bufferView = 0;
		model.images[0].mimeType = i_pMimeType;
		return model;
	}

	//	A model whose image 0 tinygltf has decoded (a data URI): 2x2 RGBA
	tinygltf::Model MakePixelModel()
	{
		tinygltf::Model model;
		model.images.resize(1);
		tinygltf::Image& image = model.images[0];
		image.width = 2;
		image.height = 2;
		image.component = 4;
		image.bits = 8;
		image.image = { 255, 0, 0, 255,   0, 255, 0, 255,   0, 0, 255, 255,   255, 255, 255, 128 };
		return model;
	}

	std::string Extract(const tinygltf::Model& i_Model, const ietTempDir& i_Tmp, gltfImageExtract::WriteResult& o_Result)
	{
		return gltfImageExtract::ExtractEmbeddedImage(i_Model, 0, i_Tmp.GetLocator("scene.glb"), &o_Result);
	}
}

TEST(ExtractEmbeddedImage, BufferViewImageWrittenAsIs)
{
	ietTempDir tmp;
	gltfImageExtract::WriteResult result = gltfImageExtract::e_Failed;
	const std::string path = Extract(MakeBufferViewModel(), tmp, result);

	EXPECT_EQ((tmp.GetPath() / "scene_textures" / "image0.png").string(), path);
	EXPECT_EQ(gltfImageExtract::e_Written, result);
	EXPECT_EQ(c_EncodedBytes, ReadAll(path));
}

TEST(ExtractEmbeddedImage, BufferViewImageSkippedOnSecondLoad)
{
	ietTempDir tmp;
	const tinygltf::Model model = MakeBufferViewModel();
	gltfImageExtract::WriteResult result = gltfImageExtract::e_Failed;
	const std::string first = Extract(model, tmp, result);
	ASSERT_EQ(gltfImageExtract::e_Written, result);

	const std::string second = Extract(model, tmp, result);
	EXPECT_EQ(gltfImageExtract::e_Unchanged, result);
	EXPECT_EQ(first, second);
}

TEST(ExtractEmbeddedImage, JpegMimeGetsJpgExtension)
{
	ietTempDir tmp;
	gltfImageExtract::WriteResult result = gltfImageExtract::e_Failed;
	const std::string path = Extract(MakeBufferViewModel("image/jpeg"), tmp, result);

	EXPECT_EQ((tmp.GetPath() / "scene_textures" / "image0.jpg").string(), path);
}

TEST(ExtractEmbeddedImage, DecodedPixelsSkippedOnSecondLoad)
{
	ietTempDir tmp;
	const tinygltf::Model model = MakePixelModel();
	gltfImageExtract::WriteResult result = gltfImageExtract::e_Failed;
	const std::string path = Extract(model, tmp, result);
	ASSERT_EQ(gltfImageExtract::e_Written, result);

	const std::vector<unsigned char> bytes = ReadAll(path);
	ASSERT_GE(bytes.size(), 4u);
	EXPECT_EQ(std::vector<unsigned char>({ 0x89, 0x50, 0x4E, 0x47 }), std::vector<unsigned char>(bytes.begin(), bytes.begin() + 4));

	Extract(model, tmp, result);
	EXPECT_EQ(gltfImageExtract::e_Unchanged, result);
}

TEST(ExtractEmbeddedImage, CreatesTextureFolder)
{
	ietTempDir tmp;
	const std::filesystem::path folder = tmp.GetPath() / "scene_textures";
	ASSERT_FALSE(std::filesystem::exists(folder));

	gltfImageExtract::WriteResult result = gltfImageExtract::e_Failed;
	const std::string path = Extract(MakeBufferViewModel(), tmp, result);

	EXPECT_TRUE(std::filesystem::is_directory(folder));
	EXPECT_EQ(gltfImageExtract::e_Written, result);
	EXPECT_TRUE(std::filesystem::exists(path));
}

TEST(ExtractEmbeddedImage, NoDataReturnsEmpty)
{
	ietTempDir tmp;
	tinygltf::Model model;
	model.images.resize(1);

	gltfImageExtract::WriteResult result = gltfImageExtract::e_Written;
	EXPECT_EQ("", Extract(model, tmp, result));
	EXPECT_EQ(gltfImageExtract::e_Failed, result);
}
