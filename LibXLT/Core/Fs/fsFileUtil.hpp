/****************************************************************************\
**  fsFileUtil.hpp
**
**      fsFileUtil.hpp supplies functions that operate on files.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#undef CreateDirectory
#ifdef FS_FILEUTIL_HPP
#error fsFileUtil.hpp multiply included
#endif
#define FS_FILEUTIL_HPP

#include <string>


//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace fsFileUtil
{
	//----------------------------------------------------------------------------
	//	CreateFile creates a file with the given name.  This must be done before
	//	reading or writing with a fsFileStream.
	//----------------------------------------------------------------------------
	void CreateFile(const fsLocator& i_Filename);

	//----------------------------------------------------------------------------
	//	RenameFile renames a file or directory.  This can be used to move a 
	//	file to different directories, etc.
	//----------------------------------------------------------------------------
	void RenameFile(const fsLocator& i_From, const fsLocator& i_To);

	//----------------------------------------------------------------------------
	//	DeleteFile deletes a file.  The file must exist, or an exception will be
	//	thrown.
	//----------------------------------------------------------------------------
	void DeleteFile(const fsLocator& i_Filename);

	//----------------------------------------------------------------------------
	//	CopyFile copies an entire file from one location to another.  Of course,
	//	the file must exist.
	//----------------------------------------------------------------------------
	void CopyFile(const fsLocator& i_From, const fsLocator& i_To);

	//----------------------------------------------------------------------------
	//	CreateDirectory creates a directory with the given name.
	//----------------------------------------------------------------------------
	void CreateDirectory(const fsLocator& i_DirectoryName);

	//----------------------------------------------------------------------------
	//	DeleteDirectory destroys a directory with the given name.  The directory
	//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
	//----------------------------------------------------------------------------
	void DeleteDirectory(const fsLocator& i_DirectoryName);

	//----------------------------------------------------------------------------
	//	DeleteDirectoryRecursively deletes the contents of the directory.
	//----------------------------------------------------------------------------
	bool DeleteDirectoryRecursively(const fsLocator& i_DirectoryName);
	
	//----------------------------------------------------------------------------
	//	CopyDirectory recursively copies the contents of one directory to another.
	//	Of course, the from directory must exist
	//----------------------------------------------------------------------------
	void CopyDirectory(const fsLocator& i_From, const fsLocator& i_To);

	//----------------------------------------------------------------------------
	//	FileExists tests if a file exists.
	//----------------------------------------------------------------------------
	bool FileExists(const fsLocator& i_Filename);

	//----------------------------------------------------------------------------
	//	DirectoryExists tests if a directory exists.
	//----------------------------------------------------------------------------
	bool DirectoryExists(const fsLocator& i_Filename);

	//----------------------------------------------------------------------------
	// UnicodeStringToLocator returns a locator from a given UTF-8 string
	// the string is separted by backslashes in the windows style ,this string
	// should not be used for pathing, but rather for logging or perhaps
	// storing a locator as a fully pathed string. NOTE: the index will be ignored,
	// be sure to call gfFileTranslationMgr::ExpandLocator first if you need to
	// The same as ANSIFilenameToLocator, now that both take UTF-8.
	//----------------------------------------------------------------------------
	void UnicodeStringToLocator(const std::string& i_String, fsLocator& o_Locator);

	//----------------------------------------------------------------------------
	// LocatorToUnicodeString returns a UTF-8 string from a given locator
	// the string is separted by backslashes in the windows style ,this string
	// should not be used for pathing, but rather for logging or perhaps
	// storing a locator as a fully pathed string. NOTE: the index will be ignored,
	// be sure to call gfFileTranslationMgr::ExpandLocator first if you need to
	// The same as LocatorToANSIFilename, now that both give UTF-8.
	//----------------------------------------------------------------------------
	void LocatorToUnicodeString(const fsLocator& i_Locator, std::string& o_String);

	//	Temporary, for callers that still hold an itString
	//	(docs/plans/2026-10-28-itstring-to-std-string.md; removed in step 4)
	void LocatorToUnicodeString(const fsLocator& i_Locator, itString& o_String);

	//----------------------------------------------------------------------------
	// ANSIFilenameToLocator returns a locator from an ANSI pathname
	// API Change: This now interprets the std::string as UTF8
	//----------------------------------------------------------------------------
	void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator);

	//----------------------------------------------------------------------------
	//	LocatorToANSIFilename converts a fsLocator to a narrow (UTF-8) filename
	//	string, for APIs that take char filenames.  It is provided here to assist in error reporting.
	// API Change: This now returns the std::string as UTF8.
	//----------------------------------------------------------------------------
	void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_bUseDoubleSlashes = false);

	//----------------------------------------------------------------------------
	//	LocatorToANSIFilename converts a fsLocator to a narrow (UTF-8) filename
	//	string, for APIs that take char filenames.  It is provided here to assist in error reporting.
	// API Change: This now returns the std::string as UTF8.
	//----------------------------------------------------------------------------
	void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, const int i_StartNum, const int i_EndNum );

	//----------------------------------------------------------------------------
	// Return true if every path in the filename is an ascii string
	//----------------------------------------------------------------------------
	bool IsFilenameASCII(const fsLocator& i_Locator);

	//----------------------------------------------------------------------------
	//	Create a filename string that is unique within the given pathlocator,
	//	using the given prefix and extension.
	//----------------------------------------------------------------------------
	void GenerateFileName(const fsLocator& i_PathLocator, std::string& i_Prefix, std::string& i_Extension, std::string& o_Name);

	//----------------------------------------------------------------------------
	//  Check whether filename is absolute or not
	//----------------------------------------------------------------------------
	bool IsFileNameAbsolute( const fsLocator & i_Loc );


	//----------------------------------------------------------------------------
	//	IsReadOnly returns true if the file is read-only, meaning that write
	//	operations to the file will fail.
	//----------------------------------------------------------------------------
	bool IsReadOnly(const fsLocator& i_Locator);

	//----------------------------------------------------------------------------
	//	SetReadOnly() - sets the file to be read only if true is passed in.
	//----------------------------------------------------------------------------
	void SetReadOnly( const fsLocator& i_Locator, bool i_bSetReadOnly );

	//------------------------------------------------------------------------
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();
}
