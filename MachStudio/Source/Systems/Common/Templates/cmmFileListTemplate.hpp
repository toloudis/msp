/*****************************************************************************
**  cmmFileListTemplate.hpp
**
**      cmmFileListTemplate enumerates a list of a type of files in the
**	project directory.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_FILELISTTEMPLATE_HPP
#error cmmFileListTemplate.hpp multiply included
#endif
#define CMM_FILELISTTEMPLATE_HPP

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif

//============================================================================
//============================================================================
template<class xxxFileTypeConfig>
class cmmFileListTemplate
{
public:
	//------------------------------------------------------------------------
	//	Init()
	//		i_SystemDirName		- the name of the directory for this system
	//		i_SceneSubDirName	- the sub-dir to look for in the scene directory (data or textures)
	//------------------------------------------------------------------------
	static void Init( const itString& i_SystemDirName, 
					  const itString& i_SceneSubDirName )
	{
		sm_SystemDirName = i_SystemDirName; 
		sm_SceneSubDirName = i_SceneSubDirName;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static void CleanUp()
	{
	}

	//------------------------------------------------------------------------
	// Set app directory to look for files within/
	//	the post-fix directory will be appended to this directory path.
	//------------------------------------------------------------------------
	static void SetAppDirectory(const fsLocator &i_Dir)
	{
		sm_AppDir = i_Dir;
	}


	//------------------------------------------------------------------------
	// system directory name
	//------------------------------------------------------------------------
	static itString GetSystemDirName()
	{
		return sm_SystemDirName;
	}

	//------------------------------------------------------------------------
	// app directory name
	//------------------------------------------------------------------------
	static fsLocator GetAppDir()
	{
		return sm_AppDir;
	}

	//------------------------------------------------------------------------
	// Get list of files, returning into given file list
	//------------------------------------------------------------------------
	 static void BuildFileList(fsysFileList& o_FileList)
	{
		o_FileList.SetSystemDirName( sm_SystemDirName );
		o_FileList.SetFileTypeExtensions( xxxFileTypeConfig::GetFileTypeExtensions() );
		o_FileList.SetSubDirName( sm_SubDirName );
		o_FileList.SetAppDir( sm_AppDir );

		o_FileList.BuildFileList();
	}

	//------------------------------------------------------------------------
	// Get list of files, returning into given file list
	//------------------------------------------------------------------------
	 static void AppendFileList(fsysFileList& o_FileList)
	{
		o_FileList.SetSystemDirName( sm_SystemDirName );
		o_FileList.SetFileTypeExtensions( xxxFileTypeConfig::GetFileTypeExtensions() );
		o_FileList.SetSubDirName( sm_SubDirName );
		o_FileList.SetSceneSubDirName( sm_SceneSubDirName );
		o_FileList.SetAppDir( sm_AppDir );

		o_FileList.AppendFileList();
	}

private:
	static fsLocator sm_AppDir;
	static itString sm_SystemDirName; 
	static itString sm_SubDirName;
	static itString sm_SceneSubDirName;
};

//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxFileTypeConfig>
fsLocator cmmFileListTemplate<xxxFileTypeConfig>::sm_AppDir;

template<class xxxFileTypeConfig>
itString cmmFileListTemplate<xxxFileTypeConfig>::sm_SystemDirName;

template<class xxxFileTypeConfig>
itString cmmFileListTemplate<xxxFileTypeConfig>::sm_SubDirName;

template<class xxxFileTypeConfig>
itString cmmFileListTemplate<xxxFileTypeConfig>::sm_SceneSubDirName;

