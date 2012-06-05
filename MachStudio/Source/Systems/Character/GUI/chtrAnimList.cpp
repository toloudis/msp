/*****************************************************************************
**  chtrAnimList.cpp
**
**      chtrAnimList holds a list of the props.
**
**	StudioGPU
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrAnimList.hpp"

#include "Support/fsys/fsysFileList.hpp"
#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Support/fsys/fsysFileUtil.hpp"

namespace
{
	const itString c_FileTypeExtensions("cha;jna;htr;gab");

	fsLocator l_AppDir;
	itString l_SystemDirName; 
	itString l_SceneSubDirName;

} // end of anonymous namespace



//------------------------------------------------------------------------
//	Init()
//		i_SystemDirName		- the name of the directory for this system
//		i_SceneSubDirName		- the sub-dir to look for extensions (data, model,...)
//------------------------------------------------------------------------
void chtrAnimList::Init( const itString& i_SystemDirName, const itString& i_SceneSubDirName )
{
	l_SystemDirName = i_SystemDirName; 
	l_SceneSubDirName = i_SceneSubDirName;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void chtrAnimList::CleanUp()
{
}

//------------------------------------------------------------------------
// Set app directory to look for geometry files within/
//	the post-fix directory will be appended to this directory path.
//------------------------------------------------------------------------
void chtrAnimList::SetAppDirectory(const fsLocator &i_Dir)
{
	l_AppDir = i_Dir;
}

//------------------------------------------------------------------------
// system directory name
//------------------------------------------------------------------------
itString chtrAnimList::GetSystemDirName()
{
	return l_SystemDirName;
}

//------------------------------------------------------------------------
// Get list of animation files, returning into given file list.
// Pass in directory of the character file (.chd or .chx) in order
// to locate the animations for this character only. 
// The directory should be the result of chtrScriptObject::GetDirectory().
//------------------------------------------------------------------------
void chtrAnimList::BuildFileList(fsysFileList& o_FileList, 
								 const fsLocator& i_CharacterDirectory)
{
	o_FileList.SetSystemDirName( l_SystemDirName );
	o_FileList.SetFileTypeExtensions( c_FileTypeExtensions );
	o_FileList.SetSceneSubDirName( l_SceneSubDirName );
	o_FileList.SetAppDir( l_AppDir );

	// Get directory name for character in order to use it as the "object directory"
	fsLocator object_dir = i_CharacterDirectory;
	object_dir.Pop(); // Pop off "Data" or "Models"
	itString obj_name = object_dir.GetLastName();
	o_FileList.SetObjectDirName( obj_name );

	o_FileList.BuildFileList();

}

//------------------------------------------------------------------------
// GetAnimationLocator - get locator to full path to animation
//	file with given filename for the character that was loaded
//	from the given directory.
//------------------------------------------------------------------------
fsLocator chtrAnimList::GetAnimationLocator(const itString& i_Filename, 
											const fsLocator& i_CharacterDirectory)
{
	// Get directory name for character in order to use it as the "object directory"
	fsLocator object_dir = i_CharacterDirectory;
	if (object_dir.GetNumNames() > 0)
		object_dir.Pop();
	//itString obj_name = object_dir.GetLastName();

	fsLocator anim_loc;
	if (fsysFileUtil::GetFilePath(chtrAnimList::GetSystemDirName(),
								  itString(gfPaths::GetSubPath(gfPaths::e_Data)), 
								  i_Filename, anim_loc) )
	{
		return anim_loc;
	}
	else
	{
		anim_loc.Clear();
		anim_loc.Push(i_Filename);
		return anim_loc;
	}
	
	// Don't throw exception if not found anymore, just return 
	// filename if a fullpath is not found. The single filename
	// will trigger a resolve full path dialog for the user.

	//else
	//{
	//	// Not found, throw exception
	//	//
	//	fsLocator missing_loc = object_dir;
	//	missing_loc.Push( gfPaths::GetSubPath(gfPaths::e_Data) );
	//	if (i_Filename.GetLength() > 0)
	//	{
	//		missing_loc.Push( i_Filename );
	//	}
	//	//else
	//	//{
	//	//	missing_loc.Push( itString("[No Anim File Specified]") );
	//	//}
	//	std::string fname;
	//	fsFileUtil::LocatorToANSIFilename( missing_loc, fname );
	//	DBG_ERROR1( "chtrAnimList::GetAnimationLocator Error [%s]", fname.c_str() );
	//	throw fsFileDoesntExistX( missing_loc );

	//	return fsLocator();
	//}
}

