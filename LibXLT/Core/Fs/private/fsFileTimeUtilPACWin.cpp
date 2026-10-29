/****************************************************************************\
**  fsFileTimeUtilPACWin.cpp
**
**	  fsFileTimeUtilPACWin.cpp defines the file util PAC for windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/fs/private/fsFileTimeUtilPACWin.hpp"

#include "Core/fs/private/fsFileUtilPACWin.hpp"

#include <stdio.h>
#include <sys/stat.h>
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
#include "Core/fs/fsFileX.hpp"

#include <iomanip>
#include <sstream>

//============================================================================
//============================================================================
namespace fsFileTimeUtilPAC
{

namespace
{
//============================================================================
//============================================================================
struct FileCloser
{
	FileCloser(HANDLE i_Handle) : m_Handle(i_Handle) {}
	~FileCloser() { ::CloseHandle(m_Handle); }
	HANDLE m_Handle;
};


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

		default:
			throw fsUnknownX(i_Locator);
		break;
	}
}

//----------------------------------------------------------------------------
//	GetHandle creates a file with the given name.  This must be done before
//	reading or writing with a fsFileStream.
//----------------------------------------------------------------------------
HANDLE GetHandle(const fsLocator& i_Filename)
{
	HANDLE new_file = INVALID_HANDLE_VALUE;

	std::wstring filename;
	fsFileUtilPAC::LocatorToUnicodeFilename(i_Filename, filename);

	if( filename.empty() )
		throw fsInvalidLocatorX(i_Filename);

	new_file = ::CreateFileW(	filename.c_str(),
								GENERIC_READ,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

	if ( new_file == INVALID_HANDLE_VALUE )
		handle_windows_error(i_Filename);		// probably throws

	return new_file;	// release file so something else can write to it
}


}


//------------------------------------------------------------------------
//	GetCreationTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's creation time
//------------------------------------------------------------------------
double GetCreationTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftCreate, ftLocal;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, &ftCreate, NULL, NULL))
		return 0;

	// Convert the create time to local time.
	if (!FileTimeToLocalFileTime(&ftCreate, &ftLocal))
		return 0;

	LONGLONG qwResult = (((ULONGLONG) ftLocal.dwHighDateTime) << 32) + ftLocal.dwLowDateTime;

	//subtract out the diff between the year 1970 (the unix and javascript style we're trying to return),
	//and the year 1600 (the time FILETIME structs are built on)
	qwResult -= 116444736000000000;

	//divide to get the value back up to seconds from the FILETIME style of 100-interval nanoseconds
	qwResult /= 10000000;

	return (double) qwResult;
}

//------------------------------------------------------------------------
//	GetLastAccessTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's last access time
//------------------------------------------------------------------------
double GetLastAccessTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftAccess, ftLocal;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, &ftAccess, NULL))
		return 0;

	// Convert the create time to local time.
	if (!FileTimeToLocalFileTime(&ftAccess, &ftLocal))
		return 0;

	LONGLONG qwResult = (((ULONGLONG) ftLocal.dwHighDateTime) << 32) + ftLocal.dwLowDateTime;

	//subtract out the diff between the year 1970 (the unix and javascript style we're trying to return),
	//and the year 1600 (the time FILETIME structs are built on)
	qwResult -= 116444736000000000;

	//divide to get the value back up to seconds from the FILETIME style of 100-interval nanoseconds
	qwResult /= 10000000;

	return (double) qwResult;
}

//------------------------------------------------------------------------
//	GetLastModifiedTimeT returns the file's last modified time as a
//	time_t, or -1 if the file can't be read.
//------------------------------------------------------------------------
time_t GetLastModifiedTimeT(const fsLocator& i_Locator)
{
	std::wstring filename;
	fsFileUtilPAC::LocatorToUnicodeFilename(i_Locator, filename);

	struct _stat64 st;
	if (_wstat64(filename.c_str(), &st) != 0)
		return -1;
	return st.st_mtime;
}

//------------------------------------------------------------------------
//	GetLastModifiedTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's last modified time
//------------------------------------------------------------------------
double GetLastModifiedTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftWrite, ftLocal;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, NULL, &ftWrite))
		return 0;

	// Convert the create time to local time.
	if (!FileTimeToLocalFileTime(&ftWrite, &ftLocal))
		return 0;

	LONGLONG qwResult = (((ULONGLONG) ftLocal.dwHighDateTime) << 32) + ftLocal.dwLowDateTime;

	//subtract out the diff between the year 1970 (the unix and javascript style we're trying to return),
	//and the year 1600 (the time FILETIME structs are built on)
	qwResult -= 116444736000000000;

	//divide to get the value back up to seconds from the FILETIME style of 100-interval nanoseconds
	qwResult /= 10000000;

	return (double) qwResult;
}

//------------------------------------------------------------------------
//	GetCreationString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetCreationString(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftCreate, ftLocal;
	SYSTEMTIME stCreate;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, &ftCreate, NULL, NULL))
		return std::string();

	// Convert the create time to local time.
	if (!FileTimeToLocalFileTime(&ftCreate, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stCreate);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d",
	  //  stCreate.wMonth, stCreate.wDay, stCreate.wYear);

	std::wostringstream create_ss(std::wostringstream::out);
	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wMonth << L"/"; 
	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wDay << L"/";
	create_ss << stCreate.wYear;
	
	return envString::WideCharToUTF8(create_ss.str());
}

//------------------------------------------------------------------------
//	GetLastAccessString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetLastAccessString(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftAccess, ftLocal;
	SYSTEMTIME stAccess;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, &ftAccess, NULL))
		return std::string();

	// Convert the last-access time to local time.
	if (!FileTimeToLocalFileTime(&ftAccess, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stAccess);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d",
	 //   stAccess.wMonth, stAccess.wDay, stAccess.wYear);

	std::wostringstream access_ss(std::wostringstream::out);

	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wMonth << L"/"; 
	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wDay << L"/";
	access_ss << stAccess.wYear;
	
	return envString::WideCharToUTF8(access_ss.str());
}

//------------------------------------------------------------------------
//	GetLastModifiedString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetLastModifiedString(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftWrite, ftLocal;
	SYSTEMTIME stWrite;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, NULL, &ftWrite))
		return std::string();

	// Convert the last-write time to local time.
	if (!FileTimeToLocalFileTime(&ftWrite, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stWrite);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d",
	//	stWrite.wMonth, stWrite.wDay, stWrite.wYear);

	std::wostringstream write_ss(std::wostringstream::out);

	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wMonth << L"/"; 
	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wDay << L"/";
	write_ss << stWrite.wYear;
	
	return envString::WideCharToUTF8(write_ss.str());
}

//------------------------------------------------------------------------
//	GetCreationString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetCreationStringWithTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftCreate, ftLocal;
	SYSTEMTIME stCreate;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, &ftCreate, NULL, NULL))
		return std::string();

	// Convert the create time to local time.
	if (!FileTimeToLocalFileTime(&ftCreate, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stCreate);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d %02d:%02d",
	//	stCreate.wMonth, stCreate.wDay, stCreate.wYear,
	 //   stCreate.wHour, stCreate.wMinute);

	std::wostringstream create_ss(std::wostringstream::out);

	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wMonth << L"/"; 
	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wDay << L"/";
	create_ss << stCreate.wYear << " ";
	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wHour << ":";
	create_ss << std::setw(2) << std::setfill(L'0') << stCreate.wMinute;
	
	
	return envString::WideCharToUTF8(create_ss.str());
}

//------------------------------------------------------------------------
//	GetLastAccessString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetLastAccessStringWithTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftAccess, ftLocal;
	SYSTEMTIME stAccess;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, &ftAccess, NULL))
		return std::string();

	// Convert the last-access time to local time.
	if (!FileTimeToLocalFileTime(&ftAccess, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stAccess);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d %02d:%02d",
	//	stAccess.wMonth, stAccess.wDay, stAccess.wYear,
	//	stAccess.wHour, stAccess.wMinute);

	std::wostringstream access_ss(std::wostringstream::out);

	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wMonth << L"/"; 
	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wDay << L"/";
	access_ss << stAccess.wYear << " ";
	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wHour << ":";
	access_ss << std::setw(2) << std::setfill(L'0') << stAccess.wMinute;
	
	
	return envString::WideCharToUTF8(access_ss.str());
}

//------------------------------------------------------------------------
//	GetLastModifiedString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//------------------------------------------------------------------------
std::string GetLastModifiedStringWithTime(const fsLocator& i_Locator)
{
	HANDLE Handle = GetHandle(i_Locator);
	FileCloser closer(Handle);	

	FILETIME ftWrite, ftLocal;
	SYSTEMTIME stWrite;

	// Retrieve the file times for the file.
	if (!GetFileTime(Handle, NULL, NULL, &ftWrite))
		return std::string();

	// Convert the last-write time to local time.
	if (!FileTimeToLocalFileTime(&ftWrite, &ftLocal))
		return std::string();

	// Convert the local file time from UTC to system time.
	FileTimeToSystemTime(&ftLocal, &stWrite);

	// Build a string showing the date and time.
	//WCHAR lpszString[20];
	//wsprintf(lpszString, L"%02d/%02d/%d %02d:%02d",
	//	stWrite.wMonth, stWrite.wDay, stWrite.wYear,
	//	stWrite.wHour, stWrite.wMinute);

	std::wostringstream write_ss(std::wostringstream::out);

	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wMonth << L"/"; 
	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wDay << L"/";
	write_ss << stWrite.wYear << " ";
	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wHour << ":";
	write_ss << std::setw(2) << std::setfill(L'0') << stWrite.wMinute;
	
	
	return envString::WideCharToUTF8(write_ss.str());
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
