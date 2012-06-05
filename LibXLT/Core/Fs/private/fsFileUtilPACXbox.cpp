/****************************************************************************\
**  fsFileUtilPACXbox.cpp
**
**      fsFileUtilPACXbox.cpp defines the file util PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "fsFileUtilPACXbox.hpp"

#include <stdio.h>
//#include <windows.h>
#include <xtl.h>

// Windows unfortunately defines several functions as macros to conditionally
// compile with Unicode or ANSI.  We will undefine them here and call the
// specific versions (which we have to do anyway).
#undef CreateFile
#undef DeleteFile
#undef CopyFile
#undef CreateDirectory

#include "dbgAssert.hpp"
#include "envSystemDataPACXbox.hpp"
#include "fsFileX.hpp"

namespace fsFileUtilPAC
{

namespace
{

	const char lc_HostDirSep = 47; // '/'
	const char lc_CDRomDirSep = 92; // '\'

	const int lc_MaxPath = 256;

	//	T: The title's persistent data region. 
	//	U: The title's user data region on the hard disk. 
	const itString lc_DVDString("D:");
	const itString lc_HDString("U:");

//	This is initialized in the Init() function
//	It is true if we are running on 95 or 98 and have to
//	convert our Unicode strings to ANSI strings before
//	passing them to the Win32 functions
bool l_ANSIFilenames = false;

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

		default:
			throw fsUnknownX(i_Locator);
		break;
	}
}

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

		default:
			throw fsUnknownX(i_From);
		break;
	}
}

}

//============================================================================
//	CreateFile creates a file with the given name.  This must be done before
//	reading or writing with a fsFileStream.
//============================================================================
void CreateFile(const fsLocator& i_Filename)
{
	HANDLE new_file = INVALID_HANDLE_VALUE;

	if ( l_ANSIFilenames )
	{
		std::string filename;
		LocatorToANSIFilename(i_Filename, filename);

		if( filename.size() == 0 )
			throw fsInvalidLocatorX(i_Filename);

		new_file = ::CreateFileA(	filename.c_str(),
									GENERIC_WRITE,
									0,
									NULL,
									CREATE_NEW,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;
		LocatorToUnicodeFilename(i_Filename, filename);
	
		if( filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_Filename);
	
		new_file = ::CreateFileW(	filename.GetString(),
									GENERIC_WRITE,
									0,
									NULL,
									CREATE_NEW,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
*/
	}

	if ( new_file == INVALID_HANDLE_VALUE )
		handle_windows_error(i_Filename);		// probably throws
	else
		::CloseHandle( new_file );	// release file so something else can write to it
}

//============================================================================
//	RenameFile renames a file.  This can be used to move a file to different
//	directories, etc.
//============================================================================
void RenameFile(const fsLocator& i_From, const fsLocator& i_To)
{
	BOOL ret_val = 0;
/*
	envSystemDataPAC::WindowsOS os_type = envSystemDataPAC::GetWindowsOSType();

	bool has_move_file_ex = (os_type == envSystemDataPAC::e_Windows2000) ||
							(os_type == envSystemDataPAC::e_WindowsXP);
*/
	bool has_move_file_ex = false;
	if ( l_ANSIFilenames )
	{
		std::string from_filename, to_filename;

		LocatorToANSIFilename(i_From, from_filename);
		LocatorToANSIFilename(i_To, to_filename);

		if( from_filename.size() == 0 )
			throw fsInvalidLocatorX(i_From);

		if( to_filename.size() == 0 )
			throw fsInvalidLocatorX(i_To);

		if( has_move_file_ex )
			ret_val = ::MoveFileExA(from_filename.c_str(), to_filename.c_str(), MOVEFILE_COPY_ALLOWED);
		else
			ret_val = ::MoveFileA(from_filename.c_str(), to_filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString from_filename, to_filename;

		LocatorToUnicodeFilename(i_From, from_filename);
		LocatorToUnicodeFilename(i_To, to_filename);

		if( from_filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_From);

		if( to_filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_To);

		if( has_move_file_ex )
			ret_val = ::MoveFileExW(from_filename.GetString(), to_filename.GetString(), MOVEFILE_COPY_ALLOWED);
		else
			ret_val = ::MoveFileW(from_filename.GetString(), to_filename.GetString());
*/
	}

	if( ret_val == 0 )
		handle_windows_multifile_error(i_From, i_To);
}

//============================================================================
//	DeleteFile deletes a file.  The file must exist, or an exception will be
//	thrown.
//============================================================================
void DeleteFile(const fsLocator& i_Filename)
{
	BOOL ret_val = 0;

	if ( l_ANSIFilenames )
	{
		std::string filename;

		LocatorToANSIFilename(i_Filename, filename);

		if( filename.size() == 0 )
			throw fsInvalidLocatorX(i_Filename);

		ret_val = ::DeleteFileA(filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;

		LocatorToUnicodeFilename(i_Filename, filename);

		if( filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_Filename);

		ret_val = ::DeleteFileW(filename.GetString());
*/
	}

	if( ret_val == 0 )
		handle_windows_error(i_Filename);
}

//============================================================================
//	CopyFile copies an entire file from one location to another.  Of course,
//	the file must exist.
//============================================================================
void CopyFile(const fsLocator& i_From, const fsLocator& i_To)
{
	BOOL ret_val = 0;

	if ( l_ANSIFilenames )
	{
		std::string from_filename, to_filename;

		LocatorToANSIFilename(i_From, from_filename);
		LocatorToANSIFilename(i_To, to_filename);

		if( from_filename.size() == 0 )
			throw fsInvalidLocatorX(i_From);

		if( to_filename.size() == 0 )
			throw fsInvalidLocatorX(i_To);

		ret_val = ::CopyFileA(from_filename.c_str(), to_filename.c_str(), true);
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString from_filename, to_filename;

		LocatorToUnicodeFilename(i_From, from_filename);
		LocatorToUnicodeFilename(i_To, to_filename);

		if( from_filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_From);

		if( to_filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_To);

		ret_val = ::CopyFileW(from_filename.GetString(), to_filename.GetString(), true);
*/
	}

	if( ret_val == 0 )
		handle_windows_multifile_error(i_From, i_To);
}

//============================================================================
//	CopyDirectory copies a directory with the given name and all sub-dirs.
//============================================================================
void CopyDirectory(const fsLocator& i_DirectoryName)
{
	DBG_ASSERT0( false, "please implement this function." );
}

//============================================================================
//	CreateDirectory creates a directory with the given name.
//
//============================================================================
void CreateDirectory(const fsLocator& i_DirectoryName)
{
	BOOL ret_val = 0;

	if ( l_ANSIFilenames )
	{
		std::string filename;

		LocatorToANSIFilename(i_DirectoryName, filename);

		if( filename.size() == 0 )
			throw fsInvalidLocatorX(i_DirectoryName);

		ret_val = ::CreateDirectoryA(filename.c_str(), NULL);
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;

		LocatorToUnicodeFilename(i_DirectoryName, filename);

		if( filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_DirectoryName);

		ret_val = ::CreateDirectoryW(filename.GetString(), NULL);
*/
	}

	if( ret_val == 0 )
		handle_windows_error(i_DirectoryName);
}

//============================================================================
//	DeleteDirectory destroys a directory with the given name.  The directory
//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
//============================================================================
void DeleteDirectory(const fsLocator& i_DirectoryName)
{
	BOOL ret_val = 0;

	if ( l_ANSIFilenames )
	{
		std::string filename;

		LocatorToANSIFilename(i_DirectoryName, filename);

		if( filename.size() == 0 )
			throw fsInvalidLocatorX(i_DirectoryName);

		ret_val = ::RemoveDirectoryA(filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;

		LocatorToUnicodeFilename(i_DirectoryName, filename);

		if( filename.GetLength() == 0 )
			throw fsInvalidLocatorX(i_DirectoryName);

		ret_val = ::RemoveDirectoryW(filename.GetString());
*/
	}

	if( ret_val == 0 )
		handle_windows_error(i_DirectoryName);
}

//============================================================================
//	FileExists tests if a file exists.
//
//============================================================================
bool FileExists(const fsLocator& i_Filename)
{
	DWORD ret_val = -1;

	if ( l_ANSIFilenames )
	{
		std::string filename;
		LocatorToANSIFilename(i_Filename, filename);

		if( filename.size() == 0 )
			return false;

		ret_val = ::GetFileAttributesA(filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;
		LocatorToUnicodeFilename(i_Filename, filename);

		if( filename.GetLength() == 0 )
			return false;

		ret_val = ::GetFileAttributesW(filename.GetString());
*/
	}

	if( (ret_val == -1) || (ret_val & FILE_ATTRIBUTE_DIRECTORY) )
		return false;
	else
		return true;
}

//============================================================================
//	DirectoryExists tests if a directory exists.
//
//============================================================================
bool DirectoryExists(const fsLocator& i_Filename)
{
	DWORD ret_val = -1;

	if ( l_ANSIFilenames )
	{
		std::string filename;
		LocatorToANSIFilename(i_Filename, filename);

		if( filename.size() == 0 )
			return false;

		ret_val = ::GetFileAttributesA(filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;
		LocatorToUnicodeFilename(i_Filename, filename);

		if( filename.GetLength() == 0 )
			return false;

		ret_val = ::GetFileAttributesW(filename.GetString());
*/
	}

	if( (ret_val == -1) || ((ret_val & FILE_ATTRIBUTE_DIRECTORY) == 0) )
		return false;
	else
		return true;
}

//========================================================================
// ANSIFilenameToLocator returns a locator from an ANSI pathname
//========================================================================
void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator)
{
	o_Locator.Clear();
	int k;
	int nBegin;
	itString itName(i_String.c_str());
	itString Piece;
	nBegin = 0;
	for (k = 0; k < itName.GetLength(); k++)
	{
		if (itName[k] == itString::CharType('\\'))
		{
			Piece = itString(nBegin, (k - nBegin), itName);
			nBegin = k + 1;
			o_Locator.Push(Piece);
		}
	}
	Piece = itString(nBegin, (itName.GetLength() - nBegin), itName);
	o_Locator.Push(Piece);
}
/*
//============================================================================
//	LocatorToANSIFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a ANSI win32 filename (for use in
//	Windows 95 or 98).
//============================================================================
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String)
{
	char work_string[MAX_PATH];
	int num_names = i_Locator.GetNumNames();
	int i, num_chars;

	o_String.resize(0);

	if ( num_names <= 0 )
		return;

	--num_names;

	// iterate and add each name to the path
	//
	for ( i = 0 ; i < num_names ; i++ )
	{
		const itString& cur_string = i_Locator.GetName(i);

		num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
											0,			// no "lo-performance" flags
											cur_string.GetString(),
											cur_string.GetLength(),
											work_string,
											MAX_PATH,	// size of target buffer
											NULL,
											NULL);

		// for some reason, this function doesn't seem to terminate the 
		// work_string
		work_string[num_chars] = 0;

		o_String += work_string;
		o_String += '\\';			// official directory separator character
	}

	const itString& last_string = i_Locator.GetLastName();

	num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
										0,			// no "lo-performance" flags
										last_string.GetString(),
										last_string.GetLength(),
										work_string,
										MAX_PATH,	// size of target buffer
										NULL,
										NULL);

	// for some reason, this function doesn't seem to terminate the 
	// work_string
	work_string[num_chars] = 0;

	o_String += work_string;
//	o_String += char(0);
}
*/
/*
//============================================================================
//	LocatorToANSIFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a ANSI win32 filename (for use in
//	Windows 95 or 98).
//============================================================================
void 
LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, const int i_StartNum, const int i_EndNum )
{
	char work_string[MAX_PATH];
	int num_names = i_Locator.GetNumNames();
	int i, num_chars;

	o_String.resize(0);

	if ( num_names <= 0 )
		return;

	--num_names;

	DBG_ASSERT1( i_StartNum >= 0, "Locator start index out of range (%d)", i_StartNum );
	DBG_ASSERT2( i_EndNum <= num_names, "Locator end index out of range (%d > %d)", i_EndNum, num_names );

	// iterate and add each name to the path
	//
	for ( i = i_StartNum ; i < i_EndNum ; i++ )
	{
		const itString& cur_string = i_Locator.GetName(i);

		num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
											0,			// no "lo-performance" flags
											cur_string.GetString(),
											cur_string.GetLength(),
											work_string,
											MAX_PATH,	// size of target buffer
											NULL,
											NULL);

		// for some reason, this function doesn't seem to terminate the 
		// work_string
		work_string[num_chars] = 0;

		o_String += work_string;
		o_String += '\\';			// official directory separator character
	}

	const itString& last_string = i_Locator.GetLastName();

	num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
										0,			// no "lo-performance" flags
										last_string.GetString(),
										last_string.GetLength(),
										work_string,
										MAX_PATH,	// size of target buffer
										NULL,
										NULL);

	// for some reason, this function doesn't seem to terminate the 
	// work_string
	work_string[num_chars] = 0;

	o_String += work_string;
//	o_String += char(0);
}
*/
//============================================================================
//	LocatorToANSIFilename is used internally by the PS2 PACs in the fs
//	package.  It converts a fsLocator to a ANSI filename (for use by
//	PS2 library functions).
//============================================================================
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_Directory)
{
	char work_string[lc_MaxPath];
	int num_names = i_Locator.GetNumNames();
	int i;

	if ( num_names <= 0 )
		return;
/*
 	char dir_sep;
	if( i_Locator.GetName(0) == lc_CDRomString )
		dir_sep = lc_CDRomDirSep;
	else if( i_Locator.GetName(0) == lc_HostString )
		dir_sep = lc_HostDirSep;
	else
		dir_sep = lc_HostDirSep;	//	hard disk?	
*/
 	char dir_sep = '\\';
	// iterate and add each name to the path
	//
	int num_chars = 0;
	for ( i = 0 ; i < num_names ; i++ )
	{
		const itString& cur_string = i_Locator.GetName(i);
		int cur_char;
		int cur_length = cur_string.GetLength();
		
		for( cur_char = 0 ; cur_char < cur_length ; ++cur_char )
			work_string[num_chars++] = char(cur_string[cur_char]);

		if( i < (num_names - 1) )
			work_string[num_chars++] = dir_sep;
	}
/*
	if( (dir_sep == lc_CDRomDirSep) && !i_Directory )
	{
		work_string[num_chars++] = ';';
		work_string[num_chars++] = '1';
	}
*/
	work_string[num_chars++] = 0;

	o_String = work_string;
}

//============================================================================
//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a Unicode filename (for use in
//	Windows NT or 2000).
//============================================================================
void LocatorToUnicodeFilename(const fsLocator& i_Locator, itString& o_String)
{
	int num_names = i_Locator.GetNumNames();
	int i;

	o_String.Clear();

	if ( num_names <= 0 ) return;

	--num_names;

	// iterate and add each name to the path
	//
	for ( i = 0 ; i < num_names ; i++ )
	{
		o_String += i_Locator.GetName(i);
	
		// a harmless assumption here:
		// Unicode "backslash" character is the same numeric
		// value as ANSI backslash
		o_String += itString::CharType('\\');		
	}

	o_String += i_Locator.GetLastName();
	o_String += itString::CharType(0);
}


//============================================================================
//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
//	package.  It converts a fsLocator to a Unicode filename (for use in
//	Windows NT or 2000).
//============================================================================
void 
LocatorToUnicodeFilename(const fsLocator& i_Locator, itString& o_String, const int i_StartNum, const int i_EndNum )
{
	int num_names = i_Locator.GetNumNames();
	int i;

	o_String.Clear();

	if ( num_names <= 0 ) return;

	--num_names;

	DBG_ASSERT1( i_StartNum >= 0, "Locator start index out of range (%d)", i_StartNum );
	DBG_ASSERT2( i_EndNum <= num_names, "Locator end index out of range (%d > %d)", i_EndNum, num_names );

	// iterate and add each name to the path
	//
	for ( i = i_StartNum ; i < i_EndNum ; i++ )
	{
		o_String += i_Locator.GetName(i);
	
		// a harmless assumption here:
		// Unicode "backslash" character is the same numeric
		// value as ANSI backslash
		o_String += itString::CharType('\\');		
	}

	o_String += i_Locator.GetLastName();
	o_String += itString::CharType(0);
}


//============================================================================
//	MustUseANSIFilenames() returns true for Windows OSs that don't have
//	full Unicode support.
//============================================================================
bool MustUseANSIFilenames()
{
	return l_ANSIFilenames;
}

//============================================================================
//	IsReadOnly returns true if the file is read-only, meaning that write
//	operations to the file will fail.
//============================================================================
bool IsReadOnly(const fsLocator& i_Locator)
{
	DWORD attribute = -1;

	if ( l_ANSIFilenames )
	{
		std::string filename;
		LocatorToANSIFilename(i_Locator, filename);

		if( filename.size() == 0 )
			return false;

		attribute = ::GetFileAttributesA(filename.c_str());
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
/*
		itString filename;
		LocatorToUnicodeFilename(i_Locator, filename);

		if( filename.GetLength() == 0 )
			return false;

		attribute = ::GetFileAttributesW(filename.GetString());
*/
	}

	if( (attribute == -1) || ((attribute & FILE_ATTRIBUTE_DIRECTORY) != 0) )
		throw fsFileDoesntExistX(i_Locator);

	return (attribute & FILE_ATTRIBUTE_READONLY) != 0;
}

//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
/*
	envSystemDataPAC::WindowsOS os_type = envSystemDataPAC::GetWindowsOSType();

	if (		(os_type == envSystemDataPAC::e_Windows95)
			||	(os_type == envSystemDataPAC::e_Windows98)
			||	(os_type == envSystemDataPAC::e_WindowsME) )
	{
		l_ANSIFilenames = true;
	}
	else
	{
		l_ANSIFilenames = false;
	}
*/
  	l_ANSIFilenames = true;
}

void CleanUp() throw()
{
}

}
