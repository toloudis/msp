/****************************************************************************\
**  fsFileErrorUtil.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Core/fs/private/fsFileErrorUtil.hpp"

#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif

#include <filesystem>


namespace fsFileErrorUtil
{

namespace
{
	bool IsMissing(const std::error_code& i_Error)
	{
		return i_Error == std::errc::no_such_file_or_directory
			|| i_Error == std::errc::not_a_directory;
	}

	bool IsDenied(const std::error_code& i_Error)
	{
		return i_Error == std::errc::permission_denied
			|| i_Error == std::errc::operation_not_permitted
			|| i_Error == std::errc::read_only_file_system;
	}

	bool IsBusy(const std::error_code& i_Error)
	{
		return i_Error == std::errc::device_or_resource_busy
			|| i_Error == std::errc::text_file_busy
			|| i_Error == std::errc::no_lock_available;
	}

	bool IsDiskFull(const std::error_code& i_Error)
	{
		return i_Error == std::errc::no_space_on_device;
	}

	bool IsInvalid(const std::error_code& i_Error)
	{
		return i_Error == std::errc::invalid_argument
			|| i_Error == std::errc::filename_too_long
			|| i_Error == std::errc::no_such_device;
	}

	bool ParentDirectoryExists(const fsLocator& i_Locator)
	{
		if (i_Locator.GetNumNames() < 2)
			return true;	// nothing above it to be missing

		fsLocator parent = i_Locator;
		parent.Pop();
		std::error_code ignored;
		return std::filesystem::is_directory(fsPathUtil::LocatorToPath(parent), ignored);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ThrowMissing(const fsLocator& i_Locator)
{
	if (ParentDirectoryExists(i_Locator))
		throw fsFileDoesntExistX(i_Locator);
	throw fsDirectoryDoesntExistX(i_Locator);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Throw(const std::error_code& i_Error, const fsLocator& i_Locator)
{
	if (IsMissing(i_Error))
		ThrowMissing(i_Locator);
	if (i_Error == std::errc::file_exists)
		throw fsFileExistsX(i_Locator);
	if (i_Error == std::errc::directory_not_empty)
		throw fsDirectoryNotEmptyX(i_Locator);
	if (IsDenied(i_Error))
		throw fsReadOnlyX(i_Locator);
	if (IsBusy(i_Error))
		throw fsFileInUseX(i_Locator);
	if (IsDiskFull(i_Error))
		throw fsDiskFullX(i_Locator);
	if (IsInvalid(i_Error))
		throw fsInvalidLocatorX(i_Locator);
	throw fsUnknownX(i_Locator);
}

//----------------------------------------------------------------------------
//	Missing: the source, or else the destination's directory. Exists,
//	denied and disk full: the destination. Anything else: the source.
//----------------------------------------------------------------------------
void Throw(const std::error_code& i_Error, const fsLocator& i_From, const fsLocator& i_To)
{
	if (IsMissing(i_Error))
	{
		std::error_code ignored;
		if (!std::filesystem::exists(fsPathUtil::LocatorToPath(i_From), ignored))
			ThrowMissing(i_From);
		throw fsDirectoryDoesntExistX(i_To);
	}
	if (i_Error == std::errc::file_exists)
		throw fsFileExistsX(i_To);
	if (IsDenied(i_Error))
		throw fsReadOnlyX(i_To);
	if (IsBusy(i_Error))
		throw fsFileInUseX(i_From);
	if (IsDiskFull(i_Error))
		throw fsDiskFullX(i_To);
	if (IsInvalid(i_Error))
		throw fsInvalidLocatorX(i_From);
	throw fsUnknownX(i_From);
}

}
