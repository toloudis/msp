/*****************************************************************************
**  chtrGeomList.hpp
**
**      chtrGeomList holds a list of the Character templates.
**
**	StudioGPU
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_GEOMLIST_HPP
#error chtrGeomList.hpp multiply included
#endif
#define CHTR_GEOMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif


//============================================================================
// Sets file extensions for this file lister
//============================================================================
class chtrGeomListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("gxb;mhx;jnx;vtx;mx;chx;chd");
		//return itString("chx;chd;gxb");
	}
};


//============================================================================
// Enumerates geometry in Character directories
//============================================================================
class chtrGeomList : public cmmFileListTemplate<chtrGeomListConfig>
{
public:
	//------------------------------------------------------------------------
	// Get list of files, returning into given file list
	//------------------------------------------------------------------------

	//bga - by commenting this function out, I am trying to get the Character system
	// to look throughout the full "Data" directory for any sort of geometry file.
	// This is the behavior that the Props systme had before the switch to fullpaths
	// and the renaming of Characters as "Objects" in the system dialog.

	//static void chtrGeomList::BuildFileList(fsysFileList& o_FileList)
	//{
	//	// IF we allow the file list to search sub directories, then we don't have
	//	// to do two searches with different SubDirNames.
	//	o_FileList.SetSystemDirName( GetSystemDirName() );
	//	o_FileList.SetFileTypeExtensions( chtrGeomListConfig::GetFileTypeExtensions() );
	//	// Not doing a subdirectory name, but allowing subdirectory search.
	//	// This allows the search to find geometry in both Data and Models.
	//	//o_FileList.SetSubDirName( itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
	//	o_FileList.SetSearchSubDirectories(true);
	//	// Scene directory should always look under "Data" for geometry.
	//	o_FileList.SetSceneSubDirName( itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
	//	o_FileList.SetAppDir( GetAppDir() );
	//	o_FileList.BuildFileList();

	//	// Append CHX files
	//	//o_FileList.SetFileTypeExtensions( chtrGeomListConfig::GetFileTypeExtensions() );
	//	//o_FileList.SetSubDirName( itString(gfPaths::GetSubPath(gfPaths::e_Models)) );
	//	//o_FileList.AppendFileList();
	//};
};
