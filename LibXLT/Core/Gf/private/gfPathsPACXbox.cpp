/****************************************************************************\
**  gfPathsPACXbox.cpp
**
**      gfPathsPACXbox.cpp defines the game paths PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "gfPathsPACXbox.hpp"

#include <stdio.h>
//#include <windows.h>
#include <xtl.h>
//#include <shlobj.h>

#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsLocator.hpp"
#include "gfPaths.hpp"

namespace gfPathsPAC
{
/*
namespace
{

//============================================================================
//============================================================================
void get_cd_path(std::string& o_Path, const char* i_CDVolumeName)
{
	if (!i_CDVolumeName)
	{
		return;
	}

	//	iterate through all cd drives
	int		i;
	DWORD	drive_mask = ::GetLogicalDrives();

	// Skip over Drive A: and B: (floppies ?)
	//
	drive_mask >>= 2;
	std::string path_name = "A:\\";
	for( i = 2 ; i < 32 ; i++ )
	{
		DWORD has_drive = drive_mask & 1;
		drive_mask >>= 1;

		if( has_drive )
		{
			path_name[0] = (char)('A' + i); 
			UINT drive_type = ::GetDriveTypeA(path_name.c_str());
			
			if( DRIVE_CDROM == drive_type )
			{			
				char volume_name_buffer[MAX_PATH];
				DWORD serial_number, max_component_length, file_system_flags;
				char file_system_name[MAX_PATH];
	
				//	don't display "insert CD in drive" type messages
				//
				::SetErrorMode(SEM_FAILCRITICALERRORS);
				
				BOOL ret_val = ::GetVolumeInformation(	path_name.c_str(),
														volume_name_buffer,
														MAX_PATH,
														&serial_number,
														&max_component_length,
														&file_system_flags,
														file_system_name,
														MAX_PATH);

				::SetErrorMode(0);

				if( ret_val )
				{
					path_name.resize(2);	//	keep only drive letter and colon
					if( ::_stricmp(volume_name_buffer, i_CDVolumeName) == 0 )
					{
						o_Path = path_name;
						return;
					}
				}
			}
		}
	}

	o_Path.resize(0);
}

//============================================================================
//============================================================================
void add_path_to_locator(const std::string& i_PathName, fsLocator& o_Locator)
{
	std::string::size_type last_pos = 0;
	std::string::size_type token_pos = 0;
	std::string name;
	itString wide_char_itstring;

	while( token_pos != std::string::npos )
	{
		token_pos = i_PathName.find('\\', last_pos);

		name = i_PathName.substr(last_pos, token_pos - last_pos);

		if( name.size() > 0 )
		{
			itString::CharType wide_char_name[512];

			::MultiByteToWideChar(	::GetACP(),
									0,
									name.c_str(),
									-1,
									wide_char_name,
									512);

			wide_char_itstring = wide_char_name;

			o_Locator.Push(wide_char_itstring);
			last_pos = token_pos + 1;
		}
	}
}

void get_sysdat_path(std::string& o_SysdatPath)
{
	//	see if we can find the crazy shell library thing
	HMODULE dll = ::LoadLibraryEx("SHFolder.dll", NULL, 0);

	if( dll != NULL )
	{
		FARPROC raw_proc = ::GetProcAddress(dll, "SHGetFolderPathA");
	
		if( raw_proc )
		{
			typedef	HRESULT FAR PASCAL FolderFunc(
											HWND hwndOwner,
											int nFolder,
											HANDLE hToken,
											DWORD dwFlags,
											LPTSTR pszPath );

			typedef FolderFunc* FolderFuncPtr;

			FolderFuncPtr get_folder_path_func = FolderFuncPtr(raw_proc);

			#define CSIDL_COMMON_APPDATA 0x0023
			#define SHGFP_TYPE_CURRENT 0

			char path[MAX_PATH];
			HRESULT op_result = get_folder_path_func(	NULL,
														CSIDL_COMMON_APPDATA,
														NULL,
														SHGFP_TYPE_CURRENT,
														path);
			o_SysdatPath = path;
			::FreeLibrary(dll);
		}
	}
}

}
*/
//============================================================================
//	InitPaths handles initializing all the predefined game paths
//	
//============================================================================
void InitPaths(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName)
{
	o_Vec[gfPaths::e_ExePath].resize(1);
//	o_Vec[gfPaths::e_ExePath][0].Push("U:");	// Xbox User data directory	
//	o_Vec[gfPaths::e_ExePath][0].Push("U:\\UltimateRideX");	// Xbox User data directory	
//	o_Vec[gfPaths::e_ExePath][0].Push("UltimateRideX");	// Xbox User data directory	
	o_Vec[gfPaths::e_ExePath][0].Push("D:");	// Xbox User data directory	

	fsLocator temp = o_Vec[gfPaths::e_ExePath][0];
	std::string fname;
	fsFileUtil::LocatorToANSIFilename( temp, fname );
	const char* rname = fname.c_str();

	o_Vec[gfPaths::e_CDROMPath].resize(1);
	o_Vec[gfPaths::e_CDROMPath][0].Push("D:");	// Xbox DVD drive

	//initialize system temp path
	o_Vec[gfPaths::e_SysTempPath].resize(1);
	o_Vec[gfPaths::e_SysTempPath][0].Push("D:\\Temp");

	//	initialize system data path
	o_Vec[gfPaths::e_SystemDataPath].resize(1);
	o_Vec[gfPaths::e_SystemDataPath][0].Push("D:\\Saves");

/*
	char exePath[MAX_PATH];
	char tempPath[MAX_PATH];

	int PathLen = 0;
	//initialize executable path
	PathLen = GetModuleFileNameA(NULL, exePath, MAX_PATH);
	DBG_ASSERT0(PathLen, "Retrieval of executable directory path failed.");
	o_Vec[gfPaths::e_ExePath].resize(1);
	add_path_to_locator(exePath, o_Vec[gfPaths::e_ExePath][0]);

	//GetmoduleFileName will leave the filename at the end, so now we pop that off
	o_Vec[gfPaths::e_ExePath][0].Pop();
	
	//initialize CDROM path
	std::string cd_path;
	get_cd_path(cd_path, i_CDVolumeName);
	
	o_Vec[gfPaths::e_CDROMPath].resize(1);
	if( cd_path.size() )
		add_path_to_locator(cd_path.c_str(), o_Vec[gfPaths::e_CDROMPath][0]);
	else
		o_Vec[gfPaths::e_CDROMPath][0].Clear();

	//initialize system temp path
	PathLen = GetTempPathA(MAX_PATH, tempPath);
	DBG_ASSERT0(PathLen, "Retrieval of Temp directory path failed.");
	o_Vec[gfPaths::e_SysTempPath].resize(1);
	add_path_to_locator(tempPath, o_Vec[gfPaths::e_SysTempPath][0]);

	//	initialize system data path
	std::string sysdat_path;
	get_sysdat_path(sysdat_path);
	o_Vec[gfPaths::e_SystemDataPath].resize(1);
	if( sysdat_path.size() > 0 )
		add_path_to_locator(sysdat_path.c_str(), o_Vec[gfPaths::e_SystemDataPath][0]);
*/
}

//============================================================================
//	returns true if the a CD with the given name is present.  This function
//	can be called without initializing the gf package.
//============================================================================
bool CDInDrive(const char* i_CDVolumeName)
{
	DBG_ASSERT0(false, "Not implemented for Xbox!");
	return false;

//	std::string cd_path;
//	get_cd_path(cd_path, i_CDVolumeName);
//	return cd_path.size() > 0;
}

//========================================================================
//	SetCDPath searched drives for i_CDVolumeName and sets the path if found
//========================================================================
void SetCDPath(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName)
{
	DBG_ASSERT0(false, "Not implemented for Xbox!");
/*
	//initialize CDROM path
	std::string cd_path;
	get_cd_path(cd_path, i_CDVolumeName);
	
	o_Vec[gfPaths::e_CDROMPath].resize(1);
	if( cd_path.size() )
		add_path_to_locator(cd_path.c_str(), o_Vec[gfPaths::e_CDROMPath][0]);
	else
		o_Vec[gfPaths::e_CDROMPath][0].Clear();
*/
}

}
