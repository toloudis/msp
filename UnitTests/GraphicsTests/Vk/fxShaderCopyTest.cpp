/*****************************************************************************
**  fxShaderCopyTest.cpp
**
**      GraphicsVulkan's shader directory is an exact copy of GraphicsDX11's:
**      the HLSL is never edited, only compiled differently (see the
**      GraphicsVulkan design, "Shaders"). The Vulkan-only material shaders
**      (gtVulkanOnly) have no original, and each of them must exist.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtPlatform.hpp"
#include "Vk/gtVulkanOnly.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace
{
	namespace fs = std::filesystem;

	const char* const c_DX11Dir = "LibXLT/GraphicsDX11/Eff/private/Shaders";
	const char* const c_VkDir = "LibXLT/GraphicsVulkan/Eff/private/Shaders";

	std::vector<char> read_file(const fs::path& i_Path)
	{
		std::ifstream in(i_Path, std::ios::binary);
		return std::vector<char>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	bool is_shader_source(const fs::path& i_Path)
	{
		const std::string name = i_Path.filename().string();
		const std::string ext = i_Path.extension().string();
		return ext == ".hlsl" || ext == ".hlsli" ||
			   (name.size() > 12 && name.compare(name.size() - 12, 12, ".effect.json") == 0);
	}

	// Paths of the files under i_Dir, relative to it, with '/' separators
	std::set<std::string> list_files(const fs::path& i_Dir, bool i_bShaderSourcesOnly)
	{
		std::set<std::string> files;
		for (const fs::directory_entry& entry : fs::recursive_directory_iterator(i_Dir))
		{
			if (entry.is_regular_file() && (!i_bShaderSourcesOnly || is_shader_source(entry.path())))
			{
				files.insert(fs::relative(entry.path(), i_Dir).generic_string());
			}
		}
		return files;
	}
}

TEST(fxShaderCopy, IdenticalToDX11)
{
	const fs::path root = gtPlatform::RepoRoot();
	ASSERT_FALSE(root.empty()) << "repository root not found above the test exe";
	const fs::path dx11 = root / c_DX11Dir;
	const fs::path vk = root / c_VkDir;
	ASSERT_TRUE(fs::is_directory(vk)) << vk.string() << " is missing";

	const std::set<std::string> vkFiles = list_files(vk, false);
	const std::set<std::string> dx11Files = list_files(dx11, true);
	EXPECT_GT(dx11Files.size(), 100u);

	for (const std::string& file : gtVulkanOnly::Files())
	{
		EXPECT_TRUE(vkFiles.count(file)) << file << " is listed as Vulkan-only but doesn't exist";
		EXPECT_FALSE(dx11Files.count(file)) << file << " is listed as Vulkan-only but has a GraphicsDX11 original";
	}

	for (const std::string& file : vkFiles)
	{
		if (gtVulkanOnly::IsVulkanOnlyFile(file))
			continue;
		ASSERT_TRUE(fs::is_regular_file(dx11 / file)) << file << " has no GraphicsDX11 original";
		EXPECT_TRUE(read_file(vk / file) == read_file(dx11 / file)) << file << " differs from GraphicsDX11's";
	}
	for (const std::string& file : dx11Files)
	{
		EXPECT_TRUE(vkFiles.count(file)) << file << " is not copied to GraphicsVulkan";
	}
}
