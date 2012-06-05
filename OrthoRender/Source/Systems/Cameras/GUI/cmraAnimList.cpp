/*****************************************************************************
**  cmraAnimList.cpp
**
**      cmraAnimList holds a list of the animation files for custom animation.
**
**	Extra Large Technology
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/GUI/cmraAnimList.hpp"

#include "Support/fsys/fsysFileList.hpp"

namespace
{
	const itString c_FileTypeExtensions(".cam");

	fsLocator l_AppDir;
	itString l_SystemDirName; 
	itString l_SceneSubDirName;

} // end of anonymous namespace


//------------------------------------------------------------------------
//	Init()
//		i_SystemDirName		- the name of the directory for this system
//		i_SubDirName		- the sub-dir to look for extensions (data, model,...)
//------------------------------------------------------------------------
void cmraAnimList::Init( const itString& i_SystemDirName, const itString& i_SceneSubDirName )
{
	l_SystemDirName = i_SystemDirName; 
	l_SceneSubDirName = i_SceneSubDirName;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmraAnimList::CleanUp()
{
}

//------------------------------------------------------------------------
// Set app directory to look for geometry files within/
//	the post-fix directory will be appended to this directory path.
//------------------------------------------------------------------------
void cmraAnimList::SetAppDirectory(const fsLocator &i_Dir)
{
	l_AppDir = i_Dir;
}

//------------------------------------------------------------------------
// system directory name
//------------------------------------------------------------------------
itString cmraAnimList::GetSystemDirName()
{
	return l_SystemDirName;
}

//------------------------------------------------------------------------
// Get list of animation files, returning into given file list
//------------------------------------------------------------------------
void cmraAnimList::BuildFileList(fsysFileList& o_FileList)
{
	o_FileList.SetSystemDirName( l_SystemDirName );
	o_FileList.SetFileTypeExtensions( c_FileTypeExtensions );
	o_FileList.SetSceneSubDirName( l_SceneSubDirName );
	o_FileList.SetAppDir( l_AppDir );

	o_FileList.BuildFileList();

}

