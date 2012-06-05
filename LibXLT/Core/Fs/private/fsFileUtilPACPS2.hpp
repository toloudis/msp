/****************************************************************************\
**  fsFileUtilPACPS2.hpp
**
**      fsFileUtilPACPS2.hpp defines the file util PAC for PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILEUTILPACPS2_HPP
#error fsFileUtilPACPS2.hpp multiply included
#endif
#define FS_FILEUTILPACPS2_HPP

#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

#include <string>

class fsLocator;

namespace fsFileUtilPAC
{
	//============================================================================
	//	SetSystemParams is a PS2 specific function that controls aspects of how
	//	the the game boots and runs.  If reboot IOP is true it will reboot the
	//	IOP to replace the IOP modules.  If i_CDFiles is true it will expect all
	//	IOP modules to be in a directory "IOP" on a CD in the drive, and
	//	additionally all game files will be loaded from CD.
	//	The final game will use "true, true" for these parameters.
	//	This function should be called before the fsPackage::Init().
	//============================================================================
	void SetSystemParams(bool i_RebootIOP, bool i_CDFiles);	

	//============================================================================
	//	GetCDMode will return true if game files are to be loaded from CD/DVD,
	//	as opposed to networked host computer.
	//============================================================================
	bool GetCDMode();

	//============================================================================
	//	CDSearchFile works like sceCdSearchFile, but works properly with > 30
	//	files/directory.  It returns true if the file is found; in such a case,
	//	the file's first sector and size will also be filled in.  The size is in
	//	bytes.
	//============================================================================
	bool CDSearchFile(int& o_Sector, int& o_Size, const fsLocator& i_Locator);

	//============================================================================
	//	GetCDDirectoryFileInfo()
	//
	//	Get all files under the given directory. Subdirectories are not included.
	//============================================================================
	bool GetCDDirectoryFileInfo( const fsLocator& i_Locator, std::vector<itString>& o_FileInfo );

	//============================================================================
	//	CreateFile creates a file with the given name.  This must be done before
	//	reading or writing with a fsFileStream.
	//============================================================================
	void CreateFile(const fsLocator& i_Filename);

	//============================================================================
	//	RenameFile renames a file.  This can be used to move a file to different
	//	directories, etc.
	//============================================================================
	void RenameFile(const fsLocator& i_From, const fsLocator& i_To);

	//============================================================================
	//	DeleteFile deletes a file.  The file must exist, or an exception will be
	//	thrown.
	//============================================================================
	void DeleteFile(const fsLocator& i_Filename);

	//============================================================================
	//	CopyFile copies an entire file from one location to another.  Of course,
	//	the file must exist.
	//============================================================================
	void CopyFile(const fsLocator& i_From, const fsLocator& i_To);

	//============================================================================
	//	CopyDirectory copies a directory with the given name and all sub-dirs.
	//============================================================================
	void CopyDirectory(const fsLocator& i_DirectoryName);

	//============================================================================
	//	CreateDirectory creates a directory with the given name.
	//============================================================================
	void CreateDirectory(const fsLocator& i_DirectoryName);

	//============================================================================
	//	DeleteDirectory destroys a directory with the given name.  The directory
	//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
	//============================================================================
	void DeleteDirectory(const fsLocator& i_DirectoryName);

	//============================================================================
	//	FileExists tests if a file exists.
	//============================================================================
	bool FileExists(const fsLocator& i_Filename);

	//============================================================================
	//	DirectoryExists tests if a directory exists.
	//============================================================================
	bool DirectoryExists(const fsLocator& i_Filename);

	//========================================================================
	// ANSIFilenameToLocator returns a locator from an ANSI pathname
	//========================================================================
	void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator);

	//============================================================================
	//	LocatorToANSIFilename is used internally by the PS2 PACs in the fs
	//	package.  It converts a fsLocator to a ANSI filename (for use by
	//	PS2 library functions).
	//============================================================================
	void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_Directory = false);

	//============================================================================
	//	LocatorToCDFilename is used internally by the PS2 PACs in the fs
	//	package.  It converts a cdrom0 locator to a format useable by 
	//	sceCDSearchFile.
	//============================================================================
	void LocatorToCDFilename(const fsLocator& i_Locator, std::string& o_String, bool i_Directory = false);

	//============================================================================
	//	LocatorToANSIFilename is used internally by the PS2 PACs in the fs
	//	package.  It converts a fsLocator to a ANSI filename (for use by
	//	PS2 library functions).
	//============================================================================
//	void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, const int i_StartNum, const int i_EndNum );

	//============================================================================
	//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
	//	package.  It converts a fsLocator to a Unicode filename (for use in
	//	Windows NT or 2000).  This function is not part of the fsFileUtil 
	//	interface.
	//============================================================================
	void LocatorToUnicodeFilename(const fsLocator& i_Locator, itString& o_String );

	//============================================================================
	//	LocatorToUnicodeFilename is used internally by the win32 PACs in the fs
	//	package.  It converts a fsLocator to a Unicode filename (for use in
	//	Windows NT or 2000).  This function is not part of the fsFileUtil 
	//	interface.
	//============================================================================
	void LocatorToUnicodeFilename(const fsLocator& i_Locator, itString& o_String, const int i_StartNum, const int i_EndNum );

	//============================================================================
	//	MustUseANSIFilenames() returns true for Windows OSs that don't have
	//	full Unicode support.  This function is not part of the fsFileUtil 
	//	interface.
	//============================================================================
	bool MustUseANSIFilenames();

	//============================================================================
	//	IsReadOnly returns true if the file is read-only, meaning that write
	//	operations to the file will fail.
	//============================================================================
	bool IsReadOnly(const fsLocator& i_Locator);

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void Init();
	void CleanUp() throw();
}

