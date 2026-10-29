/****************************************************************************\
**  fsFileUtilPACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/private/fsFileUtilPACWin.hpp"

#include <stdio.h>
#include <windows.h>

// Windows unfortunately defines several functions as macros to conditionally
// compile with Unicode or ANSI.  We will undefine them here and call the
// specific versions (which we have to do anyway).
#undef CreateFile
#undef DeleteFile
#undef CopyFile
#undef CreateDirectory

#include "Core/dbg/dbgMsg.hpp"
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#include "Core/env/private/envSystemDataPACWin.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"


//============================================================================
//============================================================================
namespace fsFileUtilPAC
{

namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void handle_windows_error(const fsLocator& i_Locator)
{
	DWORD error = ::GetLastError();

	switch( error )
	{
		case ERROR_FILE_NOT_FOUND:
			throw fsFileDoesntExistX(i_Locator);
		break;

		case ERROR_PATH_NOT_FOUND:
			throw fsDirectoryDoesntExistX(i_Locator);
		break;

		case ERROR_INVALID_DRIVE:
			throw fsInvalidLocatorX(i_Locator);
		break;

		case ERROR_SHARING_VIOLATION:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_LOCK_VIOLATION:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_FILE_EXISTS:
			throw fsFileExistsX(i_Locator);
		break;

		case ERROR_DRIVE_LOCKED:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_INVALID_NAME:
			throw fsInvalidLocatorX(i_Locator);
		break;

		case ERROR_DIR_NOT_EMPTY:
			throw fsDirectoryNotEmptyX(i_Locator);
		break;

		case ERROR_PATH_BUSY:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_ACCESS_DENIED:
			throw fsReadOnlyX(i_Locator);
		break;

		case ERROR_DISK_CORRUPT:
		case ERROR_DISK_FULL:
			throw fsDiskFullX(i_Locator);
		break;

		default:
			throw fsUnknownX(i_Locator);
		break;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void handle_windows_multifile_error(const fsLocator& i_From, const fsLocator& i_To)
{
	DWORD error = ::GetLastError();

	switch( error )
	{
		case ERROR_FILE_NOT_FOUND:
			throw fsFileDoesntExistX(i_From);
		break;

		case ERROR_PATH_NOT_FOUND:
			throw fsDirectoryDoesntExistX(i_From);
		break;

		case ERROR_INVALID_DRIVE:
			throw fsInvalidLocatorX(i_From);
		break;

		case ERROR_SHARING_VIOLATION:
			throw fsFileInUseX(i_From);
		break;

		case ERROR_LOCK_VIOLATION:
			throw fsFileInUseX(i_From);
		break;

		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			throw fsFileExistsX(i_To);
		break;

		case ERROR_DRIVE_LOCKED:
			throw fsFileInUseX(i_From);
		break;

		case ERROR_INVALID_NAME:
			throw fsInvalidLocatorX(i_From);
		break;

		case ERROR_PATH_BUSY:
			throw fsFileInUseX(i_From);
		break;

		case ERROR_ACCESS_DENIED:
			throw fsReadOnlyX(i_To);
		break;

		case ERROR_DISK_CORRUPT:
		case ERROR_DISK_FULL:
			throw fsDiskFullX(i_To);
		break;

		default:
			throw fsUnknownX(i_From);
		break;
	}
}

}


//----------------------------------------------------------------------------
//	CreateFile creates a file with the given name.  This must be done before
//	reading or writing with a fsFileStream.
//----------------------------------------------------------------------------
void CreateFile(const fsLocator& i_Filename)
{
	HANDLE new_file = INVALID_HANDLE_VALUE;

	//DBG_LOG("Creating/Opening UNICODE file  " << i_Filename);

	std::wstring filename;
	LocatorToUnicodeFilename(i_Filename, filename);

	if( filename.empty() )
		throw fsInvalidLocatorX(i_Filename);

	new_file = ::CreateFileW(	filename.c_str(),
								GENERIC_WRITE,
								0,
								NULL,
								CREATE_NEW,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

	if ( new_file == INVALID_HANDLE_VALUE )
		handle_windows_error(i_Filename);		// probably throws
	else
		::CloseHandle( new_file );				// release file so something else can write to it
}

//----------------------------------------------------------------------------
//	RenameFile renames a file.  This can be used to move a file to different
//	directories, etc.
//----------------------------------------------------------------------------
void RenameFile(const fsLocator& i_From, const fsLocator& i_To)
{
	BOOL ret_val = 0;

	std::wstring from_filename, to_filename;

	LocatorToUnicodeFilename(i_From, from_filename);
	LocatorToUnicodeFilename(i_To, to_filename);

	if( from_filename.empty() )
		throw fsInvalidLocatorX(i_From);

	if( to_filename.empty() )
		throw fsInvalidLocatorX(i_To);

	ret_val = ::MoveFileExW(from_filename.c_str(), to_filename.c_str(), MOVEFILE_COPY_ALLOWED);

	if( ret_val == 0 )
		handle_windows_multifile_error(i_From, i_To);
}

//----------------------------------------------------------------------------
//	DeleteFile deletes a file.  The file must exist, or an exception will be
//	thrown.
//----------------------------------------------------------------------------
void DeleteFile(const fsLocator& i_Filename)
{
	BOOL ret_val = 0;

	std::wstring filename;

	LocatorToUnicodeFilename(i_Filename, filename);

	if( filename.empty() )
		throw fsInvalidLocatorX(i_Filename);

	ret_val = ::DeleteFileW(filename.c_str());

	if( ret_val == 0 )
		handle_windows_error(i_Filename);
}

//----------------------------------------------------------------------------
//	CopyFile copies an entire file from one location to another.  Of course,
//	the file must exist.
//----------------------------------------------------------------------------
void CopyFile(const fsLocator& i_From, const fsLocator& i_To)
{
	BOOL ret_val = 0;

	std::wstring from_filename, to_filename;

	LocatorToUnicodeFilename(i_From, from_filename);
	LocatorToUnicodeFilename(i_To, to_filename);

	if( from_filename.empty() )
		throw fsInvalidLocatorX(i_From);

	if( to_filename.empty() )
		throw fsInvalidLocatorX(i_To);

	ret_val = ::CopyFileW(from_filename.c_str(), to_filename.c_str(), true);

	if( ret_val == 0 )
		handle_windows_multifile_error(i_From, i_To);
}

//----------------------------------------------------------------------------
//	CopyDirectory copies a directory with the given name and all sub-dirs.
//----------------------------------------------------------------------------
void CopyDirectory(const fsLocator& i_DirectoryName)
{
	DBG_ASSERT( false, "please implement this function." );
}

//----------------------------------------------------------------------------
//	CreateDirectory creates a directory with the given name.
//----------------------------------------------------------------------------
void CreateDirectoryOneDir(const fsLocator& i_DirectoryName)
{
	BOOL ret_val = 0;

	//DBG_LOG("Create One Dir= " << i_DirectoryName);

	std::string utf8_filename;
	fsFileUtil::LocatorToANSIFilename(i_DirectoryName, utf8_filename);
	envString::TrimSpaces(utf8_filename);
	const std::wstring filename = envString::UTF8ToWideChar(utf8_filename);

	if( filename.empty() )
		throw fsInvalidLocatorX(i_DirectoryName);

	ret_val = ::CreateDirectoryW(filename.c_str(), NULL);

	if( ret_val == 0 )
		handle_windows_error(i_DirectoryName);
}

//----------------------------------------------------------------------------
//	CreateDirectory creates a directory with the given name.
//----------------------------------------------------------------------------
void CreateDirectoryRecursively(const fsLocator& i_DirectoryName)
{
	if (!DirectoryExists(i_DirectoryName))
	{
		if( i_DirectoryName.GetNumNames() == 0 )
			throw fsDirectoryDoesntExistX(i_DirectoryName);

		fsLocator newdir(i_DirectoryName);
		std::string lastdir = newdir.GetLastName();
		newdir.Pop();

		CreateDirectoryRecursively(newdir);

		newdir.Push(lastdir);
		CreateDirectoryOneDir(newdir);
	}
}

//----------------------------------------------------------------------------
//	CreateDirectory creates a directory with the given name.
//----------------------------------------------------------------------------
void CreateDirectory(const fsLocator& i_DirectoryName)
{
	CreateDirectoryRecursively( i_DirectoryName );
}

//----------------------------------------------------------------------------
//	DeleteDirectory destroys a directory with the given name.  The directory
//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
//----------------------------------------------------------------------------
void DeleteDirectory(const fsLocator& i_DirectoryName)
{
	BOOL ret_val = 0;

	std::wstring filename;

	LocatorToUnicodeFilename(i_DirectoryName, filename);

	if( filename.empty() )
		throw fsInvalidLocatorX(i_DirectoryName);

	ret_val = ::RemoveDirectoryW(filename.c_str());

	if( ret_val == 0 )
		handle_windows_error(i_DirectoryName);
}

//----------------------------------------------------------------------------
//	FileExists tests if a file exists.
//
//----------------------------------------------------------------------------
bool FileExists(const fsLocator& i_Filename)
{
	DWORD ret_val = -1;

	std::wstring filename;
	LocatorToUnicodeFilename(i_Filename, filename);

	if( filename.empty() )
		return false;

	ret_val = ::GetFileAttributesW(filename.c_str());

	if( (ret_val == -1) || (ret_val & FILE_ATTRIBUTE_DIRECTORY) )
		return false;
	else
		return true;
}

//----------------------------------------------------------------------------
//	DirectoryExists tests if a directory exists.
//
//----------------------------------------------------------------------------
bool DirectoryExists(const fsLocator& i_Filename)
{
	DWORD ret_val = -1;

	std::wstring filename;
	LocatorToUnicodeFilename(i_Filename, filename);

	if( filename.empty() )
		return false;

	ret_val = ::GetFileAttributesW(filename.c_str());

	if( (ret_val == -1) || ((ret_val & FILE_ATTRIBUTE_DIRECTORY) == 0) )
		return false;
	else
		return true;
}

//------------------------------------------------------------------------
// ANSIFilenameToLocator returns a locator from an ANSI pathname
// API Change: This now interprets the std::string as UTF8
//------------------------------------------------------------------------
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
//	LocatorToANSIFilename converts a fsLocator to a narrow (UTF-8) filename
//	string, for APIs that take char filenames.
// API Change: This now returns the std::string as UTF8.
//----------------------------------------------------------------------------
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_bUseDoubleSlashes)
{
	std::string work_string;
	int num_names = i_Locator.GetNumNames();

	o_String.clear();

	if ( num_names <= 0 )
		return;

	--num_names;

	//	if UNC Path then prepend \\ at the beginning
	//
	if (i_Locator.IsUNCPath())
	{
		o_String += '\\';
		o_String += '\\';
	}

	// iterate and add each name to the path
	//
	for (int i = 0 ; i < num_names ; i++ )
	{
		work_string = i_Locator.GetName(i);

		o_String += work_string;
		o_String += '\\';			// official directory separator character

		//add a second slash seperator - this is a fix for appending 
		//"My Documents" directory to python sys.path
		if(i_bUseDoubleSlashes)
			o_String += '\\';
	}

	work_string = i_Locator.GetLastName();

	o_String += work_string;

//	o_String += char(0);
}


//----------------------------------------------------------------------------
//	LocatorToANSIFilename converts a fsLocator to a narrow (UTF-8) filename
//	string, for APIs that take char filenames.
// API Change: This now returns the std::string as UTF8.
//----------------------------------------------------------------------------
void 
LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, const int i_StartNum, const int i_EndNum )
{
	int num_names = i_Locator.GetNumNames();

	o_String.clear();

	if ( num_names <= 0 )
		return;

	--num_names;

	DBG_ASSERT( i_StartNum >= 0, "Locator start index out of range - " << i_StartNum );
	DBG_ASSERT( i_EndNum <= num_names, "Locator end index out of range - " << i_EndNum << " > " << num_names );

	//	if UNC Path then prepend \\ at the beginning
	//
	if (i_Locator.IsUNCPath() && i_StartNum == 0)
	{
		o_String += '\\';
		o_String += '\\';
	}

	// iterate and add each name to the path
	//
	for (int i = i_StartNum ; i < i_EndNum ; i++ )
	{
		const std::string& work_string = i_Locator.GetName(i);

		o_String += work_string;
		o_String += '\\';			// official directory separator character
	}

	const std::string& work_string = i_Locator.GetLastName();

	o_String += work_string;
//	o_String += char(0);
}

//----------------------------------------------------------------------------
//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a Unicode filename (for use in
//	Windows NT or 2000).
//----------------------------------------------------------------------------
void LocatorToUnicodeFilename(const fsLocator& i_Locator, std::wstring& o_String)
{
	int num_names = i_Locator.GetNumNames();
	int i;

	o_String.clear();

	if ( num_names <= 0 ) return;

	--num_names;

	//	if UNC Path then prepend \\ at the beginning
	//
	if (i_Locator.IsUNCPath())
	{
		o_String += '\\';
		o_String += '\\';
	}

	// iterate and add each name to the path
	//
	for ( i = 0 ; i < num_names ; i++ )
	{
		o_String += envString::UTF8ToWideChar(i_Locator.GetName(i));
		o_String += L'\\';
	}

	o_String += envString::UTF8ToWideChar(i_Locator.GetLastName());
}


//----------------------------------------------------------------------------
//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a Unicode filename (for use in
//	Windows NT or 2000).
//----------------------------------------------------------------------------
void 
LocatorToUnicodeFilename(const fsLocator& i_Locator, std::wstring& o_String, const int i_StartNum, const int i_EndNum )
{
	int num_names = i_Locator.GetNumNames();
	int i;

	o_String.clear();

	if ( num_names <= 0 ) return;

	--num_names;

	DBG_ASSERT( i_StartNum >= 0, "Locator start index out of range - " << i_StartNum );
	DBG_ASSERT( i_EndNum <= num_names, "Locator end index out of range - " << i_EndNum << " > " << num_names );

	//	if UNC Path then prepend \\ at the beginning
	//
	if (i_Locator.IsUNCPath() && i_StartNum == 0)
	{
		o_String += '\\';
		o_String += '\\';
	}

	// iterate and add each name to the path
	//
	for ( i = i_StartNum ; i < i_EndNum ; i++ )
	{
		o_String += envString::UTF8ToWideChar(i_Locator.GetName(i));
		o_String += L'\\';
	}

	//bga - This GetLastName() call is actually false, it should be the "EndNum" which makes me think that this
	// function isn't used anywhere. I am tempted to comment it out completely to see. Fewer functions are
	// easier to maintain.
	//o_String += i_Locator.GetLastName();
	o_String += envString::UTF8ToWideChar(i_Locator.GetName(i_EndNum));
}


//=============================================================================
//  Check whether filename is absolute or not
//=============================================================================
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
	DWORD attribute = -1;

	std::wstring filename;
	LocatorToUnicodeFilename(i_Locator, filename);

	if( filename.empty() )
		return false;

	attribute = ::GetFileAttributesW(filename.c_str());

	if( (attribute == -1) || ((attribute & FILE_ATTRIBUTE_DIRECTORY) != 0) )
		throw fsFileDoesntExistX(i_Locator);

	return (attribute & FILE_ATTRIBUTE_READONLY) != 0;
}

//----------------------------------------------------------------------------
//	SetReadOnly() - sets the file to be read only if true is passed in.
//----------------------------------------------------------------------------
void SetReadOnly( const fsLocator& i_Locator, bool i_bSetReadOnly )
{
	if ( !FileExists( i_Locator ) )
		return;

	//	create the filename string
	std::wstring fileName;
	LocatorToUnicodeFilename( i_Locator, fileName );

	//	check the attributes and determine what to set (if anything)
	//
	DWORD attribs = GetFileAttributesW( fileName.c_str() );
	if ( i_bSetReadOnly )
	{
		if (!(attribs & FILE_ATTRIBUTE_READONLY)) 
		{ 
			SetFileAttributesW( fileName.c_str(), attribs | FILE_ATTRIBUTE_READONLY); 
		} 
	}
	else
	{
		if ( (attribs & FILE_ATTRIBUTE_READONLY) )
		{ 
			SetFileAttributesW( fileName.c_str(), attribs ^ FILE_ATTRIBUTE_READONLY); 
		} 
	}
}

//----------------------------------------------------------------------------
//	Create a filename string that is unique within the given pathlocator,
//	using the given prefix and extension.
//----------------------------------------------------------------------------
void GenerateFileName(const fsLocator& i_PathLocator, std::string& i_Prefix, std::string& i_Extension, std::string& o_Name)
{
	// maybe some asserts for valid inputs?
	// extension ends in .???
	// prefix not empty

	std::string sPath;
	fsLocator loc;

	// build filename into loc.
	loc = i_PathLocator;

	// initally, try for just prefix+extension.
	std::string name = i_Prefix;
	name.append(i_Extension);
	loc.Push(name.c_str());

	int index = 0;
	while (FileExists(loc)) 
	{
		loc.Pop();

		name = i_Prefix;
		char sIndex[10];// 9 digits better be enough.
		_itoa(index++, sIndex, 10);
		name.append(sIndex);
		name.append(i_Extension);
		loc.Push(name.c_str());

	}
	// isn't name the name?
	o_Name = name;
	// strip off path portion; +1 for slash
//	LocatorToANSIFilename(loc, sPath);
//	std::string filePath;
//	LocatorToANSIFilename(i_PathLocator, filePath);
//	o_Name = sPath.substr(filePath.length()+1, sPath.length()-(filePath.length()+1));
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void Init()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CleanUp() throw()
{
}


}
