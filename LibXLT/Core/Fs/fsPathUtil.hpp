/****************************************************************************\
**  fsPathUtil.hpp
**
**      fsPathUtil converts between fsLocators, UTF-8 strings and
**      std::filesystem::path, for code that hands a file name to the
**      operating system or the standard library.
**
**      Text is UTF-8 everywhere, but on Windows a path holds UTF-16 and
**      path(std::string) reads the string in the ANSI code page. Build
**      paths from UTF-8 with Utf8ToPath and read them back with PathToUtf8,
**      never with path(std::string) or path::string(). C++17's u8path and
**      u8string are used only here: C++20 deprecates u8path and makes
**      u8string return std::u8string, so only these two change then.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef FS_PATHUTIL_HPP
#error fsPathUtil.hpp multiply included
#endif
#define FS_PATHUTIL_HPP

#include <filesystem>
#include <string>


class fsLocator;


namespace fsPathUtil
{
	//----------------------------------------------------------------------------
	//	Utf8ToPath returns the path a UTF-8 string names.
	//----------------------------------------------------------------------------
	std::filesystem::path Utf8ToPath(const std::string& i_Utf8);

	//----------------------------------------------------------------------------
	//	PathToUtf8 returns the path as a UTF-8 string, its separators as stored.
	//----------------------------------------------------------------------------
	std::string PathToUtf8(const std::filesystem::path& i_Path);

	//----------------------------------------------------------------------------
	//	LocatorToPath returns the path a locator names: its names joined with
	//	the platform's separator, two of them in front of a UNC locator. An
	//	empty locator gives an empty path. The index is ignored, so expand an
	//	indexed locator first (gfFileTranslationMgr::ExpandLocator).
	//
	//	This is the path to give the operating system. The string a locator
	//	is saved as (fsFileUtil::LocatorToUnicodeString) always uses
	//	backslashes, whatever the platform.
	//----------------------------------------------------------------------------
	std::filesystem::path LocatorToPath(const fsLocator& i_Locator);

	//----------------------------------------------------------------------------
	//	PathToLocator returns the locator for a path, as
	//	fsFileUtil::UnicodeStringToLocator does for its string.
	//----------------------------------------------------------------------------
	void PathToLocator(const std::filesystem::path& i_Path, fsLocator& o_Locator);
}
