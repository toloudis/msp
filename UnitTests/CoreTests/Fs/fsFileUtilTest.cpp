/*****************************************************************************
**  fsFileUtilTest.cpp
**
**      What each fsFileUtil file operation does, including the exception
**      it throws when it fails. Written against the Win32 implementation
**      before fsFileUtil moved to std::filesystem, so the move keeps the
**      same behavior.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace
{
	fsLocator Child(const fsLocator& i_Dir, const std::string& i_Name)
	{
		fsLocator locator = i_Dir;
		locator.Push(i_Name);
		return locator;
	}

	std::filesystem::path NativePath(const fsLocator& i_Locator)
	{
		std::string utf8;
		fsFileUtil::LocatorToUnicodeString(i_Locator, utf8);
		return std::filesystem::path(envString::UTF8ToWideChar(utf8));
	}

	void WriteText(const fsLocator& i_File, const std::string& i_Text)
	{
		std::ofstream out(NativePath(i_File), std::ios::binary);
		out << i_Text;
	}

	std::string ReadText(const fsLocator& i_File)
	{
		std::ifstream in(NativePath(i_File), std::ios::binary);
		return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	//	The exception's locator, as a string, for comparing with the expected one
	template <class X, class F>
	std::string ThrownLocator(F i_Call)
	{
		try
		{
			i_Call();
		}
		catch (const X& i_Ex)
		{
			std::string name;
			fsFileUtil::LocatorToUnicodeString(i_Ex.GetLocator(), name);
			return name;
		}
		return "(nothing thrown)";
	}

	std::string Str(const fsLocator& i_Locator)
	{
		std::string name;
		fsFileUtil::LocatorToUnicodeString(i_Locator, name);
		return name;
	}
}

//----------------------------------------------------------------------------
//	CreateFile
//----------------------------------------------------------------------------
TEST(fsFileUtil, CreateFileMakesAnEmptyFile)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("new.txt");
	fsFileUtil::CreateFile(file);
	EXPECT_TRUE(fsFileUtil::FileExists(file));
	EXPECT_EQ("", ReadText(file));
}

TEST(fsFileUtil, CreateFileFailsWhenTheFileExists)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("new.txt");
	WriteText(file, "keep");
	EXPECT_THROW(fsFileUtil::CreateFile(file), fsFileExistsX);
	EXPECT_EQ("keep", ReadText(file));
}

TEST(fsFileUtil, CreateFileFailsWhenTheDirectoryIsMissing)
{
	ctTempDir dir;
	fsLocator file = Child(dir.GetLocator("missing"), "new.txt");
	EXPECT_THROW(fsFileUtil::CreateFile(file), fsDirectoryDoesntExistX);
}

TEST(fsFileUtil, CreateFileFailsForAnEmptyLocator)
{
	EXPECT_THROW(fsFileUtil::CreateFile(fsLocator()), fsInvalidLocatorX);
}

//----------------------------------------------------------------------------
//	DeleteFile
//----------------------------------------------------------------------------
TEST(fsFileUtil, DeleteFileRemovesTheFile)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("gone.txt");
	WriteText(file, "x");
	fsFileUtil::DeleteFile(file);
	EXPECT_FALSE(fsFileUtil::FileExists(file));
}

TEST(fsFileUtil, DeleteFileFailsForAMissingFile)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileUtil::DeleteFile(dir.GetLocator("missing.txt")), fsFileDoesntExistX);
}

TEST(fsFileUtil, DeleteFileFailsForAFileInAMissingDirectory)
{
	ctTempDir dir;
	fsLocator file = Child(dir.GetLocator("missing"), "missing.txt");
	EXPECT_THROW(fsFileUtil::DeleteFile(file), fsDirectoryDoesntExistX);
}

TEST(fsFileUtil, DeleteFileLeavesAReadOnlyFile)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("locked.txt");
	WriteText(file, "x");
	fsFileUtil::SetReadOnly(file, true);
	EXPECT_THROW(fsFileUtil::DeleteFile(file), fsReadOnlyX);
	EXPECT_TRUE(fsFileUtil::FileExists(file));
	fsFileUtil::SetReadOnly(file, false);
}

TEST(fsFileUtil, DeleteFileLeavesADirectory)
{
	ctTempDir dir;
	fsLocator sub = dir.GetLocator("sub");
	fsFileUtil::CreateDirectory(sub);
	EXPECT_THROW(fsFileUtil::DeleteFile(sub), fsReadOnlyX);
	EXPECT_TRUE(fsFileUtil::DirectoryExists(sub));
}

//----------------------------------------------------------------------------
//	RenameFile
//----------------------------------------------------------------------------
TEST(fsFileUtil, RenameFileMovesTheFile)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("from.txt");
	fsLocator to = dir.GetLocator("to.txt");
	WriteText(from, "data");
	fsFileUtil::RenameFile(from, to);
	EXPECT_FALSE(fsFileUtil::FileExists(from));
	EXPECT_EQ("data", ReadText(to));
}

TEST(fsFileUtil, RenameFileMovesADirectory)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("fromDir");
	fsLocator to = dir.GetLocator("toDir");
	fsFileUtil::CreateDirectory(from);
	WriteText(Child(from, "inside.txt"), "data");
	fsFileUtil::RenameFile(from, to);
	EXPECT_FALSE(fsFileUtil::DirectoryExists(from));
	EXPECT_EQ("data", ReadText(Child(to, "inside.txt")));
}

TEST(fsFileUtil, RenameFileDoesNotReplaceAnExistingFile)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("from.txt");
	fsLocator to = dir.GetLocator("to.txt");
	WriteText(from, "new");
	WriteText(to, "old");
	EXPECT_EQ(Str(to), ThrownLocator<fsFileExistsX>([&] { fsFileUtil::RenameFile(from, to); }));
	EXPECT_EQ("new", ReadText(from));
	EXPECT_EQ("old", ReadText(to));
}

TEST(fsFileUtil, RenameFileCanChangeOnlyTheCase)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("name.txt");
	fsLocator to = dir.GetLocator("NAME.txt");
	WriteText(from, "data");
	fsFileUtil::RenameFile(from, to);
	EXPECT_EQ("NAME.txt", std::filesystem::directory_iterator(dir.GetPath())->path().filename().u8string());
}

TEST(fsFileUtil, RenameFileFailsForAMissingFile)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("missing.txt");
	fsLocator to = dir.GetLocator("to.txt");
	EXPECT_EQ(Str(from), ThrownLocator<fsFileDoesntExistX>([&] { fsFileUtil::RenameFile(from, to); }));
}

//----------------------------------------------------------------------------
//	CopyFile
//----------------------------------------------------------------------------
TEST(fsFileUtil, CopyFileCopiesTheContents)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("from.txt");
	fsLocator to = dir.GetLocator("to.txt");
	WriteText(from, "data");
	fsFileUtil::CopyFile(from, to);
	EXPECT_EQ("data", ReadText(from));
	EXPECT_EQ("data", ReadText(to));
}

TEST(fsFileUtil, CopyFileDoesNotReplaceAnExistingFile)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("from.txt");
	fsLocator to = dir.GetLocator("to.txt");
	WriteText(from, "new");
	WriteText(to, "old");
	EXPECT_EQ(Str(to), ThrownLocator<fsFileExistsX>([&] { fsFileUtil::CopyFile(from, to); }));
	EXPECT_EQ("old", ReadText(to));
}

TEST(fsFileUtil, CopyFileFailsForAMissingFile)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("missing.txt");
	EXPECT_EQ(Str(from), ThrownLocator<fsFileDoesntExistX>([&] { fsFileUtil::CopyFile(from, dir.GetLocator("to.txt")); }));
}

//----------------------------------------------------------------------------
//	Directories
//----------------------------------------------------------------------------
TEST(fsFileUtil, CreateDirectoryMakesEveryLevel)
{
	ctTempDir dir;
	fsLocator deep = Child(Child(dir.GetLocator("a"), "b"), "c");
	fsFileUtil::CreateDirectory(deep);
	EXPECT_TRUE(fsFileUtil::DirectoryExists(deep));

	// and does nothing when the directory is already there
	fsFileUtil::CreateDirectory(deep);
	EXPECT_TRUE(fsFileUtil::DirectoryExists(deep));
}

TEST(fsFileUtil, CreateDirectoryFailsForAnEmptyLocator)
{
	EXPECT_THROW(fsFileUtil::CreateDirectory(fsLocator()), fsDirectoryDoesntExistX);
}

TEST(fsFileUtil, DeleteDirectoryRemovesAnEmptyDirectory)
{
	ctTempDir dir;
	fsLocator sub = dir.GetLocator("sub");
	fsFileUtil::CreateDirectory(sub);
	fsFileUtil::DeleteDirectory(sub);
	EXPECT_FALSE(fsFileUtil::DirectoryExists(sub));
}

TEST(fsFileUtil, DeleteDirectoryLeavesADirectoryWithFiles)
{
	ctTempDir dir;
	fsLocator sub = dir.GetLocator("sub");
	fsFileUtil::CreateDirectory(sub);
	WriteText(Child(sub, "file.txt"), "x");
	EXPECT_THROW(fsFileUtil::DeleteDirectory(sub), fsDirectoryNotEmptyX);
	EXPECT_TRUE(fsFileUtil::FileExists(Child(sub, "file.txt")));
}

TEST(fsFileUtil, DeleteDirectoryFailsForAMissingDirectory)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileUtil::DeleteDirectory(dir.GetLocator("missing")), fsFileDoesntExistX);
}

TEST(fsFileUtil, DeleteDirectoryRecursivelyDeletesTheFiles)
{
	ctTempDir dir;
	fsLocator sub = dir.GetLocator("sub");
	fsFileUtil::CreateDirectory(sub);
	WriteText(Child(sub, "a.txt"), "a");
	WriteText(Child(sub, "b.txt"), "b");
	EXPECT_TRUE(fsFileUtil::DeleteDirectoryRecursively(sub));
	EXPECT_FALSE(fsFileUtil::FileExists(Child(sub, "a.txt")));
	EXPECT_FALSE(fsFileUtil::FileExists(Child(sub, "b.txt")));
	EXPECT_FALSE(fsFileUtil::DeleteDirectoryRecursively(dir.GetLocator("missing")));
}

TEST(fsFileUtil, CopyDirectoryCopiesFilesAndSubdirectories)
{
	ctTempDir dir;
	fsLocator from = dir.GetLocator("from");
	fsFileUtil::CreateDirectory(Child(from, "inner"));
	WriteText(Child(from, "a.txt"), "a");
	WriteText(Child(Child(from, "inner"), "b.txt"), "b");

	fsLocator to = dir.GetLocator("to");
	fsFileUtil::CopyDirectory(from, to);
	EXPECT_EQ("a", ReadText(Child(to, "a.txt")));
	EXPECT_EQ("b", ReadText(Child(Child(to, "inner"), "b.txt")));
}

//----------------------------------------------------------------------------
//	Existence and attributes
//----------------------------------------------------------------------------
TEST(fsFileUtil, ExistsIsFalseForAnEmptyOrMissingLocator)
{
	ctTempDir dir;
	EXPECT_FALSE(fsFileUtil::FileExists(fsLocator()));
	EXPECT_FALSE(fsFileUtil::DirectoryExists(fsLocator()));
	EXPECT_FALSE(fsFileUtil::FileExists(dir.GetLocator("missing.txt")));
	EXPECT_FALSE(fsFileUtil::DirectoryExists(dir.GetLocator("missing")));
}

TEST(fsFileUtil, IsReadOnlyFailsForAMissingFileOrADirectory)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileUtil::IsReadOnly(dir.GetLocator("missing.txt")), fsFileDoesntExistX);
	EXPECT_THROW(fsFileUtil::IsReadOnly(dir.GetLocator()), fsFileDoesntExistX);
	EXPECT_FALSE(fsFileUtil::IsReadOnly(fsLocator()));
}

TEST(fsFileUtil, SetReadOnlyIgnoresAMissingFile)
{
	ctTempDir dir;
	fsFileUtil::SetReadOnly(dir.GetLocator("missing.txt"), true);
	EXPECT_FALSE(fsFileUtil::FileExists(dir.GetLocator("missing.txt")));
}

TEST(fsFileUtil, NonAsciiNames)
{
	ctTempDir dir;
	fsLocator sub = dir.GetLocator();
	sub.Push(std::string("Ünïcødé Ωμέγα"));
	fsFileUtil::CreateDirectory(sub);
	EXPECT_TRUE(fsFileUtil::DirectoryExists(sub));

	fsLocator file = Child(sub, "日本語.txt");
	fsFileUtil::CreateFile(file);
	EXPECT_TRUE(fsFileUtil::FileExists(file));
	EXPECT_TRUE(std::filesystem::exists(dir.GetPath() / std::filesystem::u8path("Ünïcødé Ωμέγα/日本語.txt")));

	fsLocator copy = Child(sub, "コピー.txt");
	fsFileUtil::CopyFile(file, copy);
	fsFileUtil::DeleteFile(file);
	fsFileUtil::RenameFile(copy, file);
	EXPECT_TRUE(fsFileUtil::FileExists(file));
	EXPECT_FALSE(fsFileUtil::FileExists(copy));
}

//----------------------------------------------------------------------------
//	Names
//----------------------------------------------------------------------------
TEST(fsFileUtil, GenerateFileNameSkipsTakenNames)
{
	ctTempDir dir;
	std::string prefix = "shot", extension = ".png", name;
	fsFileUtil::GenerateFileName(dir.GetLocator(), prefix, extension, name);
	EXPECT_EQ("shot.png", name);

	WriteText(dir.GetLocator("shot.png"), "x");
	WriteText(dir.GetLocator("shot0.png"), "x");
	fsFileUtil::GenerateFileName(dir.GetLocator(), prefix, extension, name);
	EXPECT_EQ("shot1.png", name);
}

TEST(fsFileUtil, IsFileNameAbsolute)
{
	fsLocator locator;
	fsFileUtil::ANSIFilenameToLocator("C:\\dir\\file.txt", locator);
	EXPECT_TRUE(fsFileUtil::IsFileNameAbsolute(locator));
	fsFileUtil::ANSIFilenameToLocator("\\\\server\\share\\file.txt", locator);
	EXPECT_TRUE(fsFileUtil::IsFileNameAbsolute(locator));
	fsFileUtil::ANSIFilenameToLocator("dir\\file.txt", locator);
	EXPECT_FALSE(fsFileUtil::IsFileNameAbsolute(locator));
	EXPECT_FALSE(fsFileUtil::IsFileNameAbsolute(fsLocator()));
}

TEST(fsFileUtil, LocatorStrings)
{
	fsLocator locator;
	fsFileUtil::ANSIFilenameToLocator("C:/dir\\sub//file.txt", locator);
	ASSERT_EQ(4, locator.GetNumNames());
	EXPECT_EQ("C:", locator.GetName(0));
	EXPECT_EQ("file.txt", locator.GetLastName());
	EXPECT_FALSE(locator.IsUNCPath());
	EXPECT_EQ("C:\\dir\\sub\\file.txt", Str(locator));

	std::string doubled;
	fsFileUtil::LocatorToANSIFilename(locator, doubled, true);
	EXPECT_EQ("C:\\\\dir\\\\sub\\\\file.txt", doubled);

	fsFileUtil::ANSIFilenameToLocator("\\\\server\\share\\file.txt", locator);
	EXPECT_TRUE(locator.IsUNCPath());
	ASSERT_EQ(3, locator.GetNumNames());
	EXPECT_EQ("server", locator.GetName(0));
	EXPECT_EQ("\\\\server\\share\\file.txt", Str(locator));

	fsFileUtil::ANSIFilenameToLocator("", locator);
	EXPECT_EQ(0, locator.GetNumNames());
	EXPECT_EQ("", Str(locator));
}
