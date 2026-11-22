/*****************************************************************************
**  fsFileEnumTest.cpp
**
**      fsFileEnum: listing files and subdirectories. Written against the
**      Win32 implementation (FindFirstFileW) before it moved to
**      std::filesystem::directory_iterator, so the move keeps the same
**      results. The "." and ".." entries FindFirstFileW also gave are left
**      out of the directory checks: every caller skipped them, and
**      directory_iterator doesn't give them.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_FILEENUM_HPP
#include "Core/fs/fsFileEnum.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
	fsLocator Child(const fsLocator& i_Dir, const std::string& i_Name)
	{
		fsLocator locator = i_Dir;
		locator.Push(i_Name);
		return locator;
	}

	void Touch(const ctTempDir& i_Dir, const std::string& i_RelativeUtf8)
	{
		std::filesystem::path path = i_Dir.GetPath() / std::filesystem::u8path(i_RelativeUtf8);
		std::filesystem::create_directories(path.parent_path());
		std::ofstream(path) << "x";
	}

	void MakeDir(const ctTempDir& i_Dir, const std::string& i_RelativeUtf8)
	{
		std::filesystem::create_directories(i_Dir.GetPath() / std::filesystem::u8path(i_RelativeUtf8));
	}

	//	The last names in a list, sorted, without "." and ".."
	std::vector<std::string> Names(const fsFileEnum::fsFileList& i_List)
	{
		std::vector<std::string> names;
		for (const fsLocator& locator : i_List)
		{
			const std::string& name = locator.GetLastName();
			if (name != "." && name != "..")
				names.push_back(name);
		}
		std::sort(names.begin(), names.end());
		return names;
	}

	//	A target that stops after the first file
	class StopAfterOne : public fsFileEnum::EnumTarget
	{
		public:
			int m_Count = 0;
			bool Notify(const fsLocator&, const fsLocator&) override
			{
				++m_Count;
				return false;
			}
	};

	//	A target that records what it was given, unchanged
	class Recorder : public fsFileEnum::EnumTarget
	{
		public:
			std::vector<fsLocator> m_Directories, m_Files;
			bool Notify(const fsLocator& i_Directory, const fsLocator& i_File) override
			{
				m_Directories.push_back(i_Directory);
				m_Files.push_back(i_File);
				return true;
			}
	};

	using Strings = std::vector<std::string>;
}

TEST(fsFileEnum, EnumerateFilesListsOnlyFiles)
{
	ctTempDir dir;
	Touch(dir, "b.txt");
	Touch(dir, "a.png");
	MakeDir(dir, "sub");

	fsFileEnum::fsFileList list;
	EXPECT_TRUE(fsFileEnum::EnumerateFiles(dir.GetLocator(), list));
	EXPECT_EQ((Strings{"a.png", "b.txt"}), Names(list));

	// each entry is the full locator: the directory plus the name
	ASSERT_EQ(2u, list.size());
	EXPECT_TRUE(list[0].Equals(Child(dir.GetLocator(), list[0].GetLastName())));
}

TEST(fsFileEnum, TheTargetGetsTheDirectoryAndARelativeName)
{
	ctTempDir dir;
	Touch(dir, "only.txt");

	Recorder target;
	fsFileEnum::EnumerateFiles(dir.GetLocator(), target);
	ASSERT_EQ(1u, target.m_Files.size());
	EXPECT_TRUE(target.m_Directories[0] == dir.GetLocator());
	EXPECT_EQ(1, target.m_Files[0].GetNumNames());
	EXPECT_EQ("only.txt", target.m_Files[0].GetLastName());
}

TEST(fsFileEnum, EnumerateFilesKeepsNamesWithASubstring)
{
	ctTempDir dir;
	Touch(dir, "a.txt");
	Touch(dir, "b.png");
	Touch(dir, "c.mtl");

	fsFileEnum::fsFileList list;
	fsFileEnum::EnumerateFiles(dir.GetLocator(), list, std::string(".txt"));
	EXPECT_EQ((Strings{"a.txt"}), Names(list));

	fsFileEnum::EnumerateFiles(dir.GetLocator(), list, Strings{".png", ".mtl"});
	EXPECT_EQ((Strings{"b.png", "c.mtl"}), Names(list));

	// the match is case-sensitive
	fsFileEnum::EnumerateFiles(dir.GetLocator(), list, std::string(".TXT"));
	EXPECT_TRUE(Names(list).empty());
}

TEST(fsFileEnum, EnumerateFilesCanSearchSubdirectories)
{
	ctTempDir dir;
	Touch(dir, "top.txt");
	Touch(dir, "sub/inner.txt");
	Touch(dir, "sub/deeper/deepest.txt");
	Touch(dir, "sub/skip.png");

	fsFileEnum::fsFileList list;
	fsFileEnum::EnumerateFiles(dir.GetLocator(), list, std::string(".txt"), true);
	EXPECT_EQ((Strings{"deepest.txt", "inner.txt", "top.txt"}), Names(list));

	fsFileEnum::EnumerateFiles(dir.GetLocator(), list, std::string(".txt"), false);
	EXPECT_EQ((Strings{"top.txt"}), Names(list));
}

TEST(fsFileEnum, EnumerateDirectoriesListsOnlyDirectories)
{
	ctTempDir dir;
	MakeDir(dir, "one");
	MakeDir(dir, "two");
	Touch(dir, "file.txt");

	fsFileEnum::fsFileList list;
	EXPECT_TRUE(fsFileEnum::EnumerateDirectories(dir.GetLocator(), list));
	EXPECT_EQ((Strings{"one", "two"}), Names(list));
}

TEST(fsFileEnum, AMissingDirectoryListsNothing)
{
	ctTempDir dir;
	fsFileEnum::fsFileList list;
	EXPECT_TRUE(fsFileEnum::EnumerateFiles(dir.GetLocator("missing"), list));
	EXPECT_TRUE(list.empty());
	EXPECT_TRUE(fsFileEnum::EnumerateDirectories(dir.GetLocator("missing"), list));
	EXPECT_TRUE(Names(list).empty());
}

TEST(fsFileEnum, TheTargetCanStopTheListing)
{
	ctTempDir dir;
	Touch(dir, "a.txt");
	Touch(dir, "b.txt");
	Touch(dir, "sub/c.txt");

	StopAfterOne target;
	EXPECT_FALSE(fsFileEnum::EnumerateFiles(dir.GetLocator(), target, true));
	EXPECT_EQ(1, target.m_Count);

	// a list isn't filled when the listing stopped (it never does for a list)
	MakeDir(dir, "d1");
	MakeDir(dir, "d2");
	StopAfterOne dir_target;
	EXPECT_FALSE(fsFileEnum::EnumerateDirectories(dir.GetLocator(), dir_target));
	EXPECT_EQ(1, dir_target.m_Count);
}

TEST(fsFileEnum, NonAsciiNames)
{
	ctTempDir dir;
	Touch(dir, "Ünïcødé/日本語.txt");

	fsFileEnum::fsFileList dirs;
	fsFileEnum::EnumerateDirectories(dir.GetLocator(), dirs);
	EXPECT_EQ((Strings{"Ünïcødé"}), Names(dirs));

	fsFileEnum::fsFileList files;
	fsFileEnum::EnumerateFiles(dir.GetLocator(), files, true);
	EXPECT_EQ((Strings{"日本語.txt"}), Names(files));
}
