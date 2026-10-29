/****************************************************************************\
**  fsFileEnum.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/private/fsFileEnumPAC.hpp"

#include "Core/fs/private/fsFileUtilPAC.hpp"
#include "Core/fs/fsLocator.hpp"
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif

#include <windows.h>


//============================================================================
//============================================================================
namespace fsFileEnumPAC
{

namespace
{
	//============================================================================
	//============================================================================
	struct FindCloser
	{
		FindCloser(HANDLE i_Handle) : m_Handle(i_Handle) {}
		~FindCloser() { ::FindClose(m_Handle); }
		HANDLE m_Handle;
	};

	//--------------------------------------------------------------------
	// returns true if the filename contains one of the substrings.
	//--------------------------------------------------------------------
	bool substring_match(const std::string& i_Filename,
						 const std::vector<std::string> &i_SubStrings)
	{
		if (i_SubStrings.empty())
			return true;

		std::vector<std::string>::const_iterator it;
		for (it = i_SubStrings.begin(); it != i_SubStrings.end(); ++it)
		{
			if (i_Filename.find(*it) != std::string::npos)
				return true;
		}
		return false;
	}
	
}

//------------------------------------------------------------------------
//	EnumerateFiles lists all of the files in a directory.  If i_SubStrings
//	is not empty, only files whose names contain one of the given substrings
//	will be listed. Returns the last return value from fsEnumTarget::Notify
//------------------------------------------------------------------------
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileEnum::EnumTarget& o_Target,
					const std::vector<std::string> &i_SubStrings)
{
	HANDLE hFind;
	fsLocator ret_val;
	bool continue_enum;

	std::string filename;
	WIN32_FIND_DATAW find_file_data;
	std::wstring root_dir;

	fsLocator locator_with_wildcard = i_Locator;
	locator_with_wildcard.Push("*");
	fsFileUtilPAC::LocatorToUnicodeFilename(locator_with_wildcard, root_dir);

	hFind = ::FindFirstFileW(root_dir.c_str(), &find_file_data);

	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindCloser closer(hFind);
		if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			filename = envString::WideCharToUTF8(find_file_data.cFileName);
			
			if (substring_match(filename, i_SubStrings))
			{
				ret_val.Push(filename);	
				continue_enum = o_Target.Notify(i_Locator, ret_val);
				if( continue_enum == false )
					return false;

				ret_val.Pop();
			}
		}

		BOOL found = ::FindNextFileW(hFind, &find_file_data);

		while( found )
		{
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
				filename = envString::WideCharToUTF8(find_file_data.cFileName);
				
				if (substring_match(filename, i_SubStrings))
				{
					ret_val.Push(filename);	
					continue_enum = o_Target.Notify(i_Locator, ret_val);
					if( continue_enum == false )
						return false;

					ret_val.Pop();
				}
			}

			found = ::FindNextFileW(hFind, &find_file_data);
		}
	}				

	return true;
}

//------------------------------------------------------------------------
//	EnumerateDirectories lists all of the subdirectories in a directory.
//	Returns the last return value from fsEnumTarget::Notify
//------------------------------------------------------------------------
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							fsFileEnum::EnumTarget& o_Target)
{
	HANDLE hFind;
	fsLocator ret_val;
	bool continue_enum;

	std::string filename;
	WIN32_FIND_DATAW find_file_data;
	std::wstring root_dir;

	fsLocator locator_with_wildcard = i_Locator;
	locator_with_wildcard.Push("*");
	fsFileUtilPAC::LocatorToUnicodeFilename(locator_with_wildcard, root_dir);

	hFind = ::FindFirstFileW(root_dir.c_str(), &find_file_data);

	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindCloser closer(hFind);
		if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			filename = envString::WideCharToUTF8(find_file_data.cFileName);
			
			ret_val.Push(filename);	
			continue_enum = o_Target.Notify(i_Locator, ret_val);
			if( continue_enum == false )
				return false;

			ret_val.Pop();
		}

		BOOL found = ::FindNextFileW(hFind, &find_file_data);

		while( found )
		{
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
				filename = envString::WideCharToUTF8(find_file_data.cFileName);
				
				ret_val.Push(filename);	
				continue_enum = o_Target.Notify(i_Locator, ret_val);
				if( continue_enum == false )
					return false;

				ret_val.Pop();
			}

			found = ::FindNextFileW(hFind, &find_file_data);
		}
	}				

	return true;
}

//------------------------------------------------------------------------
//	GetDrives lists all of the drives listed by windows
//------------------------------------------------------------------------
void GetDrives(std::vector<fsLocator>& o_DirList)
{
	o_DirList.clear();

	unsigned long BufferSize = 128, ActualBufferSize;
	std::vector<wchar_t> Buffer(BufferSize);
	ActualBufferSize = GetLogicalDriveStringsW(BufferSize, Buffer.data());
	if (BufferSize < ActualBufferSize)
	{
		BufferSize = ActualBufferSize;
		Buffer.resize(BufferSize);
		ActualBufferSize = GetLogicalDriveStringsW(BufferSize, Buffer.data());
	}

	int i;
	std::wstring DriveName;
	for (i = 0; i < ActualBufferSize; ++i)
	{
		if (L'\0' == Buffer[i] && !DriveName.empty())
		{
			fsLocator Loc;
			if (L'\\' == DriveName.back())
			{
				DriveName.pop_back();
			}
			Loc.Push(envString::WideCharToUTF8(DriveName));
			o_DirList.push_back(Loc);
			DriveName.clear();
		}
		else
		{
			DriveName += Buffer[i];
		}
	}
}

}
