/*****************************************************************************
**	pathDirectoryParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/path/pathDirectoryParser.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#include "Core/fs/fsFileEnum.hpp"
#include "Core/fs/fsFileUtil.hpp"
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

//----------------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------------
pathDirectoryParser::pathDirectoryParser()
{
}

//----------------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------------
//pathDirectoryParser::~pathDirectoryParser()
//{
//}

//----------------------------------------------------------------------------
// Returns a list of files that are in the given directory
//----------------------------------------------------------------------------
void pathDirectoryParser::GetDirectoryFiles(const fsLocator& i_Directory, 
											std::vector<fsLocator>& o_Files)
{
	// If the directory doesn't exist, return an error and jump out
	if( !fsFileUtil::DirectoryExists(i_Directory) )
	{
		DBG_ERROR("Directory: " << i_Directory << " does not exist");
		return;
	}

	fsLocator NextFile;
	fsFileEnum::fsFileList Files;

	//grab all of the files in the directory
	fsFileEnum::EnumerateFiles(i_Directory, Files);
	for (int i = 0; i < Files.size(); ++i)
	{
		//complete the locator and add to our vector
		NextFile.Clear();
		NextFile = i_Directory;
		NextFile.Push(Files[i].GetLastName());
		o_Files.push_back(NextFile);
	}
}

//----------------------------------------------------------------------------
// Returns a list of files with the given extension that are in 
// the given directory
//----------------------------------------------------------------------------
void pathDirectoryParser::GetDirectoryFilesWithExt(const fsLocator& i_Directory, 
												  std::vector<fsLocator>& o_Files, 
												  const itString& i_Ext)
{
	// If the directory doesn't exist, return an error and jump out
	if( !fsFileUtil::DirectoryExists(i_Directory) )
	{
		DBG_ERROR("Directory: " << i_Directory << " does not exist");
		return;
	}

	itString ext;
	fsLocator NextFile;
	fsFileEnum::fsFileList Files;

	//grab all of the files in the directory
	fsFileEnum::EnumerateFiles(i_Directory, Files);
	for (int i = 0; i < Files.size(); ++i)
	{
		//make sure extensions match before adding the file to our list
		Files[i].GetLastName().GetExtension(ext);
		if( ext != i_Ext )
			continue;

		//complete the locator and add to our vector
		NextFile.Clear();
		NextFile = i_Directory;
		NextFile.Push(Files[i].GetLastName());
		o_Files.push_back(NextFile);
	}
}

//----------------------------------------------------------------------------
// Returns a list of sub directories that are in the given directory
//----------------------------------------------------------------------------
void pathDirectoryParser::GetSubDirectories(const fsLocator& i_Directory, 
											std::vector<fsLocator>& o_Directories)
{
	//	if Directory is empty, jump out
	if (i_Directory.GetNumNames() == 0)
	{
		//	Why are there EMPTY directories?  Missing assets
		//
		//	Programmers can put a break point on this return to figure out
		//	what is sending an empty directory.
		//
		DBG_WARNING("Getting subdirectories on an empty directory path");
		return;
	}

	// If the directory doesn't exist, return an error and jump out
	if( !fsFileUtil::DirectoryExists(i_Directory) )
	{
		DBG_ERROR("Directory: (" << i_Directory << ") does not exist");
		return;
	}

	fsLocator NextDir, tempFSFilename;
	fsFileEnum::fsFileList SubDirectories;
	itString ignore_str("99.");

	//grab all of the files in the directory
	fsFileEnum::EnumerateDirectories(i_Directory, SubDirectories);
	for (int i = 0; i < SubDirectories.size(); ++i)
	{
		itString filename_str;
		filename_str = SubDirectories[i].GetLastName();

		//	ignore "." and ".." and other system directories that start with "."
		if (filename_str.StartsWith(itString(".")))
			continue;

		//	don't include folders that start with "99."
		if (filename_str.StartsWith(ignore_str))
			continue;

		//complete the locator and add to our vector
		NextDir.Clear();
		NextDir = i_Directory;
		NextDir.Push(filename_str);
		o_Directories.push_back(NextDir);
	}
}

//----------------------------------------------------------------------------
// Returns a list of sub directories that are in the given directory. Snips the 
// numbers in the beginning of the directory names
//----------------------------------------------------------------------------
void pathDirectoryParser::GetClippedSubDirectories(const fsLocator& i_Directory, 
													std::vector<fsLocator>& o_Directories)
{
	//	if Directory is empty, jump out
	if (i_Directory.GetNumNames() == 0)
	{
		//	Why are there EMPTY directories?  Missing assets
		//
		//	Programmers can put a break point on this return to figure out
		//	what is sending an empty directory.
		//
		DBG_WARNING("Getting subdirectories on an empty directory path");
		return;
	}

	// If the directory doesn't exist, return an error and jump out
	if( !fsFileUtil::DirectoryExists(i_Directory) )
	{
		DBG_ERROR("Directory: (" << i_Directory << ") does not exist");
		return;
	}

	fsLocator NextDir, tempFSFilename;
	fsFileEnum::fsFileList SubDirectories;
	itString extension;
	std::string filename_str;
	std::string ignore_str("99");

	//grab all of the files in the directory
	fsFileEnum::EnumerateDirectories(i_Directory, SubDirectories);
	for (int i = 0; i < SubDirectories.size(); ++i)
	{
		std::string cur_dir = itStringUtil::GetStdString(SubDirectories[i].GetLastName());

		//	ignore "." and ".." and other system directories that start with "."
		std::string dot_dir(".");
		if( cur_dir[0] == dot_dir[0] )
			continue;

		//complete the locator and add to our vector
		NextDir.Clear();
		NextDir = i_Directory;

		//	don't include folders that start with "99."
		filename_str = itStringUtil::GetStdString(SubDirectories[i].GetLastName());
		strtok((char*)filename_str.c_str(), ".");
		if((filename_str[0] == ignore_str[0]) && (filename_str[1] == ignore_str[1]))
			continue;

		//	append each directory
		SubDirectories[i].GetLastName().GetExtension(extension);
		if(extension.GetLength() == 0)
			NextDir.Push(SubDirectories[i].GetLastName());
		else 
			NextDir.Push(extension);

		o_Directories.push_back(NextDir);
	}
}
