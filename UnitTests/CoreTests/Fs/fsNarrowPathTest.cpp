/*****************************************************************************
**  fsNarrowPathTest.cpp
**
**      Narrow (char) file names are UTF-8: fopen and the std::fstreams
**      given a UTF-8 std::string make and open the file of that name,
**      non-ASCII characters included. On Windows this needs UTF-8 as the
**      process code page (Utf8CodePage.manifest, merged into every exe by
**      Directory.Build.props); without it the C runtime reads the name in
**      the ANSI code page.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{

//	Accented Latin, CJK and a character outside the Basic Multilingual
//	Plane, so a name only survives if it isn't squeezed through a
//	single-byte code page.
const std::string c_Name = u8"Crème 東京 🎬.txt";

//	FindOnly returns whether i_Dir holds exactly one entry, named i_Name
//	(compared as the file system stores it, not through a narrow API).
bool FindOnly(const std::filesystem::path& i_Dir, const std::string& i_Name)
{
	std::vector<std::filesystem::path> names;
	for( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(i_Dir) )
		names.push_back(entry.path().filename());
	return names.size() == 1 && names[0] == std::filesystem::u8path(i_Name);
}

}	// namespace

TEST(fsNarrowPath, FopenTakesUTF8)
{
	ctTempDir dir;
	const std::string path = dir.GetPath().u8string() + "/" + c_Name;

	FILE* file = std::fopen(path.c_str(), "wb");
	ASSERT_NE(nullptr, file);
	std::fputs("fopen", file);
	std::fclose(file);
	EXPECT_TRUE(FindOnly(dir.GetPath(), c_Name));

	file = std::fopen(path.c_str(), "rb");
	ASSERT_NE(nullptr, file);
	char buffer[16] = {};
	EXPECT_EQ(5u, std::fread(buffer, 1, sizeof(buffer), file));
	std::fclose(file);
	EXPECT_EQ(std::string("fopen"), buffer);
}

TEST(fsNarrowPath, FstreamTakesUTF8)
{
	ctTempDir dir;
	const std::string path = dir.GetPath().u8string() + "/" + c_Name;

	{
		std::ofstream out(path, std::ios::binary);
		ASSERT_TRUE(out.is_open());
		out << "fstream";
	}
	EXPECT_TRUE(FindOnly(dir.GetPath(), c_Name));

	std::ifstream in(path, std::ios::binary);
	ASSERT_TRUE(in.is_open());
	std::string text;
	in >> text;
	EXPECT_EQ("fstream", text);
}
