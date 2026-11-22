/****************************************************************************\
**  fsFileUtil.cpp
**
**      see .hpp
**
**	The file operations use std::filesystem (and C's fopen for CreateFile's
**	exclusive create), so they are the same on every platform and fsFileUtil
**	has no PAC. Where std::filesystem differs from what fsFileUtil promises (it
**	replaces an existing file on rename and deletes read-only files), the
**	difference is checked for here first.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/fs/fsFileUtil.hpp"

#ifndef FS_FILEENUM_HPP
#include "Core/fs/fsFileEnum.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif
#ifndef FS_FILEERRORUTIL_HPP
#include "Core/fs/private/fsFileErrorUtil.hpp"
#endif
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <filesystem>
#include <string>
#include <system_error>


namespace fsFileUtil
{

namespace
{
	namespace sfs = std::filesystem;

	//	The status of a path, file_type::not_found when it can't be read
	sfs::file_status Status(const sfs::path& i_Path)
	{
		std::error_code ignored;
		sfs::file_status status = sfs::status(i_Path, ignored);
		if (status.type() == sfs::file_type::none)
			return sfs::file_status(sfs::file_type::not_found);
		return status;
	}

	//	Read-only is the owner's write permission (the read-only attribute on
	//	Windows, where std::filesystem maps one to the other)
	bool IsReadOnlyStatus(const sfs::file_status& i_Status)
	{
		return (i_Status.permissions() & sfs::perms::owner_write) == sfs::perms::none;
	}

	//	The path to give the file system, or fsInvalidLocatorX for an empty
	//	locator
	sfs::path PathOrThrow(const fsLocator& i_Locator)
	{
		sfs::path path = fsPathUtil::LocatorToPath(i_Locator);
		if (path.empty())
			throw fsInvalidLocatorX(i_Locator);
		return path;
	}
}

//----------------------------------------------------------------------------
//	CreateFile creates an empty file with the given name, failing with
//	fsFileExistsX if it exists.  This must be done before reading or
//	writing with a fsFileStream.
//
//	fopen's "x" (C11) creates the file only if it doesn't exist, in one step,
//	so nothing can create it between a check and the create. Narrow file
//	names are UTF-8 (on Windows through the UTF-8 process code page that
//	Utf8CodePage.manifest gives every exe; fsNarrowPathTest checks it).
//----------------------------------------------------------------------------
void CreateFile(const fsLocator& i_Filename)
{
	const sfs::path path = PathOrThrow(i_Filename);

	errno = 0;
	std::FILE* file = std::fopen(fsPathUtil::PathToUtf8(path).c_str(), "wx");
	if (!file)
	{
		const int error = errno != 0 ? errno : EIO;
		fsFileErrorUtil::Throw(std::error_code(error, std::generic_category()), i_Filename);
	}
	std::fclose(file);
}

//----------------------------------------------------------------------------
//	RenameFile renames a file.  This can be used to move a file to different
//	directories, etc.
//----------------------------------------------------------------------------
void RenameFile(const fsLocator& i_From, const fsLocator& i_To)
{
	const sfs::path from = PathOrThrow(i_From);
	const sfs::path to = PathOrThrow(i_To);

	if (!sfs::exists(Status(from)))
		fsFileErrorUtil::ThrowMissing(i_From);

	// std::filesystem::rename replaces an existing file; fsFileUtil never
	// has. A name that differs only in case is the same file on Windows,
	// and renaming it to that name is allowed.
	std::error_code error;
	if (sfs::exists(Status(to)) && !sfs::equivalent(from, to, error))
		throw fsFileExistsX(i_To);

	sfs::rename(from, to, error);

	// Across volumes: Windows moves the file itself (MoveFileEx with
	// MOVEFILE_COPY_ALLOWED); elsewhere rename fails, so copy then delete
	if (error == std::errc::cross_device_link && sfs::is_regular_file(Status(from)))
	{
		error.clear();
		if (sfs::copy_file(from, to, sfs::copy_options::none, error))
			sfs::remove(from, error);
	}

	if (error)
		fsFileErrorUtil::Throw(error, i_From, i_To);
}

//----------------------------------------------------------------------------
//	DeleteFile deletes a file.  The file must exist, or an exception will be
//	thrown.
//----------------------------------------------------------------------------
void DeleteFile(const fsLocator& i_Filename)
{
	const sfs::path path = PathOrThrow(i_Filename);
	const sfs::file_status status = Status(path);

	if (!sfs::exists(status))
		fsFileErrorUtil::ThrowMissing(i_Filename);

	// std::filesystem::remove also removes an empty directory, and a
	// read-only file; fsFileUtil refuses both
	if (sfs::is_directory(status) || IsReadOnlyStatus(status))
		throw fsReadOnlyX(i_Filename);

	std::error_code error;
	sfs::remove(path, error);
	if (error)
		fsFileErrorUtil::Throw(error, i_Filename);
}

//----------------------------------------------------------------------------
//	CopyFile copies an entire file from one location to another.  Of course,
//	the file must exist.
//----------------------------------------------------------------------------
void CopyFile(const fsLocator& i_From, const fsLocator& i_To)
{
	const sfs::path from = PathOrThrow(i_From);
	const sfs::path to = PathOrThrow(i_To);

	std::error_code error;
	sfs::copy_file(from, to, sfs::copy_options::none, error);
	if (error)
		fsFileErrorUtil::Throw(error, i_From, i_To);
}

//----------------------------------------------------------------------------
//	CreateDirectory creates a directory with the given name, and any
//	directories above it that are missing.
//----------------------------------------------------------------------------
void CreateDirectory(const fsLocator& i_DirectoryName)
{
	const sfs::path path = fsPathUtil::LocatorToPath(i_DirectoryName);
	if (path.empty())
		throw fsDirectoryDoesntExistX(i_DirectoryName);

	std::error_code error;
	sfs::create_directories(path, error);
	if (!error)
		return;

	// A missing drive or share: the place for the directory doesn't exist
	if (error == std::errc::no_such_file_or_directory)
		throw fsDirectoryDoesntExistX(i_DirectoryName);
	fsFileErrorUtil::Throw(error, i_DirectoryName);
}

//----------------------------------------------------------------------------
//	DeleteDirectory destroys a directory with the given name.  The directory
//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
//----------------------------------------------------------------------------
void DeleteDirectory(const fsLocator& i_DirectoryName)
{
	const sfs::path path = PathOrThrow(i_DirectoryName);
	const sfs::file_status status = Status(path);

	if (!sfs::exists(status))
		fsFileErrorUtil::ThrowMissing(i_DirectoryName);

	// std::filesystem::remove would delete a file
	if (!sfs::is_directory(status))
		throw fsDirectoryDoesntExistX(i_DirectoryName);

	std::error_code error;
	sfs::remove(path, error);
	if (error)
		fsFileErrorUtil::Throw(error, i_DirectoryName);
}

//----------------------------------------------------------------------------
//	DeleteDirectoryRecursively deletes the contents of the directory.
//----------------------------------------------------------------------------
bool DeleteDirectoryRecursively(const fsLocator& i_DirectoryName)
{
	if (!DirectoryExists(i_DirectoryName))
	{
		return false;
	}

	int i;
	fsLocator FilesToDelete;
	fsFileEnum::fsFileList Files;
	fsFileEnum::EnumerateFiles(i_DirectoryName, Files);
	for (i = 0; i < Files.size(); ++i)
	{
		FilesToDelete.Clear();
		FilesToDelete = i_DirectoryName;
		FilesToDelete.Push(Files[i].GetLastName());
		if(IsReadOnly(FilesToDelete))
			return false;
		else
			DeleteFile(FilesToDelete);
	}
	return true;
}
//----------------------------------------------------------------------------
//	CopyDirectory recursively copies the contents of one directory to another.
//	Of course, the from directory must exist
//----------------------------------------------------------------------------
void CopyDirectory(const fsLocator& i_From, const fsLocator& i_To)
{
	if (!DirectoryExists(i_To))
	{
		CreateDirectory(i_To);
	}

	int i;
	fsLocator SubFrom, SubTo;
	fsFileEnum::fsFileList Directories, Files;
	fsFileEnum::EnumerateDirectories(i_From, Directories);
	for (i = 0; i < Directories.size(); ++i)
	{
		//recursively copy subdirectories, skipping "." and ".."
		if (Directories[i].GetLastName() == "." || Directories[i].GetLastName() == "..")
		{
			continue;
		}

		SubFrom.Clear();
		SubTo.Clear();
		SubFrom = i_From;
		SubTo = i_To;
		SubFrom.Push(Directories[i].GetLastName());
		SubTo.Push(Directories[i].GetLastName());
		CopyDirectory(SubFrom, SubTo);
	}

	fsFileEnum::EnumerateFiles(i_From, Files);
	for (i = 0; i < Files.size(); ++i)
	{
		SubFrom.Clear();
		SubTo.Clear();
		SubFrom = i_From;
		SubTo = i_To;
		SubFrom.Push(Files[i].GetLastName());
		SubTo.Push(Files[i].GetLastName());
		CopyFile(SubFrom, SubTo);
	}

}

//----------------------------------------------------------------------------
//	FileExists tests if a file exists.  Anything that isn't a directory
//	counts as a file.
//----------------------------------------------------------------------------
bool FileExists(const fsLocator& i_Filename)
{
	const sfs::path path = fsPathUtil::LocatorToPath(i_Filename);
	if (path.empty())
		return false;

	const sfs::file_status status = Status(path);
	return sfs::exists(status) && !sfs::is_directory(status);
}

//----------------------------------------------------------------------------
//	DirectoryExists tests if a directory exists.
//
//----------------------------------------------------------------------------
bool DirectoryExists(const fsLocator& i_Filename)
{
	const sfs::path path = fsPathUtil::LocatorToPath(i_Filename);
	if (path.empty())
		return false;

	return sfs::is_directory(Status(path));
}

//----------------------------------------------------------------------------
// UnicodeStringToLocator returns a locator from a given UTF-8 string
// the string is separted by backslashes in the windows style ,this string
// should not be used for pathing, but rather for logging or perhaps
// storing a locator as a fully pathed string. NOTE: the index will be ignored,
// be sure to call gfFileTranslationMgr::ExpandLocator first if you need to
//----------------------------------------------------------------------------
void UnicodeStringToLocator(const std::string& i_String, fsLocator& o_Locator)
{
	ANSIFilenameToLocator(i_String, o_Locator);
}

//----------------------------------------------------------------------------
// LocatorToUnicodeString returns a UTF-8 string from a given locator
// the string is separted by backslashes in the windows style ,this string
// should not be used for pathing, but rather for logging or perhaps
// storing a locator as a fully pathed string. NOTE: the index will be ignored,
// be sure to call gfFileTranslationMgr::ExpandLocator first if you need to
//----------------------------------------------------------------------------
void LocatorToUnicodeString(const fsLocator& i_Locator, std::string& o_String)
{
	LocatorToANSIFilename(i_Locator, o_String);
}

//----------------------------------------------------------------------------
//	ANSIFilenameToLocator returns a locator from a UTF-8 path name. Either
//	slash separates names; a name can't be empty, so doubled separators
//	count as one, except that a leading separator marks a UNC path.
//----------------------------------------------------------------------------
void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator)
{
	o_Locator.Clear();

	// The separators are ASCII, so they never occur inside a UTF-8 sequence
	std::string::size_type begin = 0;
	for (std::string::size_type k = 0; k <= i_String.size(); k++)
	{
		if (k == i_String.size() || i_String[k] == '\\' || i_String[k] == '/')
		{
			if (k > begin)
			{
				o_Locator.Push(i_String.substr(begin, k - begin));
			}
			else if (k == 0 && k < i_String.size())
			{
				//	check for UNC Path
				o_Locator.SetUNCPath(true);
			}
			begin = k + 1;
		}
	}
}

//----------------------------------------------------------------------------
//	LocatorToANSIFilename converts a fsLocator to a UTF-8 path name with
//	backslash separators, on every platform: this is the form locators are
//	saved in. Use fsPathUtil::LocatorToPath for a name to open.
//----------------------------------------------------------------------------
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_bUseDoubleSlashes)
{
	const int num_names = i_Locator.GetNumNames();

	o_String.clear();

	if ( num_names <= 0 )
		return;

	//	if UNC Path then prepend \\ at the beginning
	//
	if (i_Locator.IsUNCPath())
	{
		o_String += "\\\\";
	}

	for (int i = 0 ; i < num_names ; i++ )
	{
		if (i > 0)
		{
			o_String += '\\';			// official directory separator character

			//add a second slash seperator - this is a fix for appending
			//"My Documents" directory to python sys.path
			if(i_bUseDoubleSlashes)
				o_String += '\\';
		}
		o_String += i_Locator.GetName(i);
	}
}

//----------------------------------------------------------------------------
// Return true if every path in the filename is an ascii string
//----------------------------------------------------------------------------
bool IsFilenameASCII(const fsLocator& i_Locator)
{
	for( int i = 0; i < i_Locator.GetNumNames(); ++i )
	{
		const std::string& cur_name = i_Locator.GetName(i);
		if( std::any_of(cur_name.begin(), cur_name.end(), [](char c) { return (unsigned char)(c) > 127; }) )
			return false;
	}
	return true;
}

//----------------------------------------------------------------------------
//	Create a filename string that is unique within the given pathlocator,
//	using the given prefix and extension: prefix + extension, else prefix,
//	a number counting from 0, and extension.
//----------------------------------------------------------------------------
void GenerateFileName(const fsLocator& i_PathLocator, std::string& i_Prefix, std::string& i_Extension, std::string& o_Name)
{
	std::string name = i_Prefix + i_Extension;
	fsLocator loc = i_PathLocator;
	loc.Push(name);

	int index = 0;
	while (FileExists(loc))
	{
		name = i_Prefix + std::to_string(index++) + i_Extension;
		loc.ReplaceLastName(name);
	}
	o_Name = name;
}

//----------------------------------------------------------------------------
//	Check whether filename is absolute: a UNC path, or one that starts with
//	a drive ("C:").
//----------------------------------------------------------------------------
bool IsFileNameAbsolute( const fsLocator & i_Loc )
{
	if ( i_Loc.IsUNCPath() )
	{
		return true;
	}

	if( i_Loc.GetNumNames() > 0)
	{
		const std::string& sRootDir =  i_Loc.GetName( 0 );

		if( sRootDir.size() >= 2  &&   sRootDir[1] == ':' )
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//	IsReadOnly returns true if the file is read-only, meaning that write
//	operations to the file will fail.
//----------------------------------------------------------------------------
bool IsReadOnly(const fsLocator& i_Locator)
{
	const sfs::path path = fsPathUtil::LocatorToPath(i_Locator);
	if (path.empty())
		return false;

	const sfs::file_status status = Status(path);
	if (!sfs::exists(status) || sfs::is_directory(status))
		throw fsFileDoesntExistX(i_Locator);

	return IsReadOnlyStatus(status);
}

//----------------------------------------------------------------------------
//	SetReadOnly() - sets the file to be read only if true is passed in.
//	Setting it removes every write permission; clearing it gives the owner
//	write permission back (on Windows, either changes the read-only
//	attribute).
//----------------------------------------------------------------------------
void SetReadOnly( const fsLocator& i_Locator, bool i_bSetReadOnly )
{
	if ( !FileExists( i_Locator ) )
		return;

	const sfs::path path = fsPathUtil::LocatorToPath(i_Locator);
	std::error_code ignored;	// as before, failing to change it isn't reported
	if (i_bSetReadOnly)
	{
		const sfs::perms all_write = sfs::perms::owner_write | sfs::perms::group_write | sfs::perms::others_write;
		sfs::permissions(path, all_write, sfs::perm_options::remove, ignored);
	}
	else
	{
		sfs::permissions(path, sfs::perms::owner_write, sfs::perm_options::add, ignored);
	}
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void Init()
{
}

void CleanUp() throw()
{
}

}
