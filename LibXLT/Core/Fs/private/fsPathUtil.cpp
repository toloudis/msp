/****************************************************************************\
**  fsPathUtil.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Core/fs/fsPathUtil.hpp"

#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


namespace fsPathUtil
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::filesystem::path Utf8ToPath(const std::string& i_Utf8)
{
	return std::filesystem::u8path(i_Utf8);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string PathToUtf8(const std::filesystem::path& i_Path)
{
	return i_Path.u8string();
}

//----------------------------------------------------------------------------
//	The names are joined as one string and converted once: appending them
//	with path::operator/ would make "C:" / "dir" the drive-relative "C:dir".
//----------------------------------------------------------------------------
std::filesystem::path LocatorToPath(const fsLocator& i_Locator)
{
	const int num_names = i_Locator.GetNumNames();
	if (num_names <= 0)
		return std::filesystem::path();

	// The preferred separator is ASCII ('\\' on Windows, '/' elsewhere)
	const char separator = char(std::filesystem::path::preferred_separator);

	std::string utf8;
	if (i_Locator.IsUNCPath())
	{
		utf8 += separator;
		utf8 += separator;
	}
	for (int i = 0; i < num_names; ++i)
	{
		if (i > 0)
			utf8 += separator;
		utf8 += i_Locator.GetName(i);
	}
	return Utf8ToPath(utf8);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void PathToLocator(const std::filesystem::path& i_Path, fsLocator& o_Locator)
{
	fsFileUtil::UnicodeStringToLocator(PathToUtf8(i_Path), o_Locator);
}

}
