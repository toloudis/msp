/****************************************************************************\
**  fsFileEnumPACXbox.cpp
**
**      fsFileEnumPACXbox.cpp supplies functions that enumerate files and
**	directories.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "fsFileEnumPAC.hpp"

#include "fsFileUtilPAC.hpp"
#include "fsLocator.hpp"

#include <xtl.h>

namespace fsFileEnumPAC
{

namespace
{

struct FindCloser
{
	FindCloser(HANDLE i_Handle) : m_Handle(i_Handle) {}
	~FindCloser() { ::FindClose(m_Handle); }
	HANDLE m_Handle;
};

}

//========================================================================
//	EnumerateFiles lists all of the files in a directory.  If i_SubString
//	is not NULL, only files whose names contain the given substring
//	will be listed. Returns the last return value from fsEnumTarget::Notify
//========================================================================
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileEnum::EnumTarget& o_Target,
					const itString* i_SubString)
{
	HANDLE hFind;
	fsLocator ret_val;
	bool continue_enum;

	if( fsFileUtilPAC::MustUseANSIFilenames() )
	{
		itString filename;
//		itString::CharType wide_char_name[512];			
		WIN32_FIND_DATAA find_file_data;
		std::string root_dir;

		fsLocator root_dir_locator = i_Locator;
		root_dir_locator.Push(itString("*"));
		fsFileUtilPAC::LocatorToANSIFilename(root_dir_locator, root_dir);
		
		hFind = ::FindFirstFileA(root_dir.c_str(), &find_file_data);

		if( hFind != INVALID_HANDLE_VALUE )
		{
			FindCloser closer(hFind);
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
/*
				::MultiByteToWideChar(	::GetACP(),
										MB_PRECOMPOSED,
										find_file_data.cFileName,
										-1,
										wide_char_name,
										512);
				
				filename = wide_char_name;
				
				if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
				{
					ret_val.Push(itString(wide_char_name));	
					continue_enum = o_Target.Notify(i_Locator, ret_val);

					if( continue_enum == false )
						return false;

					ret_val.Pop();
				}
*/
				filename = find_file_data.cFileName;
				
				if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
				{
					ret_val.Push( filename );	
					continue_enum = o_Target.Notify(i_Locator, ret_val);

					if( continue_enum == false )
						return false;

					ret_val.Pop();
				}
			}

			BOOL found = ::FindNextFileA(hFind, &find_file_data);

			while( found )
			{
				if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
				{
/*
					::MultiByteToWideChar(	::GetACP(),
											MB_PRECOMPOSED,
											find_file_data.cFileName,
											-1,
											wide_char_name,
											512);
					
					filename = wide_char_name;
					
					if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
					{
						ret_val.Push(itString(wide_char_name));	
						continue_enum = o_Target.Notify(i_Locator, ret_val);

						if( continue_enum == false )
							return false;

						ret_val.Pop();
					}
*/
					filename = find_file_data.cFileName;
					
					if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
					{
						ret_val.Push( filename );	
						continue_enum = o_Target.Notify(i_Locator, ret_val);

						if( continue_enum == false )
							return false;

						ret_val.Pop();
					}
				}

				found = ::FindNextFileA(hFind, &find_file_data);
			}
		}				
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
		return false;
/*
		itString filename;
		WIN32_FIND_DATAW find_file_data;
		itString root_dir;

		fsLocator locator_with_wildcard = i_Locator;
		locator_with_wildcard.Push(itString("*"));
		fsFileUtilPAC::LocatorToUnicodeFilename(locator_with_wildcard, root_dir);
		root_dir += 0;		//NULL terminate

		hFind = ::FindFirstFileW(root_dir.GetString(), &find_file_data);

		if( hFind != INVALID_HANDLE_VALUE )
		{
			FindCloser closer(hFind);
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
				filename = find_file_data.cFileName;
				
				if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
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
					filename = find_file_data.cFileName;
					
					if( (i_SubString == NULL) || (filename.HasSubString(*i_SubString)) )
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
*/
	}

	return true;
}

//========================================================================
//	EnumerateDirectories lists all of the subdirectories in a directory.
//	Returns the last return value from fsEnumTarget::Notify
//========================================================================
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							fsFileEnum::EnumTarget& o_Target)
{
	HANDLE hFind;
	fsLocator ret_val;
	bool continue_enum;

	if( fsFileUtilPAC::MustUseANSIFilenames() )
	{
		itString filename;
//		itString::CharType wide_char_name[512];			
		WIN32_FIND_DATAA find_file_data;
		std::string root_dir;

		fsLocator root_dir_locator = i_Locator;
		root_dir_locator.Push(itString("*"));
		fsFileUtilPAC::LocatorToANSIFilename(root_dir_locator, root_dir);
		
		hFind = ::FindFirstFileA(root_dir.c_str(), &find_file_data);

		if( hFind != INVALID_HANDLE_VALUE )
		{
			FindCloser closer(hFind);
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
/*
				::MultiByteToWideChar(	::GetACP(),
										MB_PRECOMPOSED,
										find_file_data.cFileName,
										-1,
										wide_char_name,
										512);
				
				filename = wide_char_name;
				
				ret_val.Push(itString(wide_char_name));	
				continue_enum = o_Target.Notify(i_Locator, ret_val);

				if( continue_enum == false )
					return false;

				ret_val.Pop();
*/
				filename = find_file_data.cFileName;
				
				ret_val.Push( filename );	
				continue_enum = o_Target.Notify(i_Locator, ret_val);

				if( continue_enum == false )
					return false;

				ret_val.Pop();
			}

			BOOL found = ::FindNextFileA(hFind, &find_file_data);

			while( found )
			{
				if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
				{
/*
					::MultiByteToWideChar(	::GetACP(),
											MB_PRECOMPOSED,
											find_file_data.cFileName,
											-1,
											wide_char_name,
											512);
					
					filename = wide_char_name;
					
					ret_val.Push(itString(wide_char_name));	
					continue_enum = o_Target.Notify(i_Locator, ret_val);

					if( continue_enum == false )
						return false;

					ret_val.Pop();
*/
					filename = find_file_data.cFileName;
					
					ret_val.Push( filename );	
					continue_enum = o_Target.Notify(i_Locator, ret_val);

					if( continue_enum == false )
						return false;

					ret_val.Pop();
				}

				found = ::FindNextFileA(hFind, &find_file_data);
			}
		}				
	}
	else
	{
		DBG_ASSERT0(false, "Unicode not implemented!");
		return false;
/*
		itString filename;
		WIN32_FIND_DATAW find_file_data;
		itString root_dir;

		fsLocator locator_with_wildcard = i_Locator;
		locator_with_wildcard.Push(itString("*"));
		fsFileUtilPAC::LocatorToUnicodeFilename(locator_with_wildcard, root_dir);
		root_dir += 0;		//NULL terminate

		hFind = ::FindFirstFileW(root_dir.GetString(), &find_file_data);

		if( hFind != INVALID_HANDLE_VALUE )
		{
			FindCloser closer(hFind);
			if( (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
				filename = find_file_data.cFileName;
				
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
					filename = find_file_data.cFileName;
					
					ret_val.Push(filename);	
					continue_enum = o_Target.Notify(i_Locator, ret_val);
					if( continue_enum == false )
						return false;

					ret_val.Pop();
				}

				found = ::FindNextFileW(hFind, &find_file_data);
			}
		}
*/
	}

	return true;
}

//========================================================================
//	GetDrives lists all of the drives listed by windows
//========================================================================
void GetDrives(std::vector<fsLocator>& o_DirList)
{
/*
	o_DirList.clear();

	unsigned long BufferSize = 128, ActualBufferSize;
	char *Buffer = new char[BufferSize];
	ActualBufferSize = GetLogicalDriveStrings(BufferSize, Buffer);
	if (BufferSize < ActualBufferSize)
	{
		delete Buffer;
		BufferSize = ActualBufferSize;
		Buffer = new char[BufferSize];
		ActualBufferSize = GetLogicalDriveStrings(BufferSize, Buffer);
	}

	int i;
	itString DriveName;
	for (i = 0; i < ActualBufferSize; ++i)
	{
		if ('\0' == Buffer[i] && DriveName.GetLength())
		{
			fsLocator Loc;
			if ('\\' == DriveName[DriveName.GetLength() - 1])
			{
				DriveName.RemoveCharAt(DriveName.GetLength() - 1);
			}
			Loc.Push(DriveName);
			o_DirList.push_back(Loc);
			DriveName.Clear();
		}
		else
		{
			DriveName += Buffer[i];
		}
	}

	delete Buffer;
*/
	o_DirList.clear();
	fsLocator dvd;
	dvd.Push("d:");
	o_DirList.push_back(dvd);
}

}
