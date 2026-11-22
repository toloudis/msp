/*****************************************************************************
**  fsPathUtilTest.cpp
**
**      fsPathUtil: locators and UTF-8 strings to std::filesystem::path and
**      back.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

namespace
{
	// The platform's separator, which LocatorToPath joins names with
	const std::string c_Sep(1, char(std::filesystem::path::preferred_separator));

	fsLocator FromString(const std::string& i_Utf8)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(i_Utf8, locator);
		return locator;
	}

	std::string Saved(const fsLocator& i_Locator)
	{
		std::string utf8;
		fsFileUtil::LocatorToUnicodeString(i_Locator, utf8);
		return utf8;
	}
}

TEST(fsPathUtil, Utf8RoundTrip)
{
	const std::string utf8 = "dir/Ünïcødé 日本語.txt";
	const std::filesystem::path path = fsPathUtil::Utf8ToPath(utf8);
	EXPECT_EQ(std::filesystem::path(L"dir/\u00DCn\u00EFc\u00F8d\u00E9 \u65E5\u672C\u8A9E.txt"), path);
	EXPECT_EQ(utf8, fsPathUtil::PathToUtf8(path));
}

TEST(fsPathUtil, LocatorToPathJoinsWithThePlatformSeparator)
{
	EXPECT_EQ("C:" + c_Sep + "dir" + c_Sep + "file.txt",
		fsPathUtil::PathToUtf8(fsPathUtil::LocatorToPath(FromString("C:/dir\\file.txt"))));
}

TEST(fsPathUtil, LocatorToPathKeepsADriveAbsolute)
{
	// "C:" / "dir" would be the drive-relative "C:dir"
	const std::filesystem::path path = fsPathUtil::LocatorToPath(FromString("C:\\dir"));
	EXPECT_EQ("C:" + c_Sep + "dir", fsPathUtil::PathToUtf8(path));
#ifdef _WIN32
	EXPECT_TRUE(path.is_absolute());
#endif
}

TEST(fsPathUtil, LocatorToPathOfAUNCLocator)
{
	const fsLocator locator = FromString("\\\\server\\share\\file.txt");
	ASSERT_TRUE(locator.IsUNCPath());
	EXPECT_EQ(c_Sep + c_Sep + "server" + c_Sep + "share" + c_Sep + "file.txt",
		fsPathUtil::PathToUtf8(fsPathUtil::LocatorToPath(locator)));
}

TEST(fsPathUtil, LocatorToPathOfNonAsciiNames)
{
	fsLocator locator;
	locator.Push(std::string("Ünïcødé"));
	locator.Push(std::string("日本語.txt"));
	const std::filesystem::path path = fsPathUtil::LocatorToPath(locator);
	EXPECT_EQ(std::filesystem::path(L"\u00DCn\u00EFc\u00F8d\u00E9") / L"\u65E5\u672C\u8A9E.txt", path);
}

TEST(fsPathUtil, LocatorToPathOfAnEmptyLocatorIsEmpty)
{
	EXPECT_TRUE(fsPathUtil::LocatorToPath(fsLocator()).empty());
	EXPECT_TRUE(fsPathUtil::LocatorToPath(fsLocator(2)).empty());
}

TEST(fsPathUtil, LocatorToPathIgnoresTheIndex)
{
	EXPECT_EQ("sub" + c_Sep + "file.txt",
		fsPathUtil::PathToUtf8(fsPathUtil::LocatorToPath(fsLocator(FromString("sub\\file.txt"), 3))));
}

TEST(fsPathUtil, PathToLocator)
{
	fsLocator locator;
	fsPathUtil::PathToLocator(std::filesystem::path(L"C:/dir\\\u65E5\u672C\u8A9E.txt"), locator);
	ASSERT_EQ(3, locator.GetNumNames());
	EXPECT_EQ("日本語.txt", locator.GetLastName());
	EXPECT_EQ("C:\\dir\\日本語.txt", Saved(locator));

	fsPathUtil::PathToLocator(std::filesystem::path(L"\\\\server\\share"), locator);
	EXPECT_TRUE(locator.IsUNCPath());
	EXPECT_EQ("\\\\server\\share", Saved(locator));
}
