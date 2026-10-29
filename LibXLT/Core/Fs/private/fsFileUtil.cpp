/****************************************************************************\
**  fsFileUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/fs/fsFileUtil.hpp"

#include "Core/fs/fsFileEnum.hpp"
#include "Core/fs/private/fsFileUtilPAC.hpp"
#include "Core/fs/fsLocator.hpp"

#include <algorithm>


namespace fsFileUtil
{

//----------------------------------------------------------------------------
//	CreateFile creates a file with the given name.  This must be done before
//	reading or writing with a fsFileStream.
//----------------------------------------------------------------------------
void CreateFile(const fsLocator& i_Filename)
{
	fsFileUtilPAC::CreateFile(i_Filename);
}

//----------------------------------------------------------------------------
//	RenameFile renames a file.  This can be used to move a file to different
//	directories, etc.
//----------------------------------------------------------------------------
void RenameFile(const fsLocator& i_From, const fsLocator& i_To)
{
	fsFileUtilPAC::RenameFile(i_From, i_To);
}

//----------------------------------------------------------------------------
//	DeleteFile deletes a file.  The file must exist, or an exception will be
//	thrown.
//----------------------------------------------------------------------------
void DeleteFile(const fsLocator& i_Filename)
{
	fsFileUtilPAC::DeleteFile(i_Filename);
}

//----------------------------------------------------------------------------
//	CopyFile copies an entire file from one location to another.  Of course,
//	the file must exist.
//----------------------------------------------------------------------------
void CopyFile(const fsLocator& i_From, const fsLocator& i_To)
{
	fsFileUtilPAC::CopyFile(i_From, i_To);
}

//----------------------------------------------------------------------------
//	CreateDirectory creates a directory with the given name.
//
//----------------------------------------------------------------------------
void CreateDirectory(const fsLocator& i_DirectoryName)
{
	fsFileUtilPAC::CreateDirectory(i_DirectoryName);
}

//----------------------------------------------------------------------------
//	DeleteDirectory destroys a directory with the given name.  The directory
//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
//----------------------------------------------------------------------------
void DeleteDirectory(const fsLocator& i_DirectoryName)
{
	fsFileUtilPAC::DeleteDirectory(i_DirectoryName);
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
//	FileExists tests if a file exists.
//
//----------------------------------------------------------------------------
bool FileExists(const fsLocator& i_Filename)
{
	return fsFileUtilPAC::FileExists(i_Filename);
}

//----------------------------------------------------------------------------
//	DirectoryExists tests if a directory exists.
//
//----------------------------------------------------------------------------
bool DirectoryExists(const fsLocator& i_Filename)
{
	return fsFileUtilPAC::DirectoryExists(i_Filename);
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
// ANSIFilenameToLocator returns a locator from an ANSI pathname
//----------------------------------------------------------------------------
void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator)
{
	if ( 0 == i_String.size() )
	{
		// nothing to do, it's not a filename, it is empty!
		o_Locator.Clear();
		return;
	}

	fsFileUtilPAC::ANSIFilenameToLocator(i_String, o_Locator);
}

//----------------------------------------------------------------------------
//	LocatorToANSIFilename converts a fsLocator to a narrow (UTF-8) filename
//	string, for APIs that take char filenames.  It is provided here to assist in error reporting.
//----------------------------------------------------------------------------
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_bUseDoubleSlashes)
{
	fsFileUtilPAC::LocatorToANSIFilename(i_Locator, o_String, i_bUseDoubleSlashes);
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
//----------------------------------------------------------------------------
void GenerateFileName(const fsLocator& i_PathLocator, std::string& i_Prefix, std::string& i_Extension, std::string& o_Name)
{
	fsFileUtilPAC::GenerateFileName(i_PathLocator, i_Prefix, i_Extension, o_Name);
}


//----------------------------------------------------------------------------
//	IsReadOnly returns true if the file is read-only, meaning that write
//	operations to the file will fail.
//----------------------------------------------------------------------------
bool IsReadOnly(const fsLocator& i_Locator)
{
	return fsFileUtilPAC::IsReadOnly(i_Locator);
}

//----------------------------------------------------------------------------
//	SetReadOnly() - sets the file to be read only if true is passed in.
//----------------------------------------------------------------------------
void SetReadOnly( const fsLocator& i_Locator, bool i_bSetReadOnly )
{
	fsFileUtilPAC::SetReadOnly( i_Locator, i_bSetReadOnly );
}

//=============================================================================
//  Check whether filename is absolute or not
//=============================================================================
bool IsFileNameAbsolute( const fsLocator & i_Loc )
{
	return fsFileUtilPAC::IsFileNameAbsolute( i_Loc );
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void Init()
{
	fsFileUtilPAC::Init();
}

void CleanUp() throw()
{
	fsFileUtilPAC::CleanUp();
}

}
