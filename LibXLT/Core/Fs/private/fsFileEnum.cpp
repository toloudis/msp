/****************************************************************************\
**  fsFileEnum.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/fsFileEnum.hpp"

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif

#include <filesystem>
#include <system_error>


//============================================================================
//============================================================================
namespace fsFileEnum
{

namespace
{
	// returns true if the file name contains one of the substrings, or there are none
	bool substring_match(const std::string& i_Filename, const std::vector<std::string>& i_SubStrings)
	{
		if (i_SubStrings.empty())
			return true;
		for (const std::string& substring : i_SubStrings)
		{
			if (i_Filename.find(substring) != std::string::npos)
				return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	//	enumerate gives the target each entry of i_Locator that is a
	//	directory (i_bDirectories) or isn't, as a one-name locator,
	//	until the target returns false. A directory that can't be read
	//	lists nothing. "." and ".." aren't listed.
	//--------------------------------------------------------------------
	bool enumerate(const fsLocator& i_Locator, EnumTarget& o_Target, bool i_bDirectories,
				   const std::vector<std::string>& i_SubStrings)
	{
		// An empty locator is the current directory, as it was to FindFirstFile
		std::filesystem::path dir = fsPathUtil::LocatorToPath(i_Locator);
		if (dir.empty())
			dir = ".";

		std::error_code error;
		std::filesystem::directory_iterator it(dir, error), end;
		for (; !error && it != end; it.increment(error))
		{
			std::error_code type_error;
			if (it->is_directory(type_error) != i_bDirectories)
				continue;

			const std::string name = fsPathUtil::PathToUtf8(it->path().filename());
			if (!substring_match(name, i_SubStrings))
				continue;

			if (!o_Target.Notify(i_Locator, fsLocator(name)))
				return false;
		}
		return true;
	}
}

FileListTarget::FileListTarget() 					{m_FileList.clear();}
FileListTarget::~FileListTarget()					{}
const fsFileList& FileListTarget::GetList() const	{return m_FileList;}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool FileListTarget::Notify(const fsLocator& i_Directory, const fsLocator& i_File)
{
	fsLocator Locator = i_Directory;
	Locator.Push(i_File);
	m_FileList.push_back(Locator);
	return true;
}

//----------------------------------------------------------------------------
//	EnumerateFiles lists all of the files in a directory.  If i_SubString
//	is not NULL, only files whose names contain the given substring
//	will be listed. Returns the last return value from fsEnumTarget::Notify
//----------------------------------------------------------------------------
bool EnumerateFiles(const fsLocator& i_Locator, 
					EnumTarget& o_Target,
					bool i_bSearchSubdirectories)
{
	std::vector<std::string> no_substrings;
	return EnumerateFiles(i_Locator, o_Target, no_substrings, i_bSearchSubdirectories);
}
bool EnumerateFiles(const fsLocator& i_Locator, 
					EnumTarget& o_Target,
					const std::string &i_SubString,
					bool i_bSearchSubdirectories)
{
	std::vector<std::string> substrings;
	substrings.push_back(i_SubString);
	return EnumerateFiles(i_Locator, o_Target, substrings, i_bSearchSubdirectories);
}
bool EnumerateFiles(const fsLocator& i_Locator, 
					EnumTarget& o_Target,
					const std::vector<std::string> &i_SubStrings,
					bool i_bSearchSubdirectories)
{
	// Search files in the root directory first
	bool continue_enum = enumerate(i_Locator, o_Target, false, i_SubStrings);

	// If we should search subdirectories and the target wants to continue looking,
	// enumerate the directories and then search each of those
	if (i_bSearchSubdirectories && continue_enum)
	{
		fsFileList dirList;
		EnumerateDirectories(i_Locator, dirList);
		fsFileList::const_iterator it;
		for (it = dirList.begin(); it != dirList.end(); ++it)
		{
			const std::string& last_name = it->GetLastName();
			if ((last_name != ".") && (last_name != ".."))
			{
				//std::string filename;
				//fsFileUtil::LocatorToANSIFilename(*it, filename);
				//DBG_LOG("Searching: " << filename.c_str());

				continue_enum = EnumerateFiles(*it, o_Target, i_SubStrings, i_bSearchSubdirectories);
				if (!continue_enum)
					return false;
			}
		}
	}

	return continue_enum;
}

//----------------------------------------------------------------------------
//	EnumerateFiles uses a fsFileEnum::fsFileList instead of a 
//  fsFileEnum::EnumTarget.
//----------------------------------------------------------------------------
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileList& o_FileList,
					bool i_bSearchSubdirectories)
{
	std::vector<std::string> no_substrings;
	return EnumerateFiles(i_Locator, o_FileList, no_substrings, i_bSearchSubdirectories);
}
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileList& o_FileList,
					const std::string &i_SubString,
					bool i_bSearchSubdirectories)
{
	std::vector<std::string> substrings;
	substrings.push_back(i_SubString);
	return EnumerateFiles(i_Locator, o_FileList, substrings, i_bSearchSubdirectories);
}
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileList& o_FileList,
					const std::vector<std::string> &i_SubStrings,
					bool i_bSearchSubdirectories)
{
	FileListTarget FileTarget;
	bool bResult = EnumerateFiles(i_Locator, FileTarget, i_SubStrings, i_bSearchSubdirectories);

	if (bResult)
		o_FileList = FileTarget.GetList();

	return bResult;
}

//----------------------------------------------------------------------------
//	EnumerateDirectories lists all of the subdirectories in a directory.
//	Returns the last return value from fsEnumTarget::Notify
//----------------------------------------------------------------------------
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							EnumTarget& o_Target)
{
	return enumerate(i_Locator, o_Target, true, std::vector<std::string>());
}

//----------------------------------------------------------------------------
//	EnumerateDirectories uses a fsFileEnum::fsFileList instead of a 
//  fsFileEnum::EnumTarget.
//----------------------------------------------------------------------------
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							fsFileList& o_DirList)
{
	FileListTarget FileTarget;
	bool bResult = EnumerateDirectories(i_Locator, FileTarget);

	if (bResult)
		o_DirList = FileTarget.GetList();

	return bResult;
}

}
